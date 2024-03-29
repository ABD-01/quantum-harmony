'''\
Copyright 2024-2025 Accolade Electronics Pvt. Ltd

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

file        app_logic.py
brief       This is the script for handling application logic and background communication

date        22 March 2024
author      Eshwar J <eshwar.jorvekar@accoladeelectronics.com>
'''

import app_ui                       # for using the GUI constructs to interact with FSM in thread

g_ui_root = app_ui.create_gui()
g_log_window = app_ui.create_log_window(g_ui_root)

def start():
    print('app_logic : started app')

    # Start the background thread for finite state machine
    fsm_thread.start()

    # Create GUI buttons and assign functions to call on user 'click'
    app_ui.create_buttons(g_ui_root, browse_file, boot_lock, upload_file, reset_ecu)
    app_ui.append_log("Software usage is restricted to Accolade service engineers or representatives only\r\n\n")

    # Bind the GUI closing event for destroying thread
    app_ui.bind_close_event(g_ui_root, on_close)

    # Run the GUI
    app_ui.run_gui(g_ui_root)

########################################## (GUI EVENT HANDLING) #####################################################
    
from tkinter import filedialog  # for file browse dialog

chunk_size = 254

def browse_file():
    global g_file_path
    g_file_path = filedialog.askopenfilename()
    global g_file_size
    if g_file_path:
        path =  "selected file " + g_file_path
        print(f'app_logic : selected file {g_file_path}')
        app_ui.append_log(path)
        try:
            g_file_size = os.path.getsize(g_file_path)

            app_ui.set_btn_enabled('BROWSE_BTN', False)
            app_ui.set_btn_enabled('BOOT_LOCK_BTN', True)
            app_ui.set_btn_enabled('UPLOAD_BTN', False)
            app_ui.set_btn_enabled('RESET_ECU_BTN', False)

        except:
            print("app_logic : exception occured during getting file size")
            return False

def boot_lock():
    print('app_logic : trying to lock into bootloader...')
    app_ui.append_log('trying to lock into bootloader...')

    app_ui.set_btn_enabled('BROWSE_BTN', False)
    app_ui.set_btn_enabled('BOOT_LOCK_BTN', False)
    app_ui.set_btn_enabled('UPLOAD_BTN', True)
    app_ui.set_btn_enabled('RESET_ECU_BTN', False)
    
    fsm_thread.fsm_transition_boot_lock()

def upload_file():
    print('app_logic : initiating file upload...')
    app_ui.append_log('initiating file upload...')
    fsm_thread.fsm_transition_upload_file()

def reset_ecu():
    print('app_logic : resetting ecu...')
    app_ui.append_log('resetting ecu...')

    handle = g_client_handle
    config = g_config
    testECUReset(handle, config)

########################################## (MULTI-THREADING) #####################################################

import threading                    # for threading
import time                         # for sleep

class FsmThread(threading.Thread):
    def __init__(self, stop_event):
        super(FsmThread, self).__init__()
        self._stop_event = stop_event
        self._lock = threading.Lock()
        self._state = 'IDLE'
        print('app_logic : started comm thread with', self._state, 'state')

    def run(self):
        while not self._stop_event.is_set():
            with self._lock:
                if self._state == 'IDLE':
                    pass
                elif self._state == 'BOOT_LOCK':
                    can_init()
                    handle = g_client_handle
                    config = g_config
                    if testTesterPresent(handle, config) == False:
                        print('app_logic : tester present test failed')
                    self._state = 'IDLE'
                elif self._state == 'FILE_UPLOAD':
                    perform_service_tests()
                    self._state = 'IDLE'
            time.sleep(0.1)
        objPCANUds.Uninitialize_2013(g_client_handle)

    def fsm_transition_boot_lock(self):
        with self._lock:
            self._state = 'BOOT_LOCK'
    
    def fsm_transition_upload_file(self):
        with self._lock:
            self._state = 'FILE_UPLOAD'

    def stop_thread(self):
        self._stop_event.set()
        self.join()
        print('app_logic : stopping comm thread')

# Create a stop event to signal the thread to stop
stop_event = threading.Event()

# Create an instance of the thread
fsm_thread = FsmThread(stop_event)

def stop_thread():
    fsm_thread.stop_thread()

# Function to handle application exit
def on_close():
    stop_thread()
    g_ui_root.destroy()
    print('app_logic : destroyed ui root')

