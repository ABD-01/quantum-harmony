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
import snoop

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
                        start_time = time.time()
                        while time.time() - start_time < self.rbtWaitTimer:
                            self.ser_dll.readline(self.ser_handle, self.rxBuffer, 1024)
                            rxData = self.rxBuffer.value.decode('utf-8', 'ignore')
                            print("rx <- \033[0;36m%s\033[0m" % rxData.strip())
                            if re.search('BOOT SIGNATURE', rxData):
                                break
                            
                    if self.commandQueue.empty():
                        self.ser_dll.readline(self.ser_handle, self.rxBuffer, 1024)
                        rxData = self.rxBuffer.value.decode('utf-8', 'ignore')
                        # print("rx <- " + rxData)

                        self.handleUi.emit(['LOG_APPEND', rxData, ''])
                    else:
                        self.fsmState = self.state.SENDIND
                except Exception as e:
                    print(e)
                    pass
                
            if self.fsmState == self.state.SENDIND:
                self.currentCmd = self.commandQueue.get()
                self.ser_dll.serial_write(self.ser_handle, self.currentCmd.cmd.encode())        
                self.logAppend("Sending command: %s" % self.currentCmd.name)
                
                if self.currentCmd.rsp_pattern:
                    self.fsmState = self.state.WAITING_RESPONSE
                else:
                    self.fsmState = self.state.TRANSPARENT
                
            if self.fsmState == self.state.WAITING_RESPONSE:
                start_time = time.time()
                while time.time() - start_time < 1:
                    self.ser_dll.serial_read(self.ser_handle, self.rxBuffer, 1024)
                self.rxBuffer.value = b''
                
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
                        self.logAppend("Response received: {}".format(match.string))
                        break
                    attempt += 1
                    print(attempt)
                if not found:
                    self.logAppend("Failed to get response for command: %s" % self.currentCmd.name)
                
                self.fsmState = self.state.TRANSPARENT
                self.currentCmd = None     
            
        self.finished.emit()
    
    def registerCommand(self, cmd, rsp=None, cb=None, timeout=15, willRbt=False) -> int:
        self.commands.append(
            Command(cmd, re.compile(rsp) if rsp else None, cb, timeout, willRbt=willRbt)
        )
        return len(self.commands) - 1

    @pyqtSlot(result=bool)
    def sendCommand(self, cmdId, command=None):
        if cmdId >= len(self.commands):
            return False
        if self.commandQueue.full():
            return False
        
        
        if cmdId == -1:
            # Incase command is not registered
            if command is None:
                return False
            self.commandQueue.put(command)
            return True 

        self.commandQueue.put(self.commands[cmdId])
        return True
        
    def logAppend(self, text):
        self.handleUi.emit(['LOG_APPEND', "<h3><span style='color:cyan'>%s</span></h3>" %text, ''])

    
    def getDeviceVariant(self, cb):
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
            if attempt == 10: # TODO: adjust this 
                self.handleUi.emit(['LOG_APPEND', "Failed to get UIN...", ''])
                looprun = False

        if res is None:
            # time.sleep(1) # is this even needed??
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
                if attempt == 10: # TODO: adjust this
                    self.handleUi.emit(['LOG_APPEND', "Failed to get UIN...", ''])
                    looprun = False
    
        self.fsmState = self.state.TRANSPARENT
        cb(res)             
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
            # self.worker = Worker2()
            self.worker = WorkerConfig(self.ser_handle)
            self.worker.moveToThread(self.thread)
            self.worker.finished.connect(self.worker.deleteLater)
            self.worker.handleUi.connect(self.handleUiFromThread) 
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
                    self.handleUiFromThread(None, 'SHOW_POPUP', "Warning", "Check if the serial port is connected and sending data")
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
            self.gif.stop()
            self.gif.deleteLater()
            self.labelBuild.setParent(self.page_cfg)
            self.labelVersion.setParent(self.page_cfg)
            self.labelBuild.show()
            self.labelVersion.show()
            return
    
        return
            
    def getVariant(self, res):
        print("Receved Callback")
        if res is None:
            self.btnGetVariant.setEnabled(True)
            self.worker.handleUi.emit(['LOG_APPEND', 'Could not get device variant', ''])
            self.worker.handleUi.emit(['SHOW_POPUP', 'Warning','Could not get device variant'])
            print("Could not get device variant")
            return
        variant, uin = res
        self.comboBoxVariant.setCurrentText(variant)
        self.lineEditUin.setText(uin)
        self.lineEditUin.setEnabled(True)
        self.lineEditVin.setEnabled(True)
        self.lineEditCip2.setEnabled(True)
        self.deviceVariant = variant
        self.setupCfgButtons(self.deviceVariant)

    def getPage2Children(self):
        self.page_mid = self.findChild(QWidget,'page_mid')
        
        self.labelWait = self.findChild(QLabel,'labelWait')
        self.labelLoading = self.findChild(QLabel,'labelLoading')

    def getPage3Children(self):
        self.page_cfg = self.findChild(QWidget,'page_cfg')

        layout = QVBoxLayout()

        hlayout1 = QGridLayout()
        labelVariant = QLabel(self, text="Variant")
        self.comboBoxVariant = QComboBox(self)
        self.comboBoxVariant.addItems(["Select Variant","Normal","CDAC"])
        list(map(lambda x: self.comboBoxVariant.model().item(x).setEnabled(False), range(3)))
        self.btnGetVariant = QPushButton(self, text="GET VARIANT")
        
        def getVariant():
            self.btnGetVariant.setEnabled(False)
            Thread(target=self.worker.getDeviceVariant, args=(self.getVariant,)).start()
            # self.worker.getDeviceVariant(self.getVariant)

        self.btnGetVariant.clicked.connect(getVariant)
        
        hlayout1.addWidget(labelVariant, 0, 0)
        hlayout1.addWidget(self.comboBoxVariant, 0, 1)
        hlayout1.addWidget(self.btnGetVariant, 0, 2)
        
        [hlayout1.setColumnStretch(i, 1) for i in range(4)]
                
        hlayout2 = QGridLayout()
        i = count(0)

        labelNwsw = QLabel(self, text="Auto NW Switching")
        lineEditNwsw = QLineEdit(self); lineEditNwsw.setDisabled(True)
        self.btnGetnSetNwsw = QPushButton(self, text="GETnSet NWSW")
        
        labelSimtype = QLabel(self, text="Sim Type")
        lineEditSimtype = QLineEdit(self); lineEditSimtype.setDisabled(True)
        self.btnGetnSetSimtype = QPushButton(self, text="GETnSet SIMTYPE")
                
        hlayout2.addWidget(labelNwsw, 0, next(i))
        hlayout2.addWidget(lineEditNwsw, 0, next(i))
        hlayout2.addWidget(self.btnGetnSetNwsw, 0, next(i))
                
        hlayout2.addWidget(QLabel(self, text=""), 0, next(i))
                
        hlayout2.addWidget(labelSimtype, 0, next(i))
        hlayout2.addWidget(lineEditSimtype, 0, next(i))
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
        
        labelVIN = QLabel(self, text="VIN")
        self.lineEditVin = QLineEdit(self)
        self.lineEditVin.setDisabled(True)
        self.btnGetVin = QPushButton(self, text="GET VIN")
        self.btnSetVin = QPushButton(self, text="SET VIN")
        
        hlayout3.addWidget(labelVIN, 0, 3)
        hlayout3.addWidget(self.lineEditVin, 0, 4, 1, 2)
        hlayout3.addWidget(self.btnGetVin, 1, 4)
        hlayout3.addWidget(self.btnSetVin, 1, 5)
        
        labelCip2 = QLabel(self, text="CIP2")
        self.lineEditCip2 = QLineEdit(self)
        self.lineEditCip2.setDisabled(True)
        self.btnGetCip2 = QPushButton(self, text="GET CIP2")
        self.btnSetCip2 = QPushButton(self, text="SET CIP2")
        
        hlayout3.addWidget(labelCip2, 0, 6)
        hlayout3.addWidget(self.lineEditCip2, 0, 7, 1, 2)
        hlayout3.addWidget(self.btnGetCip2, 1, 7)
        hlayout3.addWidget(self.btnSetCip2, 1, 8)
        
        [hlayout3.setColumnStretch(i, 1) for i in range(9)]
        
        hlayout4 = QGridLayout()
        
        self.btnReboot = QPushButton(self, text="REBOOT")
        self.btnEmrAck = QPushButton(self, text="EMR ACK")
        
        hlayout4.addWidget(self.btnReboot, 0, 0)
        hlayout4.addWidget(self.btnEmrAck, 0, 1)
        
        [hlayout4.setColumnStretch(i, 1) for i in range(5)]

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
        list(map(lambda btn: btn.clicked.connect((lambda btn: lambda: print("BTN_CLICKED:", btn.text()))(btn)), self.buttons))
        self.buttons.remove(self.btnGetVariant)
        self.buttons.disable(True)
        list(map(lambda x: x.clicked.connect(lambda: self.buttons.timeout()), self.buttons))
        
            
    def setupCfgButtons(self, variant):
        
        cfgWorker = self.worker
        
        def handleNwsw(x):
            self.lineEditNwsw.setText(x[0])
            
            nwsw = int(x[0])
            if nwsw != 1:
                buff = create_string_buffer(b'CMN *SET#DEVNWSW#1#')
                self.ser_dll.serial_write(self.ser_handle, buff, len(buff))
        
        def handleResponse(x):
            self.handleUiFromThread(None, 'LOG_APPEND', f"Reponse from {x.cmd}:\n{x.rsp}")
            
        cfgWorker.registerCommand(
            "CMN *GET#DEVNWSW#", r"<STATUS#DEVNWSW#(\d)#>",
            handleResponse,
        )
        cfgWorker.registerCommand(
            "CMN *SET#DEVNWSW#1#", None,
            handleResponse,
        )
        cfgWorker.registerCommand(
            "CMN GET DEVNWSW", r"<DEVNWSW:(\d)>",
            handleResponse,
        )
        cfgWorker.registerCommand(
            "CMN SET DEVNWSW:1", None,
            handleResponse,
        )
        cfgWorker.registerCommand(
            "CMN *GET#SIMTYP#", r"<STATUS#SIMTYP#(\d)#>",
            handleResponse,
        )
        cfgWorker.registerCommand(
            "CMN *SET#SIMTYP#0#", None,
            handleResponse,
        )
        cfgWorker.registerCommand(
            "CMN GET SIMTYP", r"<SIMTYP:(\d)>",
            handleResponse,
        )
        cfgWorker.registerCommand(
            "CMN SET SIMTYP:0", None,
            handleResponse,
        )

        cfgWorker.registerCommand(
            "CMN *GET#UIN#", r"<STATUS#UIN#(\w+)#>",
            handleResponse,
        )

        cfgWorker.registerCommand(
            "CMN GET UIN", r"<UIN:(\w+)>",
            handleResponse,
        )
        cfgWorker.registerCommand(
            "CMN *GET#CHNO#", r"<STATUS#CHNO#(\w+)#>",
            handleResponse,
        )
        cfgWorker.registerCommand(
            "CMN GET CHNO", r"<CHNO:ACCDEV07240045162>",
            handleResponse,
        )
        
        cfgWorker.registerCommand(
            "CMN *GET#CIP2#", r"<STATUS#CIP2#(.+)#(\d+)#>",
            handleResponse,
        )
        cfgWorker.registerCommand(
            "CMN *SET#CIP2#ais-data.accoladeelectronics.com#5555#", None,
            handleResponse,
        )
        cfgWorker.registerCommand(
            "CMN GET CIP2", r"<CIP2:(.+):(\d+)>",
            handleResponse,
        )
        cfgWorker.registerCommand(
            "CMN SET CIP2:ais-data.accoladeelectronics.com:5555", None,
            handleResponse,
        )
        
        cfgWorker.registerCommand(
            "CMN *SET#CRST#1#", None,
            handleResponse,
        )
        cfgWorker.registerCommand(
            "CMN SET CRST:1", None,
            handleResponse,
        )
        cfgWorker.registerCommand(
            "CMN *SET*SACK#1#", None,
            handleResponse,
        )
        cfgWorker.registerCommand(
            "CMN SET EO:EO", None,
            handleResponse,
        )
            