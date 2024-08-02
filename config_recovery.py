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
from ctypes import create_string_buffer
from enum import IntEnum
from queue import Queue
from threading import Thread
from itertools import count

import re
import time
from PyQt5.QtCore import QObject, pyqtSignal, pyqtSlot, QTimer, QByteArray
from PyQt5.QtWidgets import QWidget, QVBoxLayout, QGridLayout, QLabel, QComboBox, QLineEdit, QPushButton
from PyQt5.QtGui import QMovie


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
    ser_dll = None
    state = IntEnum('State', ['IDLE', 'TRANSPARENT', 'SENDIND', 'WAITING_RESPONSE'], start=0)
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
                        self.fsmState = self.state.SENDIND
                except Exception as e:
                    print(e)
                    pass
                
            if self.fsmState == self.state.SENDIND:
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
                    print(attempt)
                if not found:
                    self.logAppend("Failed to get response for command: %s" % self.currentCmd.cmd)
                    self.handleUi.emit(['SHOW_POPUP', 'Warning',"Failed to get response for command: %s" % self.currentCmd.cmd])

                
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

    
    def getDeviceVariant(self, cb=None):
        print("Get Device Variant")
        if self.ser_dll is None or self.ser_handle is None:
            self.handleUi.emit(['LOG_APPEND', "Serial port not initialized", ""])
            return None
        
        self.fsmState = self.state.IDLE
        
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
            if attempt == 10:
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
    
        self.fsmState = self.state.TRANSPARENT
        # cb(res)
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
            self.worker.ser_dll = self.ser_dll
            self.thread.started.connect(self.worker.work)
            self.thread.start()
            self.worker.fsmState = 1
            self.currentProfile = self.profiles.CFG_RECOVERY
            self.stackedWidget.setCurrentIndex(2) # TODO: set to 2
            
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
        print("Receved Callback")
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
        self.lineEditUin.setEnabled(True)
        self.lineEditVin.setEnabled(True)
        self.lineEditCip2.setEnabled(True)
        self.deviceVariant = variant
        self.buttons.disable(False)
        self.setupCfgButtons(self.deviceVariant)

    def setupCfgButtons(self, variant):
        
        def handleNwsw(x):
            self.lineEditNwsw.setText(x[0])
            nwsw = int(x[0])
            if nwsw != 1:
                # self.handleUiFromThread('SHOW_POPUP', 'Warning', 'Auto NW Switching was not enabled. Enabling it.')
                self.popWarning('Auto NW Switching is not enabled. Enabling it.')
                self.worker.sendCommand(self.command_ids["SET NWSW"])

        def handleSimtype(x):
            self.lineEditSimtype.setText(x[0])
            simtype = int(x[0])
            if simtype != 0:
                self.popWarning('SIM TYPE was not set to 0. Setting it.')
                self.worker.sendCommand(self.command_ids["SET SIMTYP"])

        def handleUin(x):
            uin = x[0]
            self.lineEditUin.setText(uin)
            if len(uin) < 19 or uin == 'ACCOLADE123456789':
                self.popWarning('UIN is not valid.')

        def setUIN():
            uin = self.lineEditUin.text()
            if len(uin) < 19 or uin == 'ACCOLADE123456789':
                self.popWarning('UIN is not valid.')
                return
            cmd = f"CMN SET UIN:{uin}" if self.deviceVariant == "CDAC" else f"CMN *SET#UIN#{uin}#"
            self.worker.sendCommand(-1, Command(cmd, willRbt=True))
            return
            
        def handleVin(x):
            vin = x[0]
            self.lineEditVin.setText(vin)
            if len(vin) < 17 or vin == 'AAAAAAAAAAAAAA':
                self.popWarning('VIN/CHNO is not valid.')

        def setVIN():
            vin = self.lineEditVin.text()
            if len(vin) < 17 or vin == 'AAAAAAAAAAAAAA':
                self.popWarning('VIN/CHNO is not valid.')
                return
            cmd = f"CMN SET CHNO:{vin}" if self.deviceVariant == "CDAC" else f"CMN *SET#CHNO#{vin}#"
            self.worker.sendCommand(-1, Command(cmd, willRbt=True))
            return

            
        def handleCip2(x):
            ip, port = x[0], x[1]
            self.lineEditCip2.setText("%s:%s" % (ip, port))
            if ip != 'ais-data.accoladeelectronics.com':
                self.popWarning('Invalid IP address for CIP2.')
            elif port != '5555':
                self.worker.handleUi.emit(['SHOW_POPUP', 'Warning', 'Invalid port for CIP2.'])
                self.popWarning('Invalid port for CIP2.')
        
        
            
        NORMAL_COMMANDS = {
            "GET NWSW": ("CMN *GET#DEVNWSW#", r"<STATUS#DEVNWSW#(\d)#>", handleNwsw),
            "SET NWSW": ("CMN *SET#DEVNWSW#1#", None, None),
            "GET SIMTYP": ("CMN *GET#SIMTYP#", r"<STATUS#SIMTYP#(\d)#>", handleSimtype),
            "SET SIMTYP": ("CMN *SET#SIMTYP#0#", None, None),
            "GET UIN": ("CMN *GET#UIN#", r"<STATUS#UIN#(\w+)#>", handleUin),
            "GET CHNO": ("CMN *GET#CHNO#", r"<STATUS#CHNO#(\w+)#>", handleVin),
            "GET CIP2": ("CMN *GET#CIP2#", r"<STATUS#CIP2#(.+)#(\d+)#>", handleCip2),
            "SET CIP2": ("CMN *SET#CIP2#ais-data.accoladeelectronics.com#5555#", None, None),
            "REBOOT": ("CMN *SET#CRST#1#", None, None),
            "EMR ACK": ("CMN *SET*SACK#1#", None, None),
        }

        # Define commands for CDAC variant
        CDAC_COMMANDS = {
            "GET NWSW": ("CMN GET DEVNWSW", r"<DEVNWSW:(\d)>", handleNwsw),
            "SET NWSW": ("CMN SET DEVNWSW:1", None, None),
            "GET SIMTYP": ("CMN GET SIMTYP", r"<SIMTYP:(\d)>", handleSimtype),
            "SET SIMTYP": ("CMN SET SIMTYP:0", None, None),
            "GET UIN": ("CMN GET UIN", r"<UIN:(\w+)>", handleUin),
            "GET CHNO": ("CMN GET CHNO", r"<CHNO:(\w+)>", handleVin),
            "GET CIP2": ("CMN GET CIP2", r"<CIP2:(.+):(\d+)>", handleCip2),
            "SET CIP2": ("CMN SET CIP2:ais-data.accoladeelectronics.com:5555", None, None),
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
        self.btnSetCip2.clicked.connect(lambda: self.worker.sendCommand(self.command_ids["SET CIP2"]))
        self.btnReboot.clicked.connect(lambda: self.worker.sendCommand(self.command_ids["REBOOT"]))
        self.btnEmrAck.clicked.connect(lambda: self.worker.sendCommand(self.command_ids["EMR ACK"]))

    def getPage2Children(self):
        self.page_mid = self.findChild(QWidget,'page_mid')
        
        self.labelWait = self.findChild(QLabel,'labelWait')
        self.labelLoading = self.findChild(QLabel,'labelLoading')

    def getPage3Children(self):
        self.page_cfg = self.findChild(QWidget,'page_cfg')

        layout = QVBoxLayout(self.page_cfg)

        hlayout1 = QGridLayout()
        labelVariant = QLabel(self, text="Variant")
        self.comboBoxVariant = QComboBox(self)
        self.comboBoxVariant.addItems(["Select Variant","Normal","CDAC"])
        list(map(lambda x: self.comboBoxVariant.model().item(x).setEnabled(False), range(3)))
        self.btnGetVariant = QPushButton(self, text="GET VARIANT")
        self.btnReboot = QPushButton(self, text="REBOOT")
        
        def getVariantOnClick():
            self.btnGetVariant.setEnabled(False)
            self.worker.fsmState = 4

        self.btnGetVariant.clicked.connect(getVariantOnClick)
        
        hlayout1.addWidget(labelVariant, 0, 0)
        hlayout1.addWidget(self.comboBoxVariant, 0, 1)
        hlayout1.addWidget(self.btnGetVariant, 0, 2)
        hlayout1.addWidget(self.btnReboot, 0, 4)
        
        [hlayout1.setColumnStretch(i, 1) for i in range(5)]
                
        hlayout2 = QGridLayout()
        i = count(0)

        labelNwsw = QLabel(self, text="Auto NW Switching")
        self.lineEditNwsw = QLineEdit(self); self.lineEditNwsw.setDisabled(True)
        self.btnGetnSetNwsw = QPushButton(self, text="GETnSet NWSW")
        
        labelSimtype = QLabel(self, text="Sim Type")
        self.lineEditSimtype = QLineEdit(self); self.lineEditSimtype.setDisabled(True)
        self.btnGetnSetSimtype = QPushButton(self, text="GETnSet SIMTYPE")
                
        hlayout2.addWidget(labelNwsw, 0, next(i))
        hlayout2.addWidget(self.lineEditNwsw, 0, next(i))
        hlayout2.addWidget(self.btnGetnSetNwsw, 0, next(i))
                
        hlayout2.addWidget(QLabel(self, text=""), 0, next(i))
                
        hlayout2.addWidget(labelSimtype, 0, next(i))
        hlayout2.addWidget(self.lineEditSimtype, 0, next(i))
        hlayout2.addWidget(self.btnGetnSetSimtype, 0, next(i))
        
        [hlayout2.setColumnStretch(_, 1) for _ in range(next(i))]
        
        hlayout3 = QGridLayout()
        
        labelUin = QLabel(self, text="UIN")
        self.lineEditUin = QLineEdit(self)
        self.lineEditUin.setDisabled(True)
        self.btnGetUin = QPushButton(self, text="GET UIN")
        self.btnSetUin = QPushButton(self, text="SET UIN")
        
        hlayout3.addWidget(labelUin, 0, 0)
        hlayout3.addWidget(self.lineEditUin, 0, 1, 1, 2)
        hlayout3.addWidget(self.btnGetUin, 1, 1)
        hlayout3.addWidget(self.btnSetUin, 1, 2)
        
        hlayout3.addWidget(QLabel(self, text=""), 0, 3)

        labelVIN = QLabel(self, text="VIN")
        self.lineEditVin = QLineEdit(self)
        self.lineEditVin.setDisabled(True)
        self.btnGetVin = QPushButton(self, text="GET VIN")
        self.btnSetVin = QPushButton(self, text="SET VIN")

        hlayout3.addWidget(labelVIN, 0, 4)
        hlayout3.addWidget(self.lineEditVin, 0, 5, 1, 2)
        hlayout3.addWidget(self.btnGetVin, 1, 5)
        hlayout3.addWidget(self.btnSetVin, 1, 6)
        
        [hlayout3.setColumnStretch(i, 1) for i in range(7)]
        
        hlayout4 = QGridLayout()
        
        labelCip2 = QLabel(self, text="CIP2")
        self.lineEditCip2 = QLineEdit(self)
        self.lineEditCip2.setReadOnly(True)
        self.lineEditCip2.setDisabled(True)
        self.btnGetCip2 = QPushButton(self, text="GET CIP2")
        self.btnSetCip2 = QPushButton(self, text="SET CIP2")
        self.btnSetCip2.hide()
        
        hlayout4.addWidget(labelCip2, 0, 0)
        hlayout4.addWidget(self.lineEditCip2, 0, 1, 1, 3)
        hlayout4.addWidget(self.btnGetCip2, 0, 4)
        hlayout4.addWidget(self.btnSetCip2, 0, 5)
        
        self.btnEmrAck = QPushButton(self, text="EMR ACK")
        
        hlayout4.addWidget(self.btnEmrAck, 0, 7)
        
        [hlayout4.setColumnStretch(i, 1) for i in range(8)]

        layout.insertSpacing(0, 50)
        layout.addLayout(hlayout1)
        layout.addLayout(hlayout2)
        layout.addLayout(hlayout3)
        layout.addLayout(hlayout4)
        [layout.setStretch(i, 1) for i in range(2)]
        
        self.page_cfg.setLayout(layout)
        
        class Buttons(list): 
            def disable(self, v):list(map(lambda x: x.setDisabled(v), self))
            def timeout(self):[self.disable(True), QTimer.singleShot(1000, lambda: self.disable(False))]
            
        self.buttons = Buttons(self.page_cfg.findChildren(QPushButton))
        # list(map(lambda btn: btn.clicked.connect((lambda btn: lambda: print("BTN_CLICKED:", btn.text()))(btn)), self.buttons))
        self.buttons.remove(self.btnGetVariant)
        self.buttons.disable(True)
        list(map(lambda btn: btn.clicked.connect((lambda btn: lambda: (print("BTN_CLICKED:", btn.text()),self.buttons.timeout()))(btn)), self.buttons))
        # list(map(lambda x: x.clicked.connect(lambda: self.buttons.timeout()), self.buttons))


    def popWarning(self, text):
        self.worker.handleUi.emit(['SHOW_POPUP', 'Warning', text])