########################################### (UDS over CAN: core initialisation) ####################################################

from PCAN_UDS_2013 import *     # for UDS stack
import copy                     # for deep copy

# A global counter to keep track of the number of failed tests (see display_uds_msg function)
g_nbErr = 0

# import the dlls required for stack
import os
import sys
from pathlib import Path

dll_path = Path(__file__).resolve().parent #/ "uds_stack"
sys.path.insert(0, str(dll_path))
os.add_dll_directory(str(dll_path))

# create uds library object
objPCANUds = PCAN_UDS_2013()

g_client_handle = PCANTP_HANDLE_USBBUS1
g_config = uds_msgconfig()
g_timeout_request = c_uint32(0)

BUFFER_SIZE = 256

def test_result_to_string(test):
    return "Success" if test else "Fail"

def print_test_status(test):
    return test_result_to_string(objPCANUds.StatusIsOk_2013(test, PUDS_STATUS_OK, False))

def can_init():
    # Print version information
    buffer = create_string_buffer(BUFFER_SIZE)
    status = objPCANUds.GetValue_2013(PCANTP_HANDLE_NONEBUS, PUDS_PARAMETER_API_VERSION, buffer, BUFFER_SIZE)
    print("app_logic : PCAN-UDS API Version - %s: %s" % (buffer.value, print_test_status(status)))

    # Initialize channel
    status = objPCANUds.Initialize_2013(g_client_handle, PCANTP_BAUDRATE_500K, 0, 0, 0)
    print("app_logic : Initialize channel: %s" % (print_test_status(status)))
    
    # Get timeout values
    status = objPCANUds.GetValue_2013(g_client_handle, PUDS_PARAMETER_TIMEOUT_REQUEST, g_timeout_request,
                                    sizeof(g_timeout_request))
    print("app_logic : Get request timeout value (%ums): %s" % (g_timeout_request.value, print_test_status(status)))
    customTimeOut = c_uint32(g_timeout_request.value * 2)
    status = objPCANUds.SetValue_2013(g_client_handle,
                                    PUDS_PARAMETER_TIMEOUT_REQUEST,
                                    customTimeOut, sizeof(customTimeOut))
    print("app_logic : Set request timeout value (%ums): %s" % (customTimeOut.value, print_test_status(status)))
    status = objPCANUds.GetValue_2013(g_client_handle, PUDS_PARAMETER_TIMEOUT_REQUEST, g_timeout_request,
                                    sizeof(g_timeout_request))
    print("app_logic : Get request timeout value (%ums): %s" % (g_timeout_request.value, print_test_status(status)))

    timeout_response = c_uint32(0)
    status = objPCANUds.GetValue_2013(g_client_handle, PUDS_PARAMETER_TIMEOUT_RESPONSE, timeout_response,
                                    sizeof(timeout_response))
    print("app_logic : Get response timeout value (%ums): %s" % (timeout_response.value, print_test_status(status)))
    customTimeOut = c_uint32(timeout_response.value * 2)
    status = objPCANUds.SetValue_2013(g_client_handle,
                                    PUDS_PARAMETER_TIMEOUT_RESPONSE,
                                    customTimeOut, sizeof(customTimeOut))
    print("app_logic : Set response timeout value (%ums): %s" % (customTimeOut.value, print_test_status(status)))
    status = objPCANUds.GetValue_2013(g_client_handle, PUDS_PARAMETER_TIMEOUT_RESPONSE, timeout_response,
                                    sizeof(timeout_response))
    print("app_logic : Get response timeout value (%ums): %s" % (timeout_response.value, print_test_status(status)))

    # CAN-TP PRIORITY bits.
    can_tp_prio = c_uint32(0)
    status = objPCANUds.GetValue_2013(g_client_handle, PUDS_PARAMETER_J1939_PRIORITY, can_tp_prio,
                                    sizeof(can_tp_prio))
    print("app_logic : Get default PUDS_PARAMETER_J1939_PRIORITY (%ums): %s" % (can_tp_prio.value, print_test_status(status)))
    can_tp_prio = c_uint32(3)
    status = objPCANUds.SetValue_2013(g_client_handle,
                                    PUDS_PARAMETER_J1939_PRIORITY,
                                    can_tp_prio, sizeof(can_tp_prio))
    print("app_logic : New PUDS_PARAMETER_J1939_PRIORITY (%ums): %s" % (can_tp_prio.value, print_test_status(status)))

    status = objPCANUds.AddCanIdFilter_2013(g_client_handle, 0xCDAF133)
    print("app_logic : Add can id filter (0xCDAF133): %s" % (print_test_status(status)))

    # Define Network Address Information used for all the tests
    g_config.can_msgtype = PCANTP_CAN_MSGTYPE_EXTENDED
    g_config.nai.protocol = PUDS_MSGPROTOCOL_ISO_15765_2_29B_FIXED_NORMAL
    g_config.nai.target_type = PCANTP_ISOTP_ADDRESSING_PHYSICAL
    g_config.type = PUDS_MSGTYPE_USDT
    g_config.nai.source_addr = PUDS_ISO_15765_4_ADDR_TEST_EQUIPMENT
    g_config.nai.target_addr = PUDS_ISO_15765_4_ADDR_ECU_1

