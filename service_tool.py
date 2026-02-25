#!/usr/bin/env python
'''\

Copyright 2023 Accolade Electronics Pvt. Ltd

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

file        service_tool.py
brief       This is the main script file for launching the service tool UI

date        31 Dec 2022
author      Eshwar J <eshwar.jorvekar@accoladeelectronics.com>
            Muhammed Abdullah <muhammed.shaikh@accoladeelectronics.com>

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

import sys, serial, time, os, resources, re, tarfile, shutil, zipfile, socket

from PyQt5 import QtWidgets, uic
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *

import serial.tools.list_ports as ports
from distutils.filelist import FileList
from numpy import byte
from packaging.version import Version

VERSION='1.4.1'

try:
    from ctypes import windll  # Only exists on Windows.
    myappid = f'AEPL.ATCU.ServiceTool.{VERSION}'
    windll.shell32.SetCurrentProcessExplicitAppUserModelID(myappid)
except ImportError:
    pass

# Globals
projects_list = [
    ('TCU Wifi and non Wifi', Version('5.2.7')), 
    ('TCU ethernet', Version('6.2.0')),
    ('Power Train DL', Version('2.0.14')), 
    ('Wifi DL', Version('1.1.1')), 
    ('PDL Gen 4', Version('7.2.0')), 
    ('ATCU IB', Version('8.1.1')), 
    ]

# MULTI-THREADING
class Worker(QObject):
    finished = pyqtSignal()
    intReady = pyqtSignal(str)
    handleUi = pyqtSignal(list)             # for modifying UI elements from thread
    fsmState = 'IDLE'
    startTime = 0
    endTime = 0
    tcpPort = 0
        
    def __init__(self):
        super(Worker, self).__init__()
        self.working = True

    def work(self):
        while self.working:
            if self.fsmState == 'IDLE':
                pass
            
            ''' Only applicable for 'SERIAL' interface. 
            All UART data received on the connected port will be shown in log window '''
            if self.fsmState == 'UART_TRANSPARENT_MODE':
                try:
                    if(ser.isOpen()):
                        while ser.inWaiting():
                            rxData = str(ser.readline(), 'utf-8', 'ignore')
                            
                            # in case of device reboot, search this boot print and re-enable buttons by emiting 0
                            if re.search(r'applying log filters', rxData):
                                self.handleUi.emit(['RE-ENABLE_BUTTONS', '', ''])
                                self.handleUi.emit(['SET_PROGRESS', 0, ''])
                                
                            # strip the VT color codes that are received from the device before showing in log window
                            cleanString = re.sub(r'\x1b\[\d+m', '', rxData)
                            self.handleUi.emit(['LOG_APPEND', cleanString, ''])
                except Exception as e:
                    print(e)
                    pass
                
            ''' PREVALIDATE can only be done over 'SERIAL' for FIRMWARE or CERTIFICATE
            FIRMWARE PREVALIDATE:    Internal battery must be above 3.8V. get io status command *GET#IOST#
            CERTIFICATE PREVALIDATE: TCU4G must run atleast 5.2.7. PDL must run atleast 2.0.14. get fw info command *GET#FWSTAT# '''
            if self.fsmState == 'PREVALIDATE':
                attempt = 0
                # ser.write(bytes(b'*SET#LOGFLTR#65533#'))    # TODO eshwar: last resort
                try:
                    if(ser.isOpen()):
                        print('checking battery info')
                        # ensure good internal battery voltage before serial DFU
                        loopRun = True
                        batteryChecksPassed = False
                        ser.flushInput()
                        ser.write(b'*GET#IOST#')
                        while loopRun == True:
                            while ser.inWaiting():
                                rxData = str(ser.readline(), 'utf-8', 'ignore')
                                if re.search(r'STATUS#IOST#', rxData):
                                    # Use re.findall to find all Regular expression pattern to match the decimal number in rxData
                                    matches = re.findall(r'\d+\.\d+', rxData)
                                    if len(matches) >= 2:
                                        externalBatteryVtg = float(matches[0])
                                        internalBatteryVtg = float(matches[1])
                                        if internalBatteryVtg >= 3.6 and externalBatteryVtg >= 8:
                                            print(f'battery checks passed, IBAT {internalBatteryVtg}V EBAT {externalBatteryVtg}V')
                                            batteryChecksPassed = True
                                            loopRun = False
                                            break
                                        else:
                                            print(f'battery checks failed, IBAT {internalBatteryVtg}V EBAT {externalBatteryVtg}V')
                                            self.handleUi.emit(['PREVALIDATION_ERR', '', ''])
                                            self.handleUi.emit(['SHOW_POPUP', 'Prevalidation failed', f'Battery checks failed IBAT {internalBatteryVtg}V EBAT {externalBatteryVtg}V\nRequired minimum internal battery 3.6V\nRequired minimum external battery 8V\nAborting...'])
                                            self.fsmState = 'UART_TRANSPARENT_MODE'
                                            loopRun = False
                                            break
                                elif attempt == 10:
                                    self.handleUi.emit(['PREVALIDATION_ERR', '', ''])
                                    self.handleUi.emit(['SHOW_POPUP', 'Prevalidation failed', 'Timed out during detecting internal battery voltage from device\nPlease try again'])
                                    self.fsmState = 'UART_TRANSPARENT_MODE'
                                    loopRun = False
                                    break
                                else:
                                    attempt = attempt + 1
                                    continue
                        if batteryChecksPassed:
                            loopRun = True
                            # ensure firmware support first, must be according to the table
                            print('checking firmware compatibility')
                    
                        ser.flushInput()
                        ser.write(b'*GET#FWSTAT#')
                        while loopRun == True:
                            while ser.inWaiting():
                                rxData = str(ser.readline(), 'utf-8', 'ignore')
                                print('rx <- ', rxData)
                                if re.search(r'STATUS#FWSTAT#\d+\.', rxData):
                                    result = re.findall(r'\d+\.\d+\.\d+', rxData)
                                    if result:
                                        fwStatVersion = Version(result[0])
                                        print("Current Firmware Version: ", fwStatVersion)
                                        print("Major: %d, Minor: %d, Micro: %d" % (fwStatVersion.major, fwStatVersion.minor, fwStatVersion.micro))
                                        certificatesCompatible = None
                                        if fwStatVersion.major == requiredVersion.major:
                                            if upgradeType == 'FIRMWARE':
                                                print(f'version checks passed for firmware flashing. {currentProject} current major {fwStatVersion.major}, required major {requiredVersion.major}')
                                                self.fsmState = 'WRITE_FILES_LEGACY'
                                                loopRun = False
                                                break
                                            if upgradeType == 'CERTIFICATE' and fwStatVersion.micro >= requiredVersion.micro:
                                                print(f'version checks passed for certificate flashing. {currentProject} current {fwStatVersion}, required {requiredVersion}')
                                                self.fsmState = 'WRITE_FILES'
                                                loopRun = False
                                                break
                                            else:
                                                certificatesCompatible = False
                                        if (fwStatVersion.major != requiredVersion.major) or certificatesCompatible == False:
                                            print(f'version checks failed. {currentProject} current {fwStatVersion}, required minimum {requiredVersion}')
                                            self.handleUi.emit(['PREVALIDATION_ERR', '', ''])
                                            self.handleUi.emit(['SHOW_POPUP', 'Device or version conflict', f'Incorrect device or device is running incompatible firmware\nRequired minimum {requiredVersion} for {currentProject}\nAborting...'])
                                            self.fsmState = 'UART_TRANSPARENT_MODE'
                                            loopRun = False
                                            break
                                elif attempt == 10:
                                    self.handleUi.emit(['PREVALIDATION_ERR', '', ''])
                                    self.handleUi.emit(['SHOW_POPUP', 'Prevalidation failed', 'Timed out during detecting AEPL_FIRMWARE_VERSION\nPlease try again'])
                                    self.fsmState = 'UART_TRANSPARENT_MODE'
                                    loopRun = False
                                    break
                                else:
                                    attempt = attempt + 1
                                    continue
                except Exception as e:
                    self.handleUi.emit(['SHOW_POPUP', 'def work:', f'PREVALIDATE\n{e}'])
                    return
                
            ''' Uses common protocol (55AA) suports 'SERIAL' and 'TCP' interface. See v0.3 dfu and certificate upgrade protocol specifications.pdf.
            'SERIAL' : firmware and certificates can be transferred, PRAVALIDATION will be done
            'TCP'    : only firmware can be transferred, PRAVALIDATION will not be done '''
            if self.fsmState == 'WRITE_FILES':
                global conn
                if commInterface == 'TCP':
                    soc  = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    soc.settimeout(3600)
                    soc.bind(('', self.tcpPort))
                    self.handleUi.emit(['LOG_APPEND', f'Waiting for incoming connections on port {self.tcpPort}...', ''])
                    soc.listen()
                    conn, addr = soc.accept()
                    self.handleUi.emit(['LOG_APPEND', f'Client connected {addr}', ''])
                try:
                    if commInterface == 'SERIAL':
                        ser.flushInput()
                    
                    if commInterface == 'TCP':
                        while True:
                            ''' FIRMWARE login packet is expected from device side as the server must know the connected device '''
                            rxData = conn.recv(2048)
                            print('rx <- ', rxData)
                            time.sleep(2)
                            break
                        
                    self.handleUi.emit(['DISABLE_BUTTONS', '', ''])
                    
                    ''' in case of firmware, file count will be 1 and in case of certificate, it will be 3, order of certificates is fixed at CA CC CK '''
                    fileCount = fileList.__len__()
                    self.handleUi.emit(['LOG_APPEND', f'Starting upload of {fileCount} file(s)...', ''])
                    
                    self.startTime = time.time()
                    
                    for idx in range(fileCount):
                        time.sleep(0.1)
                        currentFilePath = fileList[idx]
                        totalFileSize = os.path.getsize(currentFilePath)
                        currentFile = open(currentFilePath,'rb')
                        
                        # print file name to the log window
                        parts = currentFilePath.split('/')
                        fileName = parts[-1]
                        self.handleUi.emit(['LOG_APPEND', f'Uploading {fileName}...', ''])
                        
                        packetType = 0
                        if upgradeType == 'FIRMWARE':         # packet types 1 and 2 for init and data respectively
                            packetType = '1'
                        elif upgradeType == 'CERTIFICATE':    # packet types 3 and 4 for init and data respectively
                            packetType = '3'
                            
                        certType = str(idx+1)   # in case of FIRMWARE, it will be don't care
                        
                        # generate the init packet according to packet type
                        initFrame = ['55AA',packetType,'2','0','5.2.4','1', certType,'0', totalFileSize ,'0','1024','FF']
                        initPacket = ','.join([str(elem) for elem in initFrame])
                        print('tx -> ', initPacket)
                        
                        if commInterface == 'SERIAL':
                            ser.write(initPacket.encode())
                        if commInterface == 'TCP':
                            conn.sendall(initPacket.encode())
                        
                        currentOffset = 0
                        cachedOffset  = 0
                        remainingBytes = totalFileSize
        
                        while remainingBytes > 0:
                            
                            if commInterface == 'SERIAL':
                                rxData = str(ser.readline(), 'utf-8', 'ignore')
                            if commInterface == 'TCP':
                                rxData = str(conn.recv(2048), 'utf-8')
                            
                            print('rx <- ', rxData)
                            
                            if re.search(r'55AA,', rxData):
                                if re.search(r'55AA,1', rxData):  # firmware init packet received again over TCP (due to connectivity loss etc), send handshake packet
                                    print('tx -> ', initPacket)
                                    conn.sendall(initPacket.encode())
                                    time.sleep(2)
                                    continue
                                else:
                                    pass
                            else:
                                continue
        
                            req = rxData.split(',')
                            currentOffset = int(req[2])
                            chunkSize = int(req[3])
                            
                            # proceed with file data
                            currentFile.seek(currentOffset)
                            binData = currentFile.read(chunkSize)
                            binDataLen = len(binData)
                            crc = self.crc16(0, binData, binDataLen)
                            blockCrc = bytearray([0, 0])
                            blockCrc[0] = ((crc >> 8) & 0x00FF)
                            blockCrc[1] = (crc & 0x00FF)
                            
                            if upgradeType == 'FIRMWARE':         # packet types 1 and 2 for init and data respectively
                                packetType = '2'
                            elif upgradeType == 'CERTIFICATE':    # packet types 3 and 4 for init and data respectively
                                packetType = '4'
                            
                            dataPacket = bytearray(bytes('55AA,', 'utf-8') + bytes(str(packetType) + ',', 'utf-8') + bytes(str(currentOffset) + ',', 'utf-8') + bytes(str(binDataLen) + ',', 'utf-8') + binData + bytes(',', 'utf-8') + blockCrc + bytes(',FF', 'utf-8'))
                            
                            if commInterface == 'SERIAL':
                                ser.write(dataPacket)
                            if commInterface == 'TCP':
                                time.sleep(0.05)    # keep a bit server side delay
                                conn.sendall(dataPacket)
                                
                            if( (currentOffset != cachedOffset) or currentOffset == 0):
                                cachedOffset = currentOffset
                                remainingBytes = remainingBytes - binDataLen
                                percentComplete = (int(totalFileSize - remainingBytes)/totalFileSize)*100   # typecasted to int
                                self.handleUi.emit(['SET_PROGRESS', round(percentComplete), ''])
                                # self.handleUi.emit(['LOG_APPEND', f'{percentComplete:.1f}% completed', '']) # TODO eshwar check if required
                    currentFile.close()
                    self.handleUi.emit(['SET_PROGRESS', 100, ''])    #TODO: eshwar check if required
                    self.handleUi.emit(['END_SESSION', '', ''])
                    self.fsmState = 'UART_TRANSPARENT_MODE'
                    
                    # Calculate the execution time in seconds
                    self.endTime = time.time()
                    executionTime = self.endTime - self.startTime
                    
                    self.handleUi.emit(['SHOW_POPUP', 'Update completed', f'Serial update completed successfully in {executionTime:.1f} sec'])
                except Exception as e:
                    self.handleUi.emit(['SHOW_POPUP', 'def work:', f'WRITE_FILES:\n{e}'])
                    return
                
            ''' Uses non-standard old protocol (Begin File upload task) and no documentation available. Only applicable for 'SERIAL' interface for FIRMWARE only.
            PREVALIDATE using serial commands will be done before hand
            FIRMWARE PREVALIDATE:    Internal battery must be above 3.8V. get io status command *GET#IOST# '''
            if self.fsmState == 'WRITE_FILES_LEGACY':
                try:
                    if(ser.isOpen()):
                        self.handleUi.emit(['DISABLE_BUTTONS', '', ''])
                        parts = fileList[0].split('/')
                        fwFileName = parts[-1]
                        self.handleUi.emit(['LOG_APPEND', f'Starting serial firmware upload of file {fwFileName}...', ''])
                        binFile = None         # file object
                        binFileSize = 0
                        currentOffset = 0
                        remainingBytes = 0
                        binDataPacket = 0
                        
                        while self.fsmState == 'WRITE_FILES_LEGACY':
                            rxData = str(ser.readline(), 'utf-8', 'ignore')
                            print('rx <- ',rxData)
                        
                            if binFileSize == 0:
                                binFileSize = os.path.getsize(fileList[0])
                                self.handleUi.emit(['LOG_APPEND', f'bin file {binFileSize} bytes\n', ''])
                                print('binFileSize ',binFileSize)
                                remainingBytes = binFileSize
                        
                                ser.flushInput()
                                print('tx -> Begin File upload task')
                                ser.write(bytes(b'Begin File upload task'))
                                self.startTime = time.time()
                        
                            elif re.search(r'Please upload File', rxData):
                                print('SOF match with Please upload')
                        
                                binFile = open(fileList[0],'rb')
                                binFile.seek(currentOffset)
                                binData = binFile.read(1024)        # read 1024 data bytes
                                binDataLen = len(binData)
                        
                                crc = self.crc16(0, binData, binDataLen)
                                blockCrc = bytearray([0, 0])
                                blockCrc[0] = ((crc >> 8) & 0x00FF)
                                blockCrc[1] = (crc & 0x00FF)
                        
                                binDataPacket = bytearray(bytes('StartP_WRFWFile_', 'utf-8') + bytes(str('3:/dir_fota/AEPL_V5.bin'), 'utf-8') + bytes(',', 'utf-8') + blockCrc + binData)
                        
                                ser.flushInput()
                                ser.write(binDataPacket)
                        
                                currentOffset = currentOffset + 1024
                                remainingBytes = remainingBytes - binDataLen
                        
                            elif re.search(r'NEXT CHUNK', rxData):
                                print('SOF match with NEXT_CHUNK, remaining bytes ', remainingBytes)
                                if remainingBytes > 0:
                                    binFile.seek(currentOffset)
                                    binData = binFile.read(1024)        # read 1024 data bytes
                                    binDataLen = len(binData)
                        
                                    crc = self.crc16(0, binData, binDataLen)
                                    blockCrc = bytearray([0, 0])
                                    blockCrc[0] = ((crc >> 8) & 0x00FF)
                                    blockCrc[1] = (crc & 0x00FF)
                        
                                    binDataPacket = bytearray(bytes('MidP_WRFWFile_', 'utf-8') + bytes(str('3:/dir_fota/AEPL_V5.bin'), 'utf-8') + bytes(',', 'utf-8') + blockCrc + binData)
                        
                                    ser.flushInput()
                                    ser.write(binDataPacket)
                                    # print('tx -> ',binDataPacket)    # avoid print flooding
                        
                                    currentOffset = currentOffset + 1024
                                    remainingBytes = remainingBytes - binDataLen
                                    percentComplete = int(((currentOffset)/binFileSize)*100)
                                    if percentComplete > 0:
                                        self.handleUi.emit(['SET_PROGRESS', round(percentComplete), ''])
                        
                            elif re.search(r'NACK', rxData):
                                print('SOF match with NACK, remaining bytes ', remainingBytes)
                                ser.flushInput()
                                ser.write(binDataPacket)
                        
                            # after binary data is completed, 'CRC_MATCH' is received. We also ensure remainingBytes to be 0
                            elif re.search(r'CRC_MATCH', rxData) and remainingBytes == 0:
                                print('SOF match with CRC_MATCH, remaining bytes ', remainingBytes)
                                remainingBytes = -1
                                binFile.close()
                                bootloaderPkt = bytearray(bytes('SETBLFLAG FUEOFGIVE_SIZE_3:/dir_fota/AEPL_V5.bin$IMAGE FILE UPLOADED', 'utf-8'))
                                print('tx -> ',bootloaderPkt)
                                ser.flushInput()
                                ser.write(bootloaderPkt)
                        
                            elif re.search(r'file_size', rxData):
                                print('SOF match with file_size, remaining bytes ', remainingBytes)
                        
                                # Regular expression pattern to match one or more digits
                                matches = re.findall(r'\d+', rxData)
                                if matches:
                                    receivedBinSize = int(matches[0])
                        
                                print('expected file size = ', binFileSize, ', actual file size = ', receivedBinSize)
                        
                                if receivedBinSize != binFileSize:
                                    print('***** expected file size = ', binFileSize, ', actual file size = ', receivedBinSize, ' aborting...')
                                    self.handleUi.emit(['SHOW_POPUP', 'File size error', f'expected {binFileSize}, actual {receivedBinSize}\nplease try again'])
                                    return
                        
                                # we provide fixed CRC of '7ed3' (LSB first) of fota.txt contents starting from '@' to the end of text file
                                fotaTxtPkt = bytearray(bytes('StartP_WRFWFile_3:/dir_fota/FOTA.txt,', 'utf-8') + bytearray([0xd3, 0x7e]) + bytes('@,0,4,0,0,0,0,0,0,\r\n@,1,4,0,1,0,0,0,0,\r\n@,2,4,0,2,4,0,0,0,\r\n@,3,4,0,3,5,0,0,0.', 'utf-8'))
                        
                                time.sleep(0.1)
                        
                                print('tx -> ',fotaTxtPkt)
                                ser.flushInput()
                                ser.write(fotaTxtPkt)
                        
                                time.sleep(0.1)
                        
                                self.endTime = time.time()
                        
                                # Calculate the execution time in minutes
                                executionTime = (self.endTime - self.startTime)/60
                        
                                print('tx -> *SET#CRST#1#')
                                self.handleUi.emit(['END_SESSION', '', ''])
                                self.fsmState = 'UART_TRANSPARENT_MODE'
                                
                                self.handleUi.emit(['SHOW_POPUP', 'Update completed', f'Serial update completed successfully in {executionTime:.1f} mins'])
                            else:
                                print('***** no matching SOF pattern *****')
                        
                except Exception as e:
                    self.handleUi.emit(['SHOW_POPUP', 'def work:', f'WRITE_FILES_LEGACY\n{e}'])
                    return
                
        self.finished.emit()
    
    def crc16(self, InputCRC, data, len):
        crc = InputCRC
        for a in range(len):
            crc ^= (data[a] << 8)
            for i  in range(8):
                if ((crc & 0x8000) != 0):
                    crc = ((crc << 1) ^ 0x1021)
                else:
                    crc = (crc << 1)
        exabyte = hex(crc)
        return crc & 0xffff
        
