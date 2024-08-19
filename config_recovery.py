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

date        17 July 2024
author      Muhammed Abdullah <muhammed.shaikh@accoladeelectronics.com>

tested on python 3.10.11 on windows 10 x64

# dependencies
pip3 install PyQt5
pip3 install serial-tool
pip install pyqt5-tools
'''
import re
import sys
import time
from ctypes import create_string_buffer
from enum import IntEnum
from itertools import count
from os import execl
from queue import Queue
from threading import Thread

import requests

from PyQt5.QtCore import pyqtSignal, pyqtSlot, QByteArray, QObject, QTimer
from PyQt5.QtGui import QMovie, QPixmap
from PyQt5.QtWidgets import (
    QComboBox,
    QGridLayout,
    QLabel,
    QLineEdit,
    QMessageBox,
    QPushButton,
    QVBoxLayout,
    QWidget,
)

class Command:
    def __init__(self, cmd, rsp_pattern=None, callback=None, timeout=15, **kwargs):
        self.cmd = cmd
        self.rsp_pattern = rsp_pattern
        self.callback = callback
        self.timeout = timeout # in seconds
        self.willRbt = kwargs.get('willRbt', False)

class WorkerConfig(QObject):
    finished = pyqtSignal()
    handleUi = pyqtSignal(list)
    getVariantSignal = pyqtSignal(object)
    rebootingSignal = pyqtSignal(bool)
    checkCfgSignal = pyqtSignal()
    ser_dll = None
    state = IntEnum('State', ['IDLE', 'TRANSPARENT', 'SENDING', 'WAITING_RESPONSE', 'GET_VARIANT'], start=0)
    rbtWaitTimer = 20
    def __init__(self, ser_handle=None):
        super(WorkerConfig, self).__init__()
        self.working = True
        self.fsmState = self.state.IDLE
        self.ser_handle = ser_handle
        self.rxBuffer = create_string_buffer(1024)
        
        self.commands = []
        self.currentCmd = None
        self.commandQueue = Queue(maxsize=10)
        
    def work(self):

        prev_state = self.fsmState
        print("Starting worker...", prev_state)
        while self.working:

            if prev_state != self.fsmState:
                print("State Changed to : ", self.fsmState)
                prev_state = self.fsmState
                        
            if self.fsmState == self.state.IDLE:
                # if self.ser_dll is None or self.ser_handle is None:
                    continue

                # self.fsmState = self.state.TRANSPARENT
            
            if self.fsmState == self.state.TRANSPARENT:
                try:
                    if self.currentCmd and self.currentCmd.willRbt:
                        self.logAppend("Waiting for Reboot...")
                        print("Waiting for Reboot...")
                        self.rebootingSignal.emit(True)
                        start_time = time.time()
                        while (time.time() - start_time) < self.rbtWaitTimer:
                            ret = self.ser_dll.readline(self.ser_handle, self.rxBuffer, 1024)
                            if ret == -1:
                                self.handleUi.emit(['SHOW_POPUP', 'COM PORT ERROR', 'Failed to read from serial port'])
                                raise Exception("Failed to read from serial port")
                            rxData = self.rxBuffer.value.decode('utf-8', 'ignore')
                            print("rx <- \033[0;36m%s\033[0m" % rxData.strip())
                            if re.search('BOOT SIGNATURE', rxData):
                                self.logAppend("Reboot Successful...")
                                print("Reboot Successful...")
                                break
                        # Wait 5 sec after reboot
                        start_time = time.time()
                        while time.time() - start_time < 5:
                            self.ser_dll.serial_read(self.ser_handle, self.rxBuffer, 1024)
                        self.rxBuffer.value = b''
                        
                        self.rebootingSignal.emit(False)
                        self.currentCmd = None
                            
                    if self.commandQueue.empty():
                        ret = self.ser_dll.readline(self.ser_handle, self.rxBuffer, 1024)
                        if ret == -1:
                            self.handleUi.emit(['SHOW_POPUP', 'COM PORT ERROR', 'Failed to read from serial port'])
                            raise Exception("Failed to read from serial port")
                        rxData = self.rxBuffer.value.decode('utf-8', 'ignore')
                        self.handleUi.emit(['LOG_APPEND', rxData, ''])
                        # print("rx <- " + rxData)
                    else:
                        self.fsmState = self.state.SENDING
                except Exception as e:
                    print(e)
                    pass
                
            if self.fsmState == self.state.SENDING:
                self.currentCmd: Command = self.commandQueue.get()
                self.ser_dll.serial_write(self.ser_handle, self.currentCmd.cmd.encode(), len(self.currentCmd.cmd.encode()))        
                self.logAppend("Sending command: %s" % self.currentCmd.cmd)
                print("Sending command: %s" % self.currentCmd.cmd)
                
                if self.currentCmd.rsp_pattern:
                    self.fsmState = self.state.WAITING_RESPONSE
                else:
                    if self.currentCmd.callback:
                        self.currentCmd.callback()
                    self.fsmState = self.state.TRANSPARENT
                
            if self.fsmState == self.state.WAITING_RESPONSE:
                self.handleUi.emit(['LOG_APPEND', "Waiting for response ...", ''])
                print("Waiting for response ...")
                start_time = time.time()
                attempt = 0
                found = False
                while time.time() - start_time < self.currentCmd.timeout:
                    self.ser_dll.readline(self.ser_handle, self.rxBuffer, 1024)
                    rxData = self.rxBuffer.value.decode('utf-8', 'ignore')
                    print("rx <- " + rxData)
                    
                    match = re.search(self.currentCmd.rsp_pattern, rxData)
                    if match:
                        found = True
                        if self.currentCmd.callback:
                            self.currentCmd.callback(match.groups())
                        self.logAppend("Response received: {}".format(str(match[0])))
                        print("Response received: {}".format(str(match[0])))
                        break
                    attempt += 1
                if not found:
                    self.logAppend("Failed to get response for command: %s" % self.currentCmd.cmd)
                    self.handleUi.emit(['SHOW_POPUP', 'Warning',"Failed to get response for command: %s" % self.currentCmd.cmd])

                
                self.fsmState = self.state.TRANSPARENT

            if self.fsmState == self.state.GET_VARIANT:
                self.getDeviceVariant()
                self.fsmState = self.state.TRANSPARENT
            
        self.finished.emit()
    
    def registerCommand(self, cmd, rsp=None, cb=None, timeout=15, willRbt=False) -> int:
        self.commands.append(
            Command(cmd, re.compile(rsp) if rsp else None, cb, timeout, willRbt=willRbt)
        )
        return len(self.commands) - 1

    def sendCommand(self, cmdId, command=None):
        if cmdId >= len(self.commands):
            self.logAppend("Invalid command")
            return False
        if self.commandQueue.full():
            self.logAppend("Command queue is full. Try Again")
            return False
        
        self.logAppend("Command Queued")
        if cmdId == -1:
            # Incase command is not registered
            if command is None:
                return False
            self.commandQueue.put(command)
            return True 

        self.commandQueue.put(self.commands[cmdId])
        return True
        
    def logAppend(self, text):
        escape = lambda t : t.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;").replace("'", "&#39;").replace('"', "&quot;")
        self.handleUi.emit(['LOG_APPEND', f"<h3><span style='color:cyan'>{escape(text)}</span></h3>", ''])

    
    def getDeviceVariant(self):
        print("Get Device Variant")
        if self.ser_dll is None or self.ser_handle is None:
            self.handleUi.emit(['LOG_APPEND', "Serial port not initialized", ""])
            return None
        
        uin_pattern1 = re.compile(r"<STATUS#UIN#(\w+)#>")
        uin_pattern2 = re.compile(r"<UIN:(\w+)>")
        self.handleUi.emit(['LOG_APPEND', "\nGetting UIN ...", ''])
        
        res = None
        
        # start_time = time.time()
        # while time.time() - start_time < 2:
        #     self.ser_dll.serial_read(self.ser_handle, self.rxBuffer, 1024)
        # self.rxBuffer.value = b''

        looprun = True
        attempt = 0
        variantFound = None
        self.ser_dll.serial_write(self.ser_handle, b'CMN *GET#UIN#\r\n', 16)
        while looprun:
            self.ser_dll.readline(self.ser_handle, self.rxBuffer, 1024)
            rxData = self.rxBuffer.value.decode('utf-8', 'ignore')
            print("rx <- \033[0;36m%s\033[0m" % rxData.strip())
            
            match = re.search(uin_pattern1, rxData)
            if not match:
                attempt += 1
            else:
                uin = match.groups()[0]
                self.handleUi.emit(['LOG_APPEND', f"UIN: <h2><span style='color:green'>{uin}</span></h2>", ''])
                looprun = False
                variantFound = "Normal"
                res =  variantFound, uin
            if attempt == 50:
                self.handleUi.emit(['LOG_APPEND', "Failed to get UIN...", ''])
                looprun = False

        if res is None:
            looprun = True
            attempt = 0
            self.ser_dll.serial_write(self.ser_handle, b'CMN GET UIN\r\n', 16)
            while looprun:
                self.ser_dll.readline(self.ser_handle, self.rxBuffer, 1024)
                rxData = self.rxBuffer.value.decode('utf-8', 'ignore')
                print("rx <- \033[0;36m%s\033[0m" % rxData.strip())
                
                match = re.search(uin_pattern2, rxData)
                if not match:
                    attempt += 1
                else:
                    uin = match.groups()[0]
                    self.handleUi.emit(['LOG_APPEND', f"UIN: <h2><span style='color:green'>{uin}</span></h2>", ''])
                    looprun = False
                    variantFound = "CDAC"
                    res =  variantFound, uin
                if attempt == 50:
                    self.handleUi.emit(['LOG_APPEND', "Failed to get UIN...", ''])
                    looprun = False
    
        self.getVariantSignal.emit(res)         
        return res
    

class UIConfigRecovery:

    def switchProfile(self):
        """
        A function that handles switching profiles. 
        
        Developer Note:
            To make sure that the function only switches from `FLASH` to `CFG_RECOVERY`,
            a guard is implemented and will raise an error.
            It is not possible to call this function via UI once the profile is switched.
            It is important to maintain this function in future changes.
        
        The function sets the state of the worker to IDLE, and quits the thread.
        
        `QThread.quit()` or `QThread.exit()` does not immedially stop the thread. 
        `QThread.wait()` acts like thread.join but is blocking call.
        
        Instead of waiting, the `finished` signal of the thread is polled for 5 seconds.               
        The function schedules a single shot timer to call itself (`switchProfile`) again after 1 second.
        In case the thread is not finished after 5 seconds, it terminates the thread using
        `self.thread.terminate()` about which the documentation says: 
            "This function is dangerous and its use is discouraged."

        This function sets the current index of the `stackedWidget` to 1 until the thread is finished.
        
        If the `thread` is finished, it creates a new `WorkerConfig` object, moves it to the thread,
        and starts the thread. Also, sets the current index of the `stackedWidget` to 2. 

        """
        
        if self.currentProfile == self.profiles.CFG_RECOVERY:
            self.handleUiFromThread(None, "SHOW_POPUP", 
                "Error Occurred", "This is not supposed to happen\n"
                "Inform the developer\n"
                "\"ERROR: Trying to switching from CFG_RECOVERY to FLASH\"")
            raise Exception("Cannot switch from CFG_RECOVERY to FLASH")
        
        self.stackedWidget.setCurrentIndex(1)
        if self.worker.working:
            self.worker.fsmState = 'IDLE'
            self.worker.working = False
            self.thread.quit()
            print("Quitting thread")
            QTimer.singleShot(1000, self.switchProfile)
            self._updateUiOnSwicth(1)
            return

        if self.thread.isFinished():
            print('Starting cfg recovery...')
            self.worker = WorkerConfig(self.ser_handle)
            self.worker.moveToThread(self.thread)
            self.worker.finished.connect(self.worker.deleteLater)
            self.worker.handleUi.connect(self.handleUiFromThread) 
            self.worker.getVariantSignal.connect(self.getVariant)
            self.worker.rebootingSignal.connect(self.rebooting)
            self.worker.checkCfgSignal.connect(self.checkAllConfig)
            self.worker.ser_dll = self.ser_dll
            self.thread.started.connect(self.worker.work)
            self.thread.start()
            self.worker.fsmState = 1
            self.currentProfile = self.profiles.CFG_RECOVERY
            self.stackedWidget.setCurrentIndex(2)
            
            # UI Stuff on change
            self._updateUiOnSwicth(2)
            return    
        
        if self.currentProfile == self.profiles.FLASH and self.thread.isRunning():
            if self.switchProfileAttempt > 5:
                # Used self.thread.terminate()
                print("terminating thread: ", self.thread.terminate())
                if self.switchProfileAttempt % 6 == 0:
                    self.handleUiFromThread('SHOW_POPUP', "Warning", "Check if the serial port is connected and sending data")
            else:        
                print("is thread running: ", self.thread.isRunning())
                print('is thread finished: ', self.thread.isFinished())
                
            self.switchProfileAttempt += 1 
            print(self.switchProfileAttempt)
            QTimer.singleShot(1000, self.switchProfile)

    def _updateUiOnSwicth(self, idx):
        if idx == 1:
            self.gif = QMovie(self.getResourcePath("resources/loading.gif"), QByteArray(), self)  
            self.labelLoading.setMovie(self.gif)
            self.gif.start()
            self.labelBuild.setParent(self.page_mid)
            self.labelVersion.setParent(self.page_mid)
            self.labelBuild.show()
            self.labelVersion.show()
            return
        if idx == 2:
            # self.gif.stop()
            # self.gif.deleteLater()
            self.labelBuild.setParent(self.page_cfg)
            self.labelVersion.setParent(self.page_cfg)
            self.labelBuild.show()
            self.labelVersion.show()
            
            self.labelWait.setParent(self.page_cfg)
            self.labelWait.setText("Please Wait. Rebooting...")
            self.labelLoading.setParent(self.page_cfg)
            self.labelWait.hide()
            self.labelLoading.hide()
            return
    
        return
    
    def rebooting(self, isRebooting):
        if isRebooting:
            self.labelWait.show()
            self.labelLoading.show()
            QTimer.singleShot(1000, lambda: self.buttons.disable(True))
        else:
            self.labelWait.hide()
            self.labelLoading.hide() 
            QTimer.singleShot(1000, lambda: self.buttons.disable(False))  
         
    def getVariant(self, res):
        print("Receved getVariantSignal")
        if res is None:
            self.btnGetVariant.setEnabled(True)
            self.worker.handleUi.emit(['LOG_APPEND', 'Could not get device variant', ''])
            self.popWarning('Could not get device variant')
            print("Could not get device variant")
            return
        variant, uin = res
        self.comboBoxVariant.setCurrentText(variant)
        if not (len(uin) < 19 or uin == 'ACCOLADE123456789'):
            self.lineEditUin.setText(uin)
        self.lineEditNwsw.setEnabled(True)
        self.lineEditSimtype.setEnabled(True)
        self.lineEditUin.setEnabled(True)
        self.lineEditVin.setEnabled(True)
        self.lineEditCip2.setEnabled(True)
        self.lineEditCert.setEnabled(True)
        self.deviceVariant = variant
        self.buttons.disable(False)
        self.setupCfgButtons(self.deviceVariant)

    def startCfgRecovery(self):
        self.config_states = dict.fromkeys(['NWSW', 'SIMTYP', 'UIN', 'VIN', 'CIP2', 'CERT'], False)

        i = count(1100,2000)
        with self.buttons:
            QTimer.singleShot(next(i), lambda: self.worker.sendCommand(self.command_ids["GET NWSW"]))
            QTimer.singleShot(next(i), lambda: self.worker.sendCommand(self.command_ids["GET SIMTYP"]))
            QTimer.singleShot(next(i), lambda: self.worker.sendCommand(self.command_ids["GET CIP2"]))
            QTimer.singleShot(next(i), lambda: self.worker.sendCommand(self.command_ids["GET UIN"]))
            QTimer.singleShot(next(i), lambda: self.worker.sendCommand(self.command_ids["GET CHNO"]))
            QTimer.singleShot(next(i), lambda: self.worker.sendCommand(self.command_ids["GET CERT"]))

    def setupCfgButtons(self, variant):

        def handleNwsw(x):
            self.lineEditNwsw.setText(x[0])
            nwsw = int(x[0])
            self.updateConfigState(
                'NWSW', 
                nwsw == 1,
                'Auto NW Switching is not enabled. It will be enabled now.',
                self.lineEditNwsw,
                "SET NWSW"
            )

        def handleSimtype(x):
            self.lineEditSimtype.setText(x[0])
            simtype = int(x[0])
            self.updateConfigState(
                'SIMTYP', 
                simtype == 0,
                'SIM TYPE was not set to 0. It is now being set to 0.',
                self.lineEditSimtype,
                "SET SIMTYP"
            )

        def handleUin(x):
            uin = x[0]
            self.lineEditUin.setText(uin)
            is_valid = not (len(uin) < 19 or uin == 'ACCOLADE123456789')
            self.updateConfigState(
                'UIN', 
                is_valid,
                'UIN is not valid. Please enter a valid UIN as per the device sticker.',
                self.lineEditUin
            )

        def setUIN():
            uin = self.lineEditUin.text()
            if len(uin) < 19 or uin == 'ACCOLADE123456789':
                self.popWarning('UIN is not valid. Please enter a valid UIN as per the device sticker.')
                return
            cmd = f"CMN SET UIN:{uin}" if self.deviceVariant == "CDAC" else f"CMN *SET#UIN#{uin}#"
            self.worker.sendCommand(-1, Command(cmd, willRbt=True))
            return
            
        def handleVin(x):
            vin = x[0]
            self.lineEditVin.setText(vin)
            is_valid = not (len(vin) < 17 or vin == 'AAAAAAAAAAAAAA')
            self.updateConfigState(
                'VIN', 
                is_valid,
                'VIN is not valid. Please enter a valid VIN number as per the vehicle Chassis number.',
                self.lineEditVin
            )

        def setVIN():
            vin = self.lineEditVin.text()
            if len(vin) < 17 or vin == 'AAAAAAAAAAAAAA':
                self.popWarning('VIN is not valid. Please enter a valid VIN number as per the vehicle Chassis number.')
                return
            cmd = f"CMN SET CHNO:{vin}" if self.deviceVariant == "CDAC" else f"CMN *SET#CHNO#{vin}#"
            self.worker.sendCommand(-1, Command(cmd, willRbt=True))
            return

        def handleCip2(x):
            ip, port = x[0], x[1]
            self.lineEditCip2.setText("%s:%s" % (ip, port))
            is_valid = ip == 'ais-data.accoladeelectronics.com' and port == '5555'
            self.updateConfigState(
                'CIP2', 
                is_valid,
                'CIP2 is invalid. It will now be set to valid CIP2.',
                self.lineEditCip2,
                "SET CIP2"
            )
        
        def handleCert(x):
            certs = int(x[0]), int(x[1]), int(x[2])
            self.lineEditCert.setText("CA: %s, CC: %s, CK: %s" % (certs[0], certs[1], certs[2]))
            is_within_1_percent = lambda given, target: abs(given - target) <= 0.01 * given
            # expects cert lengths to be withing 1% of expected values
            EXPECTED_VALUES = [2048, 1984, 3294]
            is_valid = all(is_within_1_percent(cert, expected) for cert, expected in zip(certs, EXPECTED_VALUES))
            self.updateConfigState(
                'CERT', 
                is_valid,
                'One or more certificates are invalid. Please flash the valid Certificate.',
                self.lineEditCert
            )
 
        NORMAL_COMMANDS = {
            "GET NWSW": ("CMN *GET#DEVNWSW#", r"<STATUS#DEVNWSW#(\d+)#>", handleNwsw),
            "SET NWSW": ("CMN *SET#DEVNWSW#1#", None, None),
            "GET SIMTYP": ("CMN *GET#SIMTYP#", r"<STATUS#SIMTYP#(\d+)#>", handleSimtype),
            "SET SIMTYP": ("CMN *SET#SIMTYP#0#", None, None),
            "GET UIN": ("CMN *GET#UIN#", r"<STATUS#UIN#(\w+)#>", handleUin),
            "GET CHNO": ("CMN *GET#CHNO#", r"<STATUS#CHNO#(\w+)#>", handleVin),
            "GET CIP2": ("CMN *GET#CIP2#", r"<STATUS#CIP2#(.+)#(\d+)#>", handleCip2),
            "SET CIP2": ("CMN *SET#CIP2#ais-data.accoladeelectronics.com#5555#", None, None),
            "GET CERT": ("CMN *GET#CERT#", r"<STATUS#CERT#(\d+),(\d+),(\d+)#>", handleCert),
            "REBOOT": ("CMN *SET#CRST#1#", None, None),
            "EMR ACK": ("CMN *SET#SACK#1#", None, None),
        }

        # Define commands for CDAC variant
        CDAC_COMMANDS = {
            "GET NWSW": ("CMN GET DEVNWSW", r"<DEVNWSW:(\d+)>", handleNwsw),
            "SET NWSW": ("CMN SET DEVNWSW:1", None, None),
            "GET SIMTYP": ("CMN GET SIMTYP", r"<SIMTYP:(\d+)>", handleSimtype),
            "SET SIMTYP": ("CMN SET SIMTYP:0", None, None),
            "GET UIN": ("CMN GET UIN", r"<UIN:(\w+)>", handleUin),
            "GET CHNO": ("CMN GET CHNO", r"<CHNO:(\w+)>", handleVin),
            "GET CIP2": ("CMN GET CIP2", r"<CIP2:(.+):(\d+)>", handleCip2),
            "SET CIP2": ("CMN SET CIP2:ais-data.accoladeelectronics.com:5555", None, None),
            "GET CERT": ("CMN GET CERT", r"<CERT:(\d+),(\d+),(\d+)>", handleCert),
            "REBOOT": ("CMN SET CRST:1", None, None),
            "EMR ACK": ("CMN SET EO:EO", None, None),
        }

        command_table = CDAC_COMMANDS if variant == "CDAC" else NORMAL_COMMANDS
        self.command_ids = {}
        for btn, (cmd, rsp_pattern, callback) in command_table.items():
            willRbt = False
            if 'SET' in cmd and btn != 'EMR ACK': willRbt = True
            cmd_id = self.worker.registerCommand(cmd, rsp_pattern, callback, willRbt=willRbt)
            self.command_ids[btn] = cmd_id
        
        # Connect buttons with commands
        self.btnGetnSetNwsw.clicked.connect(lambda: self.worker.sendCommand(self.command_ids["GET NWSW"]))
        self.btnGetnSetSimtype.clicked.connect(lambda: self.worker.sendCommand(self.command_ids["GET SIMTYP"]))
        self.btnGetUin.clicked.connect(lambda: self.worker.sendCommand(self.command_ids["GET UIN"]))
        self.btnSetUin.clicked.connect(setUIN)
        self.btnGetVin.clicked.connect(lambda: self.worker.sendCommand(self.command_ids["GET CHNO"]))
        self.btnSetVin.clicked.connect(setVIN)
        self.btnGetCip2.clicked.connect(lambda: self.worker.sendCommand(self.command_ids["GET CIP2"]))
        self.btnGetCert.clicked.connect(lambda: self.worker.sendCommand(self.command_ids["GET CERT"]))
        self.btnReboot.clicked.connect(lambda: self.worker.sendCommand(self.command_ids["REBOOT"]))
        self.btnEmrAck.clicked.connect(lambda: self.worker.sendCommand(self.command_ids["EMR ACK"]))
    
    def updateConfigState(self, cfg, is_valid, warning_msg, line_edit_key, set_command=None):
        if is_valid:
            self.setBg(line_edit_key)
            self.config_states[cfg] = True
        else:
            self.popWarning(warning_msg)
            self.setBg(line_edit_key, 1)
            self.config_states[cfg] = False
            if set_command is not None:
                self.worker.sendCommand(self.command_ids[set_command])

        self.worker.checkCfgSignal.emit()

    def checkAllConfig(self):
        if not all(self.config_states.values()):
            return

        # send config recovery status
        SERVER_URL = "http://ais140vahan-test2.cloudjiffy.net/addDeviceCfgStatus"
        try:
            uin = self.lineEditUin.text()
            x = requests.get(SERVER_URL, [('uin', uin), ('cfg_recovery_status', 'success')])
            x.raise_for_status()

            msgBox = QMessageBox()
            msgBox.setWindowTitle("OK")
            msgBox.setIconPixmap(QPixmap(':/resources/ok.png'))
            msgBox.setText(x.json()['userMessage'])
            msgBox.exec()

            return True

        except requests.exceptions.RequestException as e:
            self.popWarning('Config Recovery Successfull, but could not update status to server')
            print(e)

    def getPage2Children(self):
        self.page_mid = self.findChild(QWidget,'page_mid')
        
        self.labelWait = self.findChild(QLabel,'labelWait')
        self.labelLoading = self.findChild(QLabel,'labelLoading')

    def getPage3Children(self):
        self.page_cfg = self.findChild(QWidget,'page_cfg')


        self.comboBoxVariant = self.findChild(QComboBox,'comboBoxVariant')
        list(map(lambda x: self.comboBoxVariant.model().item(x).setEnabled(False), range(3)))
        self.btnGetVariant = self.findChild(QPushButton,'btnGetVariant')
        self.btnStartCfgRecovery = self.findChild(QPushButton,'btnStartCfgRecovery')

        self.btnReboot = self.findChild(QPushButton,'btnReboot')
        self.btnEmrAck = self.findChild(QPushButton,'btnEmrAck')
        
        self.lineEditNwsw = self.findChild(QLineEdit,'lineEditNwsw') 
        self.btnGetnSetNwsw = self.findChild(QPushButton,'btnGetnSetNwsw')
        self.btnGetnSetNwsw.hide()
        
        self.lineEditSimtype = self.findChild(QLineEdit,'lineEditSimtype')
        self.btnGetnSetSimtype = self.findChild(QPushButton,'btnGetnSetSimtype')
        self.btnGetnSetSimtype.hide()
                
        self.lineEditUin = self.findChild(QLineEdit,'lineEditUin')
        self.btnGetUin = self.findChild(QPushButton,'btnGetUin')
        self.btnSetUin = self.findChild(QPushButton,'btnSetUin')
        
        self.lineEditVin = self.findChild(QLineEdit,'lineEditVin')
        self.btnGetVin = self.findChild(QPushButton,'btnGetVin')
        self.btnSetVin = self.findChild(QPushButton,'btnSetVin')

        self.lineEditCip2 = self.findChild(QLineEdit,'lineEditCip2')
        self.btnGetCip2 = self.findChild(QPushButton,'btnGetCip2')
        self.btnGetCip2.hide()

        self.lineEditCert = self.findChild(QLineEdit,'lineEditCert')
        self.btnGetCert = self.findChild(QPushButton,'btnGetCert')

        self.btnFlashCert = self.findChild(QPushButton,'btnFlashCert')
        self.btnFlashCert.clicked.connect(self.restart)


        def getVariantOnClick():
            self.btnGetVariant.setEnabled(False)
            self.worker.fsmState = 4

        self.btnGetVariant.clicked.connect(getVariantOnClick)
        
        self.btnStartCfgRecovery.clicked.connect(self.startCfgRecovery)
        
        class Buttons(list): 
            def disable(self, v):list(map(lambda x: x.setDisabled(v), self))
            def timeout(self):[self.disable(True), QTimer.singleShot(1000, lambda: self.disable(False))]
            def __enter__(self):self.disable(True)
            def __exit__(self, type, value, traceback):self.disable(False)
            
        self.buttons = Buttons(self.page_cfg.findChildren(QPushButton))
        # list(map(lambda btn: btn.clicked.connect((lambda btn: lambda: print("BTN_CLICKED:", btn.text()))(btn)), self.buttons))
        self.buttons.remove(self.btnGetVariant)
        self.buttons.disable(True)
        list(map(lambda btn: btn.clicked.connect((lambda btn: lambda: (print("BTN_CLICKED:", btn.text()),self.buttons.timeout()))(btn)), self.buttons))
        # list(map(lambda x: x.clicked.connect(lambda: self.buttons.timeout()), self.buttons))

    def popWarning(self, text):
        self.worker.handleUi.emit(['SHOW_POPUP', 'Warning', text])

    def setBg(self, widget, color=0):
        """
        if color is `1` Background will be set to danger
        if color is `0` Background will be set to success
        """
        if color:
            widget.setStyleSheet("background-color: rgba(255, 0, 0, 30%);")
        else:
            widget.setStyleSheet("background-color: rgba(0, 255, 0, 30%);")

    @staticmethod
    def restart():
        execl(sys.executable, sys.executable, *sys.argv)