########################################### (UDS over CAN: service tests) ####################################################
    
def perform_service_tests():
    handle = g_client_handle
    config = g_config

    # used for write DID
    lastProgramming = (c_uint8 * 5)(1, 2, 3, 4, 5)
    shopCode = (c_uint8 * 5)(1, 2, 3, 4, 5)
    downloadingSite = (c_uint8 * 5)(1, 2, 3, 4, 5)

    # used for read DID
    read_did = (c_uint16 * 1)(0x5405)

    if testReadDataByIdentifier(handle, config, read_did) == False:
        print("app_logic : read data by id test failed")
    elif testControlDTCSetting(handle, config, objPCANUds.PUDS_SVC_PARAM_CDTCS_OFF) == False:
        print('app_logic : control DTC setting test failed')
    elif testDiagnosticSessionControl(handle, config) == False:
        print("Accessing session control fail")
    elif testSecurityAccess(handle, config) == False:
        print("Fail Security Access")
    elif testLinkControl(handle, config) == False:
        print("Link Control Fail")
    elif testAccessTimingParameter(handle, config) == False:
        print("Access timing Parameter Fail")
    elif testWriteDataByIdentifier(handle, config, 0x5407, lastProgramming, 5) == False:
        print("Last programming  Write DID fail")
    elif testWriteDataByIdentifier(handle, config, 0x5408, shopCode, 5) == False:
        print("Shop code Write DID fail")
    elif testWriteDataByIdentifier(handle, config, 0x5409, downloadingSite, 5) == False:
        print("Download site Write DID fail")
    elif testRoutineControlFlashErase(handle, config) == False:
        print("Routine control for Flash erase fail")
    elif testRequestDownload(handle, config) == False:
        print("Request Download Fail")
    elif testTrasferFile(handle, config) == False:
        print("Transfer Data Fail")
    # elif testRequestTransferExit(handle, config) == False:
    #     print("Transfer Exit Fail")
    # elif testRoutineControlCrcCheck(handle, config) == False:
    #     print("CRC check fail")
    # elif testECUReset(handle, config) == False:
    #     print("Ecu reset fail")
        
def testTesterPresent(channel, config):
    request = uds_msg()
    response = uds_msg()
    confirmation = uds_msg()

    print('app_logic : broadcasting tester present for 5 seconds')
    result = False
    
    start_time = time.time()

    # Send tester present for 5 seconds so that the bootloader can be locked
    while time.time() - start_time < 5:
        status = objPCANUds.SvcTesterPresent_2013(channel, config, request, objPCANUds.PUDS_SVC_PARAM_TP_ZSUBF)
        #print("app_logic : execute tester present service: %s" % (print_test_status(status)))

        # if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        #     status = objPCANUds.WaitForService_2013(channel, request, response, confirmation)
        # if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        #     result = display_uds_msg_validate(confirmation, response, False)
        # else:
        #     result = display_uds_msg_validate(request, None, False)

    #print("app_logic : TesterPresent: " + test_result_to_string(result))
    status = objPCANUds.MsgFree_2013(request)
    status = objPCANUds.MsgFree_2013(response)
    status = objPCANUds.MsgFree_2013(confirmation)
    return result