class ServiceToolUi(QtWidgets.QMainWindow):
    def __init__(self):
        super(ServiceToolUi, self).__init__()
        uic.loadUi(self.getResourcePath('service_tool.ui'), self)
        self.setWindowIcon(QIcon(self.getResourcePath('resources/thumbnail.ico')))
        
        self.setStyleSheet('QMainWindow#MainWindow { background-image:url(:/resources/background.png) }')
    
        self.labelVersion = self.findChild(QtWidgets.QLabel,'labelVersion')
        self.labelBuild = self.findChild(QtWidgets.QLabel,'labelBuild')
        self.comboBoxComPort = self.findChild(QtWidgets.QComboBox,'comboBoxComPort')
        self.comboBoxProject = self.findChild(QtWidgets.QComboBox,'comboBoxProject')
        self.btnConnect = self.findChild(QtWidgets.QPushButton,'btnConnect')
        self.btnWrite = self.findChild(QtWidgets.QPushButton,'btnWrite')
        self.log = self.findChild(QtWidgets.QTextBrowser,'textBrowserLog')
        self.btnFwMode = self.findChild(QtWidgets.QRadioButton,'btnFwDownload')
        self.btnCertMode = self.findChild(QtWidgets.QRadioButton,'btnCertDownload')
        self.progressBar = self.findChild(QtWidgets.QProgressBar,'progressBar')
    
        self.comboBoxProject.currentTextChanged.connect(self.selectProject)
        self.btnConnect.clicked.connect(self.connectInterface)
        self.btnFwMode.clicked.connect(self.browseFiles)
        self.btnCertMode.clicked.connect(self.browseFiles)
        self.btnWrite.clicked.connect(self.writeFiles)

        # Setting Radio Buttons to be independent
        self.btnFwMode.setAutoExclusive(False)
        self.btnCertMode.setAutoExclusive(False)
    
        # populate the comboboxes
        self.btnConnect.setIcon(QIcon(':/resources/disconnected.png'))
        comPortList = []   # TODO eshwar enable TCP support after verified working. reserve TCP port string here, it will be followed by available serial ports
        # comPortList = ['TCP Port']   # reserve TCP port string here, it will be followed by available serial ports
        availablePorts = list(ports.comports())
        for i in availablePorts:
            comPortList.append(i.device + ': ' + i.description) # COM21: Prolific USB-to-Serial Comm Port (COM21)
        self.comboBoxComPort.addItems(comPortList)
        self.comboBoxProject.addItems(map(lambda x: x[0], projects_list))
        
        # MUTLI-THREADING
        self.worker = Worker()                                  # a new worker to perform those tasks
        self.thread = QThread()                                 # a new thread to run our background tasks in
        self.worker.moveToThread(self.thread)                   # move the worker into the thread, do this first before connecting the signals
        self.worker.finished.connect(self.worker.deleteLater)   # have worker mark itself for deletion
        self.worker.handleUi.connect(self.handleUiFromThread)   # for modifying UI class elements after a resultant operation from thread
    
        self.thread.started.connect(self.worker.work)           # begin our worker object's loop when the thread starts running
        self.thread.finished.connect(self.thread.deleteLater)   # have thread mark itself for deletion
        self.thread.start()
        
        self.handleUiFromThread(['SHOW_POPUP', 'Disclaimer', 'Accolade Service tool usage is restricted to Accolade service engineers and representatives only\n'])
        self.labelVersion.setText('SERVICE TOOL v1.4.0')
        self.labelVersion.setText('SERVICE TOOL v{}'.format(VERSION))
        self.labelBuild.setText('Stable Build 12 Feb 2024')
        
        self.setFixedSize(600,400)
        self.show() # show the gui
    
    def writeFiles(self):
        self.btnWrite.setEnabled(False)
        self.worker.fsmState = 'IDLE'
        if commInterface == 'TCP':   # for TCP, PREVALIDATION cannot be done
            self.btnWrite.setEnabled(False)
            self.btnFwMode.setEnabled(False)
            self.btnCertMode.setEnabled(False)
            self.worker.fsmState = 'WRITE_FILES'
            return
        # for serial port prevalidation must be done over SERIAL commands for battery and other checks
        if commInterface == 'SERIAL':
            self.worker.fsmState = 'PREVALIDATE'
    
    def browseFiles(self):
        global fileList, upgradeType
        fileList = ['', '', '']     # cert1/bin, cert2, cert3 are reserved respectively
        options = QFileDialog.Options()
        if self.btnFwMode.isChecked() or commInterface == 'TCP' :
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
                if upgradeType == 'FIRMWARE' or commInterface == 'TCP' :
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
                                self.handleUiFromThread(['SHOW_POPUP', 'No suitable file found', 'The selected file does not contain a .bin file'])
                                self.btnFwMode.setChecked(False)
                                return
                    
                    embeddedVersion = self.getVersionFromBin(fileList[0])
                    
                    if len(embeddedVersion) < 1:
                        self.handleUiFromThread(['SHOW_POPUP', 'Prevalidation failed', 'The selected file does not contain a valid version.\nAborting...'])
                        return
                    
                    # ensure firmware support first, must match the major atleast
                    embeddedVersion = Version(embeddedVersion)    # force to 'version' object type
                    if embeddedVersion.major != requiredVersion.major: 
                        self.handleUiFromThread(['SHOW_POPUP', 'Operation not allowed', f'{self.comboBoxProject.currentText()} device must only be flashed with {requiredVersion.major}.x.x\nPlease select correct firmware file\nAborting...'])
                        self.btnFwMode.setChecked(False)
                        return
                    
                    self.btnWrite.setEnabled(True)
        
                if upgradeType == 'CERTIFICATE': # must select 3 cert files
                    if fileList.__len__() != 3:
                        self.handleUiFromThread(['SHOW_POPUP', 'Count unsatisfied', 'Please select 3 certificates'])
                        return
                    
                    # must have exact names for recognition
                    fileList.sort()
                    if (fileList[0].find('ca.pem') != -1) and (fileList[1].find('cc.pem') != -1) and (fileList[2].find('ck.pem') != -1):
                        pass
                    else:
                        self.handleUiFromThread(['SHOW_POPUP', 'File', 'Please rename files as per certificate types \nca.pem \ncc.pem \nck.pem'])
                        return
                    self.btnWrite.setEnabled(True)
        except:
            pass

    def getVersionFromBin(self, filepath)->str:
        # similar to grep -a <input_file> "FIRMWARE_VER"
        pattern = re.compile(br'\b\w+_FIRMWARE_VER (?P<version>\d+\.\d+\.\d+)') 
        with open(filepath, 'rb') as binary_file:
            for line in binary_file:
                match = pattern.search(line)
                if match:
                    version = match.group('version').decode('utf-8')
                    return str(version)
        return ''

    def connectInterface(self):
        global commInterface
        selectedPort = self.comboBoxComPort.currentText()    # full port name COM21: Prolific USB-to-Serial Comm Port (COM21)
        if self.worker.tcpPort != 0:
            self.handleUiFromThread(['SHOW_POPUP', 'Port busy', 'Port already in use'])
            return
        if selectedPort == 'TCP Port':
            tcpPort, ok = QInputDialog.getText(self, 'Specify port number', 'Enter the TCP port number to mapped to this system')
            try:
                tcpPort = int(tcpPort)
                # check if the 'tcpPort' intance is an int type
                if ok and isinstance(tcpPort, int):
                    self.worker.tcpPort = tcpPort
                    commInterface = 'TCP'
                    self.comboBoxProject.setEnabled(True)
                    self.comboBoxComPort.setEnabled(False)
                    self.btnConnect.setIcon(QIcon(':/resources/connected.png'))
                    return
            except Exception as e:
                print('def connectInterface exception: ', e)
                self.handleUiFromThread(['SHOW_POPUP', 'Invalid input', 'Please enter a valid port number'])
                return
        
        portName = selectedPort.split(':')      # splits the 'COM21', 'Prolific ...'
        try:
            global ser
            ser = serial.Serial(portName[0], 115200, timeout = 0.1)
            commInterface = 'SERIAL'
            self.comboBoxProject.setEnabled(True)
            self.comboBoxComPort.setEnabled(False)
            self.btnConnect.setIcon(QIcon(':/resources/connected.png'))
            self.worker.fsmState = 'UART_TRANSPARENT_MODE'
            
        except:
            self.handleUiFromThread(['SHOW_POPUP', 'COM Port Error', 'Access denied'])

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
    
    def selectProject(self):
        global currentProject, requiredVersion
        currentProject = ''
        requiredVersion = Version('0.0.0')
        for item in projects_list:
            if item[0] == self.comboBoxProject.currentText():
                currentProject = item[0]
                requiredVersion = item[1]

        self.comboBoxProject.setEnabled(False)
        self.btnFwMode.setEnabled(True)
        if commInterface == 'TCP':   # allow certificate flashing only on serial interface
            pass
        else:
            self.btnCertMode.setEnabled(True)
        
        if currentProject == 'Wifi Data Logger':     # certificate flashing not allowed for wifi data logger
            self.btnCertMode.setEnabled(False)
        
    def handleUiFromThread(self, arg):
        if arg[0] == 'SHOW_POPUP':
            msgBox = QMessageBox()
            msgBox.setWindowTitle(arg[1])
            msgBox.setIcon(QMessageBox.Information)
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
        if arg[0] == 'RE-ENABLE_BUTTONS':
            self.btnFwMode.setEnabled(True)
            self.btnCertMode.setEnabled(True)
            if commInterface == 'SERIAL':   # allow certificate flashing only on serial interface
                self.btnConnect.setEnabled(True)
        if arg[0] == 'END_SESSION':
            ser.write(bytes(b'*SET#CRST#1#'))
            self.btnFwMode.setChecked(False)
            self.btnCertMode.setChecked(False)
            fileList.clear()
            self.btnWrite.setEnabled(False)
        if arg[0] == 'PREVALIDATION_ERR':
            self.btnFwMode.setChecked(False)
            self.btnCertMode.setChecked(False)

app = QtWidgets.QApplication(sys.argv)
window = ServiceToolUi()

app.exec_()
