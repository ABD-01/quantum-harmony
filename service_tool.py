#!/usr/bin/env python
'''\

Copyright 2024 Accolade Electronics Pvt. Ltd

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

file        service_tool.py
brief       This is the main script file for launching the service tool UI

date        22 June 2024
author      Accolade Electronics <www.accoladeelectronics.com>

tested on python 3.8.0 on windows 10 x64

python --version
pip install --user --upgrade pip

pip3 install virtualenv
virtualenv <name>

source <name>/Scripts/activate

# dependencies
pip3 install serial
pip3 install PyQt5
pip3 install numpy
pip3 install serial-tool
pip install pyqt5-tools
pip install packaging
'''

# ### Developer Note: Delays and Timeouts in the Software
#
# #### Implicit Delays (from DLL)
# - These delays are inherent and cannot be altered:
#   a) 3-second delay after sending "StartP"
#   b) 1-second delay after sending each file chunk
#   c) 2-second delay before sending the "RBT" command for reboot
#
# #### Explicit Delays
# - These delays are currently set based on current operational requirements and can be adjusted by the developer:
#   a) 0.1-second delay in while loops as a standard practice
#   b) 1-second timeout on all threads' join methods
#   c) 20-second wait timeout for device reboot during certificate flashing
#   d) 5-second delay to clear the serial input buffer before using the "GET" command for UIN [Important! Do not change this delay]
#   e) 1-second delay before using the "GET" command for certificates
#   f) 2-second delay after the "END SESSION" event occurs and before using "RBT"


import ctypes
import os, re, resources, shutil, sys, tarfile, time, zipfile
import os.path as osp
import threading
from datetime import datetime
from enum import IntEnum

import serial.tools.list_ports as ports
from PyQt5 import QtWidgets, uic
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *

import bridge


CERT_TYPE_CA_CRT = 0        # file name must be "ca-crt.pem"
CERT_TYPE_DEVICE_CRT = 1    # file name must be "device-crt.pem"
CERT_TYPE_DEVICE_KEY = 2    # file name must be "device-key.pem"

VERSION='1.4.0'

# THIS VARIABLE WILL SERVE AS A SWITCH TO ENABLE OR DISABLE CONFIG RECOVERY
CONFIG_RECOVERY_SUPPORT = True

if CONFIG_RECOVERY_SUPPORT:
    from config_recovery import UIConfigRecovery

try:
    from ctypes import windll  # Only exists on Windows.
    myappid = f'AEPL.ATCU.ServiceTool.{VERSION}'
    windll.shell32.SetCurrentProcessExplicitAppUserModelID(myappid)
except ImportError:
    pass