def testReadDataByIdentifier(channel, config, did):
    request = uds_msg()
    response = uds_msg()
    confirmation = uds_msg()
    result = False

    print("\n\n*** UDS Service: ReadDataByIdentifier ***")

    # Sends a physical ReadDataByIdentifier message
    status = objPCANUds.SvcReadDataByIdentifier_2013(channel, config, request, did, 1)
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        status = objPCANUds.WaitForService_2013(channel, request, response, confirmation)
    print(" UDS_SvcReadDataByIdentifier_2013: %i" % (status.value))
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        result = display_uds_msg_validate(confirmation, response, False)
    else:
        result = display_uds_msg_validate(request, None, False)

    print("UDS Service: ReadDataByIdentifier [" + hex(did[0]) + "] :" + test_result_to_string(result))

    # Free messages
    status = objPCANUds.MsgFree_2013(request)
    status = objPCANUds.MsgFree_2013(response)
    status = objPCANUds.MsgFree_2013(confirmation)
    return result

def testControlDTCSetting(channel, config, controlParameter):
    request = uds_msg()
    response = uds_msg()
    confirmation = uds_msg()
    result = False

    # Sends a physical ControlDTCSetting message
    print("\n\nSends a physical ControlDTCSetting message: ")
    dtc_setting_control_option_record = (c_uint8 * 1)(0x00)
    status = objPCANUds.SvcControlDTCSetting_2013(channel, config, request, controlParameter,
                                                  dtc_setting_control_option_record, 0)
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        status = objPCANUds.WaitForService_2013(channel, request, response, confirmation)
    print(" UDS_SvcControlDTCSetting_2013: %i" % (status.value))
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        result = display_uds_msg_validate(confirmation, response, False)
    else:
        result = display_uds_msg_validate(request, None, False)

    print("UDS Service: ControlDTCSetting: " + test_result_to_string(result))

    # Free messages
    status = objPCANUds.MsgFree_2013(request)
    status = objPCANUds.MsgFree_2013(response)
    status = objPCANUds.MsgFree_2013(confirmation)
    return result

def testDiagnosticSessionControl(channel, config):
    request = uds_msg()
    response = uds_msg()
    session_info = uds_sessioninfo()
    confirmation = uds_msg()
    status = objPCANUds.SvcDiagnosticSessionControl_2013(channel, config, request, objPCANUds.PUDS_SVC_PARAM_DSC_ECUPS)
    print("Execute Diagnostic Session Control service: %s" % (print_test_status(status)))
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        status = objPCANUds.WaitForService_2013(channel, request, response, confirmation)
    result = False
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        result = display_uds_msg_validate(confirmation, response, False)

    else:
        result = display_uds_msg_validate(request, None, False)

    print("UDS Service: DiagnosticSessionControl: " + test_result_to_string(result))

    status = objPCANUds.MsgFree_2013(request)
    status = objPCANUds.MsgFree_2013(response)
    status = objPCANUds.MsgFree_2013(confirmation)
    return result

def testSecurityAccess(channel, config):
    request = uds_msg()
    response = uds_msg()
    confirmation = uds_msg()
    value_little_endian = 0xF0A1B2C3
    security_access_data = Reverse32(value_little_endian)
    status = objPCANUds.SvcSecurityAccess_2013(channel, config, request, objPCANUds.PUDS_SVC_PARAM_SA_RSD_1,
                                               security_access_data, 4)
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        status = objPCANUds.WaitForService_2013(channel, request, response, confirmation)
    print(" UDS_SvcSecurityAccess_2013: %i" % (status.value))
    result = False
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        result = display_uds_msg_validate(confirmation, response, False)
    else:
        result = display_uds_msg_validate(request, None, False)
    status = objPCANUds.MsgFree_2013(request)
    status = objPCANUds.MsgFree_2013(response)
    status = objPCANUds.MsgFree_2013(confirmation)

    if result == True:
        status = objPCANUds.SvcSecurityAccess_2013(channel, config, request,
                                                   objPCANUds.PUDS_SVC_PARAM_SA_SK_2,
                                                   security_access_data, 4)
        if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
            status = objPCANUds.WaitForService_2013(channel, request, response, confirmation)
        print(" UDS_SvcSecurityAccess_2013: %i" % (status.value))
        if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
            result = display_uds_msg_validate(confirmation, response, False)
        else:
            result = display_uds_msg_validate(request, None, False)

    print("UDS Service: SecurityAccess: " + test_result_to_string(result))

    return result

def testLinkControl(channel, config):
    request = uds_msg()
    response = uds_msg()
    confirmation = uds_msg()
    result = False

    # Sends a physical LinkControl message
    print("\n\nSends a physical LinkControl message (Verify Fixed Baudrate): ")
    status = objPCANUds.SvcLinkControl_2013(channel, config, request, objPCANUds.PUDS_SVC_PARAM_LC_VBTWFBR,
                                            objPCANUds.PUDS_SVC_PARAM_LC_BAUDRATE_CAN_500K, 0)
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        status = objPCANUds.WaitForService_2013(channel, request, response, confirmation)
    print(" UDS_SvcLinkControl_2013: %i" % (status.value))
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        result = display_uds_msg_validate(confirmation, response, False)
    else:
        result = display_uds_msg_validate(request, None, False)

    print("UDS Service: LinkControl: " + test_result_to_string(result))

    # Free messages
    status = objPCANUds.MsgFree_2013(request)
    status = objPCANUds.MsgFree_2013(response)
    status = objPCANUds.MsgFree_2013(confirmation)
    return result

def testAccessTimingParameter(channel, config):
    request_record = create_string_buffer(b'\xAB\xCD')
    record_size = c_uint32(2)

    request = uds_msg()
    response = uds_msg()
    confirmation = uds_msg()
    request_record[0] = getCChar(0xAB)
    request_record[1] = getCChar(0xCD)

    # Sends a physical AccessTimingParameter message
    print("\n\nSends a physical AccessTimingParameter message: ")
    status = objPCANUds.SvcAccessTimingParameter_2013(channel, config, request, objPCANUds.PUDS_SVC_PARAM_ATP_RCATP,
                                                      request_record, record_size)
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        status = objPCANUds.WaitForService_2013(channel, request, response, confirmation)
    print(" UDS_SvcAccessTimingParameter_2013: %d" % (status.value))
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        result = display_uds_msg_validate(confirmation, response, False)
    else:
        result = display_uds_msg_validate(request, None, False)

    print("UDS Service: AccessTimingParameter: " + test_result_to_string(result))

    # Free messages
    status = objPCANUds.MsgFree_2013(request)
    status = objPCANUds.MsgFree_2013(response)
    status = objPCANUds.MsgFree_2013(confirmation)
    return result

def testWriteDataByIdentifier(channel, config, did, data_record, data_record_size):
    request = uds_msg()
    response = uds_msg()
    confirmation = uds_msg()

    # Sends a physical WriteDataByIdentifier message
    print("\n\nSends a physical WriteDataByIdentifier message: ")
    status = objPCANUds.SvcWriteDataByIdentifier_2013(channel, config, request, did, data_record, data_record_size)
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        status = objPCANUds.WaitForService_2013(channel, request, response, confirmation)
    print(" UDS_SvcWriteDataByIdentifier_2013: %i" % (status.value))
    result = False
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        result = display_uds_msg_validate(confirmation, response, False)
    else:
        result = display_uds_msg_validate(request, None, False)

    print("UDS Service: WriteDataByIdentifier [" + hex(did) + "] :" + test_result_to_string(result))

    # Free messages
    status = objPCANUds.MsgFree_2013(request)
    status = objPCANUds.MsgFree_2013(response)
    status = objPCANUds.MsgFree_2013(confirmation)
    return result