# MULTI-THREADING
class Worker(QObject):
    finished = pyqtSignal()
    intReady = pyqtSignal(str)
    handleUi = pyqtSignal(list)             # for modifying UI elements from thread
    fsmState = 'IDLE'
    startTime = 0
    endTime = 0
    ser_dll = None
    serIsOpen = False
    csvHeader = ['time', 'UIN', 'CA', 'CC', 'CK']
        
    def __init__(self, ser_handle=None):
        super(Worker, self).__init__()
        self.ser_handle = ser_handle
        self.working = True
        self.certCsvFile = "2G_certificates_flashed_%s.csv" % datetime.now().strftime("%d-%b-%Y")

    def initCsvFile(self):
        if not osp.exists(self.certCsvFile):
            with open(self.certCsvFile, 'w') as f:
                f.write(','.join(self.csvHeader) + '\n')
                

    def work(self):
        global round_up
        round_up = lambda x: int(x) if int(x) == x else int(x) + 1
        while self.working:
            if self.fsmState == 'IDLE':
                pass

            if self.fsmState == 'UART_TRANSPARENT_MODE':
                try:
                    if(self.serIsOpen):
                        while self.fsmState == 'UART_TRANSPARENT_MODE':
                            rxData = ctypes.create_string_buffer(1024)
                            ret = self.ser_dll.readline(self.ser_handle, rxData, 1024)
                            if ret == -1:
                                self.handleUi.emit(['SHOW_POPUP', 'COM PORT ERROR', 'Failed to read from serial port'])
                                raise Exception("Failed to read from serial port")
                            rxData = rxData.value.decode('utf-8', 'ignore')
                            print("rx <- " + rxData)
                            
                            # in case of device reboot, search this boot print and re-enable buttons by emiting 0
                            if re.search('BOOT SIGNATURE', rxData):
                                self.handleUi.emit(['RE-ENABLE_BUTTONS', '', ''])
                                self.handleUi.emit(['SET_PROGRESS', 0, ''])
                                
                            self.handleUi.emit(['LOG_APPEND', rxData, ''])
                except Exception as e:
                    print(e)
                    return
                
            # Skipping PREVALIDATE state in case of 2G devices
            if self.fsmState == 'PREVALIDATE':
                self.fsmState = 'WRITE_FILES'
                continue
                
            # Uses the DLL functin serial_upload_firmware and serial_upload_certificate
            # The underlying protocol is neither documented nor understood to it's full extent.
            if self.fsmState == 'WRITE_FILES':
                global conn
                try:
                    if commInterface == 'SERIAL':
                        time.sleep(0.05)
                        
                    self.handleUi.emit(['DISABLE_BUTTONS', '', ''])
                    
                    ''' in case of firmware, file count will be 1 and in case of certificate, it will be 3, order of certificates is fixed at CA CC CK '''
                    fileCount = len(fileList)
                    self.handleUi.emit(['LOG_APPEND', f'Starting upload of {fileCount} file(s)...', ''])
                    
                    self.startTime = time.time()
                    
                    for idx in range(fileCount):
                        time.sleep(0.1)
                        currentFilePath = fileList[idx]
                        
                        # print file name to the log window
                        parts = currentFilePath.split('/')
                        fileName = parts[-1]
                        self.handleUi.emit(['LOG_APPEND', f'Uploading {fileName}...', ''])
                        
                        certType = idx   # in case of FIRMWARE, it will be don't care

                        if upgradeType == 'FIRMWARE':
                            upload_file = self.ser_dll.serial_upload_firmware
                            upload_file.argtypes = [ctypes.wintypes.HANDLE, ctypes.c_char_p]
                            upload_file.restype = ctypes.wintypes.INT

                            filePath = ctypes.create_string_buffer(currentFilePath.encode('utf-8'))
                            
                            args = (self.ser_handle, filePath)
                        elif upgradeType == 'CERTIFICATE':    # packet types 3 and 4 for init and data respectively
                            upload_file = self.ser_dll.serial_upload_certificate
                            upload_file.argtypes = [ctypes.wintypes.HANDLE, ctypes.wintypes.INT, ctypes.c_char_p]
                            upload_file.restype = ctypes.wintypes.INT
                            filePath = ctypes.create_string_buffer(currentFilePath.encode('utf-8'))

                            args = (self.ser_handle, certType, filePath)
                            
                        # Progress Thread
                        t = threading.Thread(target=self.updateProgress, args=(idx,), daemon=True)
                        t.start()
                        
                        status = upload_file(*args)
                        if status != 0:
                            raise Exception('Upload failed')
                        
                        # Joining Progress thread
                        try:
                            t.join(timeout=1)
                        except:
                            pass

                        # Wait for reboot after each certificate
                        if upgradeType == 'CERTIFICATE':
                            self.handleUi.emit(['SET_PROGRESS', round_up((idx + 1) * 100 / fileCount), ''])
                            t = threading.Thread(target=self.timedTransparentMode)
                            t.start()
                            t.join()
                            
                    # Get UIN and Certificate Status 
                    if upgradeType == 'CERTIFICATE' and self.statCerts():
                        self.handleUi.emit(['SHOW_POPUP', 'Certificate Status Validation failed', 'Timed out during detecting certificate status from device.'])
                        self.handleUi.emit(['RE-ENABLE_BUTTONS', '', ''])
                        self.fsmState = 'UART_TRANSPARENT_MODE'
                        continue

                        
                    self.handleUi.emit(['SET_PROGRESS', 100, ''])
                    self.handleUi.emit(['END_SESSION', '', ''])
                    self.fsmState = 'UART_TRANSPARENT_MODE'
                    
                    # Calculate the execution time in seconds
                    self.endTime = time.time()
                    executionTime = self.endTime - self.startTime
                    
                    self.handleUi.emit(['SHOW_POPUP', 'Update completed', f'{"Certificate" if upgradeType == "CERTIFICATE" else "Firmware"} update completed successfully in {executionTime:.1f} sec'])
                                        
                except Exception as e:
                    self.handleUi.emit(['SHOW_POPUP', 'Failed:', f'WRITE_FILES:\n{e}'])
                    return
              
            time.sleep(0.1)  
        self.finished.emit()
    
    def updateProgress(self, fileIdx=0):
        """
        This function utilizes IPC (Inter-Process Communication) over sockets to 
        receive an integer progress value over UDP.
        """
        p = bridge.Progess()
        p.start()
        progress = 0
        _prev = progress
        total_progress = 0

        while progress < 100:
            progress = p.get_progress()
            if _prev == progress:
                continue
            if upgradeType == 'FIRMWARE':
                total_progress = progress
            elif upgradeType == 'CERTIFICATE':
                total_progress = round_up((fileIdx*33.33) + (progress/3.0))
            self.handleUi.emit(['SET_PROGRESS', total_progress, ''])
            time.sleep(0.1)
            _prev = progress
        p.stop()
        
    def timedTransparentMode(self, timeout=20):
        """
        The name explains the functioning.
        To perform timed transparent mode operation.
        Because cannot change states mid file transfer loop.
        """
        rxBuffer = ctypes.create_string_buffer(1024)
        start_time = time.time()
        while time.time() - start_time < timeout:
            self.ser_dll.readline(self.ser_handle, rxBuffer, 1024)
            rxData = rxBuffer.value.decode('utf-8', 'ignore')
            print("rx <- \033[0;36m%s\033[0m" % rxData.strip())
        
            if re.search('BOOT SIGNATURE', rxData):
                break

            # TODO: next line is for debugging purpose. Remove in final release
            # self.handleUi.emit(['LOG_APPEND', f"<span style='color:cyan'>{rxData}</span>", ''])
        time.sleep(0.1)
    
    def statCerts(self) -> int:
        """
        Retrieves the UIN and certificate status using 'GET' commands.
        
        Returns:
            int: 0 on successful retrieval and saving, 1 on failure.
        """
        uin_pattern = re.compile(r"STATUS#UIN#(\w+)#")
        cert_pattern = re.compile(r"STATUS#CERT#(\d+),(\d+),(\d+)#")
        self.handleUi.emit(['LOG_APPEND', "\nGetting UIN and Certificate status...", ''])
        rxBuffer = ctypes.create_string_buffer(1024)
        # time.sleep(5) # wait for proper bootup
        start_time = time.time()
        while time.time() - start_time < 5:
            self.ser_dll.serial_read(self.ser_handle, rxBuffer, 1024)
        rxBuffer.value = b''

        looprun = True
        attempt = 0
        self.ser_dll.serial_write(self.ser_handle, b'CMN *GET#UIN#\r\n', 16)
        while looprun:
            self.ser_dll.readline(self.ser_handle, rxBuffer, 1024)
            rxData = rxBuffer.value.decode('utf-8', 'ignore')
            print("rx <- \033[0;36m%s\033[0m" % rxData.strip())
            
            match = re.findall(uin_pattern, rxData)
            if not match:
                attempt += 1
            else:
                uin = match[0]
                self.handleUi.emit(['LOG_APPEND', f"UIN: <h2><span style='color:green'>{uin}</span></h2>", ''])
                looprun = False
            if attempt == 50:
                self.handleUi.emit(['LOG_APPEND', "Failed to get UIN...", ''])
                looprun = False
                return 1
        time.sleep(1) # is this even needed??
        looprun = True
        attempt = 0
        self.ser_dll.serial_write(self.ser_handle, b'CMN *GET#CERT#\r\n', 16)
        while looprun:
            self.ser_dll.readline(self.ser_handle, rxBuffer, 1024)
            rxData = rxBuffer.value.decode('utf-8', 'ignore')
            print("rx <- \033[0;36m%s\033[0m" % rxData.strip())
            
            match = re.search(cert_pattern, rxData)
            if not match:
                attempt += 1
            else:
                ca, cc, ck = match.groups()
                self.handleUi.emit(['LOG_APPEND', f"Certificate: <h2><span style='color:green'>CA: {ca}, CC: {cc}, CK: {ck}</span></h2>", ''])
                looprun = False
            if attempt == 50:
                self.handleUi.emit(['LOG_APPEND', "Failed to get Certificates Status...", ''])
                looprun = False
                return 1

        row = [datetime.now().strftime('%d-%m-%Y %H:%M:%S'), uin, ca, cc, ck]
        with open(self.certCsvFile, 'a') as f:
            f.write(f'{",".join(map(str, row))}\n')

        return 0
        