def testRoutineControlFlashErase(channel, config):
    routine_control_option_record = create_string_buffer(1)
    routine_control_option_record_size = 1

    request = uds_msg()
    response = uds_msg()
    confirmation = uds_msg()
    result = False

    print("\n\n*** UDS Service: RoutineControl Erasing Flash***")

    # Sends a physical RoutineControl message
    print("\n\nSends a physical RoutineControl message: ")
    for i in range(routine_control_option_record_size):
        routine_control_option_record[i] = 0x0

    status = objPCANUds.SvcRoutineControl_2013(channel, config, request, objPCANUds.PUDS_SVC_PARAM_RC_STR, 0x2001,
                                               routine_control_option_record, routine_control_option_record_size)
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        status = objPCANUds.WaitForService_2013(channel, request, response, confirmation)
    print(" UDS_SvcRoutineControl_2013 Start: %i" % (status.value))

    status = objPCANUds.MsgFree_2013(request)
    status = objPCANUds.MsgFree_2013(response)
    status = objPCANUds.MsgFree_2013(confirmation)

    request = uds_msg()
    response = uds_msg()
    confirmation = uds_msg()

    status = objPCANUds.SvcRoutineControl_2013(channel, config, request, objPCANUds.PUDS_SVC_PARAM_RC_STPR, 0x2001,
                                               routine_control_option_record, routine_control_option_record_size)
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        status = objPCANUds.WaitForService_2013(channel, request, response, confirmation)
    print(" UDS_SvcRoutineControl_2013 Stop: %i" % (status.value))

    status = objPCANUds.MsgFree_2013(request)
    status = objPCANUds.MsgFree_2013(response)
    status = objPCANUds.MsgFree_2013(confirmation)

    request = uds_msg()
    response = uds_msg()
    confirmation = uds_msg()

    status = objPCANUds.SvcRoutineControl_2013(channel, config, request, objPCANUds.PUDS_SVC_PARAM_RC_RRR, 0x2001,
                                               routine_control_option_record, routine_control_option_record_size)
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        status = objPCANUds.WaitForService_2013(channel, request, response, confirmation)
    print(" UDS_SvcRoutineControl_2013 Result: %i" % (status.value))
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        result = display_uds_msg_validate(confirmation, response, False)
    else:
        result = display_uds_msg_validate(request, None, False)

    print("UDS Service: RoutineControl Erasing Flash: " + test_result_to_string(result))

    # Free messages
    status = objPCANUds.MsgFree_2013(request)
    status = objPCANUds.MsgFree_2013(response)
    status = objPCANUds.MsgFree_2013(confirmation)
    return result

def testRequestDownload(channel, config):
    memory_address_buffer = create_string_buffer(4)
    memory_size_buffer = create_string_buffer(4)
    memory_address_size = 4
    memory_size_size = 4

    request = uds_msg()
    response = uds_msg()
    confirmation = uds_msg()

    print("\n\n*** UDS Service: RequestDownload ***")

    # Sends a physical RequestDownload message
    for i in range(memory_address_size):
        memory_address_buffer[i] = 0xFF
        memory_size_buffer[i] = (g_file_size & (0x01 < i)) > i
        print(memory_size_buffer[i])
        print(" ")

    status = objPCANUds.SvcRequestDownload_2013(channel, config, request, 0x0, 0x0, memory_address_buffer,
                                                memory_address_size, memory_size_buffer,
                                                memory_size_size)
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        status = objPCANUds.WaitForService_2013(channel, request, response, confirmation)
    result = False
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        result = display_uds_msg_validate(confirmation, response, False)
    else:
        result = display_uds_msg_validate(request, None, False)

    print("UDS Service: RequestDownload: " + test_result_to_string(result))

    # Free messages
    status = objPCANUds.MsgFree_2013(request)
    status = objPCANUds.MsgFree_2013(response)
    status = objPCANUds.MsgFree_2013(confirmation)
    return result

def testTrasferFile(channel, config):
    status = True
    simulate_file_transfer(channel, config)
    # file_transfer_thread = threading.Thread(target=simulate_file_transfer, args=(channel, config))
    # file_transfer_thread.start()
    # file_transfer_thread.join()
    # while not fileOperationComplete:
        # status = fileTransferStatus
        # time.sleep(0.1)
    return status

################################################## TMP START

def readFileInChunk(offset, chunk_size):
    with open(g_file_path, 'rb') as file:
        file.seek(offset)
        readChukData = file.read(chunk_size)
    return readChukData, len(readChukData)


def update_progress_bar(file_size, fileOffset):
    percentage = (fileOffset / file_size) * 100
    print('upload percent {percentage}')
    app_ui.append_log(f'{percentage :.1f}')
    #progress_bar["value"] = percentage

    if(percentage == 100):
        app_ui.set_btn_enabled('BROWSE_BTN', False)
        app_ui.set_btn_enabled('BOOT_LOCK_BTN', False)
        app_ui.set_btn_enabled('UPLOAD_BTN', False)
        app_ui.set_btn_enabled('RESET_ECU_BTN', True)

def simulate_file_transfer(channel, config):
    print(f'transferring file {g_file_size} {g_file_path}')
    sequence = 0
    fileOffset = 0
    index = 0
    status = True
    global fileTransferStatus
    global fileOperationComplete
    fileTransferStatus = False
    fileOperationComplete = True
    while index < g_file_size - 1:
        data, read_bytes = readFileInChunk(fileOffset, chunk_size)
        print(f"Offset : {fileOffset}, read_bytes : {read_bytes}, seq : {sequence}")
        fileOffset = read_bytes + fileOffset
        update_progress_bar(g_file_size, fileOffset)
        index = fileOffset
        print(f"value {index} {g_file_size}")
        status &= testTransferData(channel, config, read_bytes, data, sequence)
        sequence = sequence + 1
    if index == g_file_size:
        fileTransferStatus = True
    else:
        fileTransferStatus = False
    fileOperationComplete = True
    return

def testTransferData(channel, config, size, buffer, sequence):
    record = create_string_buffer(size)
    record_size = size

    request = uds_msg()
    response = uds_msg()
    confirmation = uds_msg()

    for i in range(record_size):
        record[i] = buffer[i]

    status = objPCANUds.SvcTransferData_2013(channel, config, request, sequence, record, record_size)
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        status = objPCANUds.WaitForService_2013(channel, request, response, confirmation)
    print(" UDS_SvcTransferData_2013: %i" % (status.value))
    result = False
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        result = display_uds_msg_validate(confirmation, response, False)
    else:
        result = display_uds_msg_validate(request, None, False)

    print("UDS Service: TransferData: [" + str(sequence) + "] :" + test_result_to_string(result))

    # Free messages
    status = objPCANUds.MsgFree_2013(request)
    status = objPCANUds.MsgFree_2013(response)
    status = objPCANUds.MsgFree_2013(confirmation)
    return result

################################################## TMP END

def testRequestTransferExit(channel, config):
    record = create_string_buffer(50)
    record_size = 2

    request = uds_msg()
    response = uds_msg()
    confirmation = uds_msg()

    print("\n\n*** UDS Service: RequestTransferExit ***")

    for i in range(record_size):
        record[i] = 0xFF

    status = objPCANUds.SvcRequestTransferExit_2013(channel, config, request, record, record_size)
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        status = objPCANUds.WaitForService_2013(channel, request, response, confirmation)
    result = False
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        result = display_uds_msg_validate(confirmation, response, False)
    else:
        result = display_uds_msg_validate(request, None, False)

    print("UDS Service: RequestTransferExit: " + test_result_to_string(result))

    # Free messages
    status = objPCANUds.MsgFree_2013(request)
    status = objPCANUds.MsgFree_2013(response)
    status = objPCANUds.MsgFree_2013(confirmation)
    return result

def testRoutineControlCrcCheck(channel, config):
    routine_control_option_record = create_string_buffer(2)
    routine_control_option_record_size = 2

    request = uds_msg()
    response = uds_msg()
    confirmation = uds_msg()
    result = False

    print("\n\n*** UDS Service: RoutineControl Erasing Flash***")

    # Sends a physical RoutineControl message
    print("\n\nSends a physical RoutineControl message: ")
    for i in range(routine_control_option_record_size):
        routine_control_option_record[i] = 0x0

    status = objPCANUds.SvcRoutineControl_2013(channel, config, request, objPCANUds.PUDS_SVC_PARAM_RC_STR, 0x2002,
                                               routine_control_option_record, routine_control_option_record_size)
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        status = objPCANUds.WaitForService_2013(channel, request, response, confirmation)
    print(" UDS_SvcRoutineControl_2013 Start: %i" % (status.value))

    status = objPCANUds.MsgFree_2013(request)
    status = objPCANUds.MsgFree_2013(response)
    status = objPCANUds.MsgFree_2013(confirmation)

    request = uds_msg()
    response = uds_msg()
    confirmation = uds_msg()

    status = objPCANUds.SvcRoutineControl_2013(channel, config, request, objPCANUds.PUDS_SVC_PARAM_RC_RRR, 0x2002,
                                               routine_control_option_record, routine_control_option_record_size)
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        status = objPCANUds.WaitForService_2013(channel, request, response, confirmation)
    print(" UDS_SvcRoutineControl_2013 Result: %i" % (status.value))
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        result = display_uds_msg_validate(confirmation, response, False)
    else:
        result = display_uds_msg_validate(request, None, False)

    print("UDS Service: RoutineControl Erasing Flash: " + test_result_to_string(result))

    # Free messages
    status = objPCANUds.MsgFree_2013(request)
    status = objPCANUds.MsgFree_2013(response)
    status = objPCANUds.MsgFree_2013(confirmation)
    return result