def is_config_recvery(cls):
    if CONFIG_RECOVERY_SUPPORT:
        class ServiceToolUI(cls, UIConfigRecovery):
            pass
        return ServiceToolUI
    return cls

@is_config_recvery
class ServiceToolUi(QtWidgets.QMainWindow):
    ser_handle = None
    def __init__(self):
        super(QtWidgets.QMainWindow, self).__init__()
        uic.loadUi(self.getResourcePath('service_tool.ui'), self)
        self.setWindowIcon(QIcon(self.getResourcePath('resources/thumbnail.ico')))
        
        self.setStyleSheet('QMainWindow#MainWindow { background-image:url(:/resources/background.png) }')
    
        self.labelVersion = self.findChild(QtWidgets.QLabel,'labelVersion')
        self.labelBuild = self.findChild(QtWidgets.QLabel,'labelBuild')
        self.comboBoxComPort = self.findChild(QtWidgets.QComboBox,'comboBoxComPort')
        self.btnConnect = self.findChild(QtWidgets.QPushButton,'btnConnect')
        self.btnWrite = self.findChild(QtWidgets.QPushButton,'btnWrite')
        self.btnCfgRecover = self.findChild(QtWidgets.QPushButton,'btnCfgRecover')
        self.log = self.findChild(QtWidgets.QTextBrowser,'textBrowserLog')
        self.btnFwMode = self.findChild(QtWidgets.QRadioButton,'btnFwDownload')
        self.btnCertMode = self.findChild(QtWidgets.QRadioButton,'btnCertDownload')
        self.progressBar = self.findChild(QtWidgets.QProgressBar,'progressBar')
    
        self.btnConnect.clicked.connect(self.connectInterface)
        self.btnFwMode.clicked.connect(self.browseFiles)
        self.btnCertMode.clicked.connect(self.browseFiles)
        self.btnWrite.clicked.connect(self.writeFiles)

        # Setting Radio Buttons to be independent
        self.btnFwMode.setAutoExclusive(False)
        self.btnCertMode.setAutoExclusive(False)


        if CONFIG_RECOVERY_SUPPORT:
            self.getPage2Children()
            self.getPage3Children()

            self.stackedWidget = self.findChild(QtWidgets.QStackedWidget,'stackedWidget')
            self.btnCfgRecover.clicked.connect(self.switchProfile)

            self.switchProfileAttempt = 0
        else:
            self.btnCfgRecover.hide()
            self.btnCfgRecover.deleteLater()
    
        # loading the DLL
        self.ser_dll = ctypes.CDLL(self.getResourcePath('resources/serial_upload'))
    
        # populate the comboboxes
        self.btnConnect.setIcon(QIcon(':/resources/disconnected.png'))
        comPortList = ["COM23"]   # TODO eshwar enable TCP support after verified working. reserve TCP port string here, it will be followed by available serial ports
        # comPortList = ['TCP Port']   # reserve TCP port string here, it will be followed by available serial ports
        availablePorts = list(ports.comports())
        for i in availablePorts:
            comPortList.append(i.device + ': ' + i.description) # COM21: Prolific USB-to-Serial Comm Port (COM21)
        self.comboBoxComPort.addItems(comPortList)
        
        # Profile Switching setup
        self.profiles = IntEnum('Profiles', ['FLASH', 'CFG_RECOVERY'])
        self.currentProfile = self.profiles.FLASH
        
        # MUTLI-THREADING
        self.worker = Worker()                                  # a new worker to perform those tasks
        self.thread = QThread()                                 # a new thread to run our background tasks in
        self.worker.moveToThread(self.thread)                   # move the worker into the thread, do this first before connecting the signals
        self.worker.finished.connect(self.worker.deleteLater)   # have worker mark itself for deletion
        self.worker.handleUi.connect(self.handleUiFromThread)   # for modifying UI class elements after a resultant operation from thread
        self.worker.ser_dll = self.ser_dll                      # pass the DLL to worker
    
        self.thread.started.connect(self.worker.work)           # begin our worker object's loop when the thread starts running
        # self.thread.finished.connect(self.thread._deleteLater)   # have thread mark itself for deletion
        self.thread.finished.connect(lambda: print("Thread Finished"))
        self.worker.destroyed.connect(lambda: print("Worker Destroyed"))
        self.thread.destroyed.connect(lambda: print("Thread Destroyed"))
        self.thread.start()
        
        self.handleUiFromThread('SHOW_POPUP', 'Disclaimer', 'Accolade Service tool usage is restricted to Accolade service engineers and representatives only\n')
        self.labelVersion.setText('SERVICE TOOL v{}'.format(VERSION))
        self.labelBuild.setText('<strong>Phase I</strong> Stable build 19 Aug 2024')
        
        self.setFixedSize(680,460)
        self.show() # show the gui
    
    def writeFiles(self):
        self.btnWrite.setEnabled(False)
        self.btnCfgRecover.setEnabled(False)
        self.worker.fsmState = 'IDLE'
        # for serial port prevalidation must be done over SERIAL commands for battery and other checks
        if commInterface == 'SERIAL':
            self.worker.fsmState = 'PREVALIDATE'
    
    def browseFiles(self):
        global fileList, upgradeType
        fileList = ['', '', '']     # cert1/bin, cert2, cert3 are reserved respectively
        options = QFileDialog.Options()
        if self.btnFwMode.isChecked():
            upgradeType = 'FIRMWARE'
            self.btnCertMode.setChecked(False)
            fileList, _ = QFileDialog.getOpenFileNames(self,'Select Firmware file', '','(*.bin *.tar.gz *.zip)', options=options)
        elif self.btnCertMode.isChecked():
            upgradeType = 'CERTIFICATE'
            self.btnFwMode.setChecked(False)
            fileList, _ = QFileDialog.getOpenFileNames(self,'Select Certificates', '','Certificate Files (*.pem)', options=options)
        
        if fileList.__len__() == 0:
            self.btnFwMode.setChecked(False)
            self.btnCertMode.setChecked(False)
            return
        try:
            if fileList:
                if upgradeType == 'FIRMWARE' :
                    if fileList[0].endswith('.bin'):
                        pass
                    else:
                        # Create the '__temp__' folder if it doesn't exist and delete it later
                        temp_folder = '__temp__'
                        if os.path.exists(temp_folder):
                            shutil.rmtree(temp_folder, ignore_errors=True)
                            os.makedirs(temp_folder)
                        else:
                            os.makedirs(temp_folder)
                        
                        # if tar selected, extract bin from it in temp location and proceed further
                        if fileList[0].endswith('.tar.gz'):
                            # Extract the tar.gz file into the '__temp__' folder
                            with tarfile.open(fileList[0], 'r:gz') as tar:
                                tar.extractall(temp_folder)
                        
                        # if tar selected, extract bin from it in temp location and proceed further
                        if fileList[0].endswith('.zip'):
                            # Open the zip file for reading
                            with zipfile.ZipFile(fileList[0], 'r') as zip_ref:
                                # Extract file into the '__temp__' folder
                                zip_ref.extractall(temp_folder)
                                
                        # Get a list and find the bin file
                        extractedFileList = os.listdir(temp_folder)
                        index = 0
                        for file_name in extractedFileList:
                            index += 1
                            if file_name.endswith('.bin'):
                                fileList[0] = './__temp__/' + file_name
                                break
                            elif extractedFileList.__len__() == index:
                                self.handleUiFromThread('SHOW_POPUP', 'No suitable file found', 'The selected file does not contain a .bin file')
                                self.btnFwMode.setChecked(False)
                                return
                    
                    self.btnWrite.setEnabled(True)
        
                if upgradeType == 'CERTIFICATE': # must select 3 cert files
                    if fileList.__len__() != 3:
                        self.handleUiFromThread('SHOW_POPUP', 'Count unsatisfied', 'Please select 3 certificates')
                        return
                    
                    # must have exact names for recognition
                    cert_names =  ['CA_cert.pem', 'client-crt.pem', 'client.pem']
                    fileList.sort()
                    if (fileList[0].find(cert_names[0]) != -1) and (fileList[1].find(cert_names[1]) != -1) and (fileList[2].find(cert_names[2]) != -1):
                        self.worker.initCsvFile()
                    else:
                        self.handleUiFromThread('SHOW_POPUP', 'File', 'Please rename files as per certificate types \n%s' % "\n".join(cert_names))
                        return
                    self.btnWrite.setEnabled(True)
        except:
            pass

    def connectInterface(self):
        global commInterface
        selectedPort = self.comboBoxComPort.currentText()    # full port name COM21: Prolific USB-to-Serial Comm Port (COM21)
        
        portName = selectedPort.split(':')      # splits the 'COM21', 'Prolific ...'
        try:
            # ser = serial.Serial(portName[0], 115200, timeout = 0.1)
            self.ser_handle = bridge.CreateFileSerialPort(portName[0])
            if self.ser_handle == bridge.INVALID_HANDLE_VALUE:
                raise Exception
            self.serIsOpen = True
            self.worker.ser_handle = self.ser_handle
            self.worker.serIsOpen = self.serIsOpen
            commInterface = 'SERIAL'
            self.comboBoxComPort.setEnabled(False)
            self.btnConnect.setIcon(QIcon(':/resources/connected.png'))
            self.worker.fsmState = 'UART_TRANSPARENT_MODE'
            self.btnCertMode.setEnabled(True)
            self.btnFwMode.setEnabled(True)
            if CONFIG_RECOVERY_SUPPORT:
                self.btnCfgRecover.setEnabled(True)
            
        except:
            self.handleUiFromThread('SHOW_POPUP', 'COM Port Error', 'Access denied')

    def getResourcePath(self, relative_path):
        '''
        Returns the absolute path of a resource file
        that has been packed within the exe.
        It removes the necessity of the .ui file at the exe path

        The file is stored inside %%TEMP%% directory, 
        in a folder named _MEIxxxxxx, where xxxxxx is a 
        random number.
        eq: C:/Users/<USERNAME>/AppData/Local/Temp/_MEI187482/resources/ATCU_5.2.6_REL07.bin

        Ref: https://stackoverflow.com/a/55085519
        '''
        if getattr(sys, 'frozen', False):
            return os.path.join(sys._MEIPASS, relative_path)

        return os.path.join(os.path.abspath('.'), relative_path)
         
    def handleUiFromThread(self, *args):
        arg = args[0] if isinstance(args[0], list) else args
        if arg[0] == 'SHOW_POPUP':
            msgBox = QMessageBox()
            msgBox.setWindowTitle(arg[1])
            msgBox.setIcon(QMessageBox.Critical if 'ERROR' in arg[1].upper() else QMessageBox.Warning if 'WARN' in arg[1].upper() else QMessageBox.Information)
            msgBox.setText(arg[2])
            msgBox.exec()
        if arg[0] == 'LOG_APPEND':
            self.log.append(arg[1])
        if arg[0] == 'SET_PROGRESS':
            self.progressBar.setValue(int(arg[1]))
        if arg[0] == 'DISABLE_BUTTONS':
            self.log.setText('')
            self.btnWrite.setEnabled(False)
            self.comboBoxComPort.setEnabled(False)
            self.btnFwMode.setEnabled(False)
            self.btnCertMode.setEnabled(False)
            if CONFIG_RECOVERY_SUPPORT:
                self.btnCfgRecover.setEnabled(False)
        if arg[0] == 'RE-ENABLE_BUTTONS':
            self.btnFwMode.setEnabled(True)
            self.btnCertMode.setEnabled(True)
            if commInterface == 'SERIAL':   # allow certificate flashing only on serial interface
                 self.btnConnect.setEnabled(True)
        if arg[0] == 'END_SESSION':
            time.sleep(2) # is this even needed??
            buff = ctypes.create_string_buffer(b'RBT')
            self.ser_dll.serial_write(self.ser_handle, buff, len(buff))
            print("END SESSION")
            self.btnFwMode.setChecked(False)
            self.btnCertMode.setChecked(False)
            fileList.clear()
            self.btnWrite.setEnabled(False)
            if CONFIG_RECOVERY_SUPPORT:
                self.btnCfgRecover.setEnabled(True)
        if arg[0] == 'PREVALIDATION_ERR':
            self.btnFwMode.setChecked(False)
            self.btnCertMode.setChecked(False)

app = QtWidgets.QApplication(sys.argv)
window = ServiceToolUi()

app.exec_()