def testECUReset(channel, config):
    request = uds_msg()
    response = uds_msg()
    confirmation = uds_msg()

    print("\n\n*** UDS Service: ECUReset ***")

    # Sends a physical ECUReset message
    print("\n\nSends a physical ECUReset message: ")
    status = objPCANUds.SvcECUReset_2013(channel, config, request, objPCANUds.PUDS_SVC_PARAM_ER_HR)
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        status = objPCANUds.WaitForService_2013(channel, request, response, confirmation)
    print(" UDS_SvcECUReset_2013: %i" % (status.value))
    if objPCANUds.StatusIsOk_2013(status, PUDS_STATUS_OK, False):
        result = display_uds_msg_validate(confirmation, response, False)
    else:
        result = display_uds_msg_validate(request, None, False)

    print("UDS Service: ECUReset: " + test_result_to_string(result))

    # Free messages
    status = objPCANUds.MsgFree_2013(request)
    status = objPCANUds.MsgFree_2013(response)
    status = objPCANUds.MsgFree_2013(confirmation)
    time.sleep(5)
    return result

########################################### (UDS over CAN: helpers for service tests) ####################################################

def getCChar(c):
    return c if sys.version_info.major >= 3 else chr(c)

def Reverse32(v):
    res = create_string_buffer(4)
    res[3] = getCChar(v & 0x000000FF)
    res[2] = getCChar((v >> 8) & 0x000000FF)
    res[1] = getCChar((v >> 16) & 0x000000FF)
    res[0] = getCChar((v >> 24) & 0x000000FF)
    return res

def display_uds_msg_validate(request, response, no_response_expected):
    """
    A function that displays UDS Request and Response messages (and count error if no response)

    parameters:
     request: Request message
     response: Received response message
     no_response_expected: if no response is expected, do not increment error counter
    """
    global g_nbErr
    if request != None and request.msg.msgdata.isotp:
        print("\nUDS request from 0x%04X (to 0x%04X, with extension address 0x%02X) - result: %i - %s" % (
            request.msg.msgdata.isotp.contents.netaddrinfo.source_addr,
            request.msg.msgdata.isotp.contents.netaddrinfo.target_addr,
            request.msg.msgdata.isotp.contents.netaddrinfo.extension_addr,
            request.msg.msgdata.any.contents.netstatus,
            "ERROR !!!" if request.msg.msgdata.any.contents.netstatus != PCANTP_NETSTATUS_OK.value else "OK !"))
        # display data
        s = "\t-> Length: {x1}, Data= ".format(x1=format(request.msg.msgdata.any.contents.length, "d"))
        for i in range(request.msg.msgdata.any.contents.length):
            s += "{x1} ".format(x1=format(request.msg.msgdata.any.contents.data[i], "02X"))

        print(s)

    if response != None and response.msg.msgdata.isotp:
        print("\nUDS RESPONSE from 0x%04X (to 0x%04X, with extension address 0x%02X) - result: %i - %s" % (
            response.msg.msgdata.isotp.contents.netaddrinfo.source_addr,
            response.msg.msgdata.isotp.contents.netaddrinfo.target_addr,
            response.msg.msgdata.isotp.contents.netaddrinfo.extension_addr, response.msg.msgdata.any.contents.netstatus,
            ("ERROR !!!" if response.msg.msgdata.any.contents.netstatus != PCANTP_NETSTATUS_OK.value else "OK !")))
        # display data
        s = "\t-> Length: {x1}, Data= ".format(x1=format(response.msg.msgdata.any.contents.length, "d"))
        for i in range(response.msg.msgdata.any.contents.length):
            s += "{x1} ".format(x1=format(response.msg.msgdata.any.contents.data[i], "02X"))

        print(s)

        respSID = request.msg.msgdata.any.contents.data[0] + 0x40
        if respSID == response.msg.msgdata.any.contents.data[0]:
            print("Positive Response")
            return True

        elif 0x7F == response.msg.msgdata.any.contents.data[0]:
            print("Negative Response")
            g_nbErr += 1
            return False
        else:
            print("Invalid Response")
            g_nbErr += 1
            return False

    elif not no_response_expected:
        print("\n      ERROR: NO UDS RESPONSE !!\n")
        g_nbErr += 1
        return False
