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

import threading                    # for threading
import time                         # for sleep
import app_ui                       # for using the GUI constructs to interact with FSM in thread
import PCAN_UDS_2013 as udsModule   # import uds library
from tkinter import filedialog

# import the dlls required for stack

import os
import sys
from pathlib import Path

dll_path = Path(__file__).resolve().parent / "uds_stack"
sys.path.insert(0, str(dll_path))
os.add_dll_directory(str(dll_path))

# create uds library object
objPCANUds = udsModule.PCAN_UDS_2013()

class FsmThread(threading.Thread):
    def __init__(self, stop_event):
        super(FsmThread, self).__init__()
        self._stop_event = stop_event
        self._lock = threading.Lock()
        self._state = 'IDLE'

    def run(self):
        while not self._stop_event.is_set():
            with self._lock:
                if self._state == 'IDLE':
                    pass
                elif self._state == 'BOOT_LOCK':
                    pass
                elif self._state == 'FILE_UPLOAD':
                    pass
            time.sleep(0.1)
        print("thread: exit")

    def fsm_transition_boot_lock(self):
        app_ui.append_log(log_window, 'trying to lock into bootloader...')
        with self._lock:
            self._state = 'BOOT_LOCK'
    
    def fsm_transition_upload_file(self):
        app_ui.append_log(log_window, 'initiating file upload...')
        with self._lock:
            self._state = 'FILE_UPLOAD'

    def stop_thread(self):
        self._stop_event.set()
        self.join()

# Create a stop event to signal the thread to stop
stop_event = threading.Event()

# Create an instance of the thread
fsm_thread = FsmThread(stop_event)

def browse_file():
    file_path = filedialog.askopenfilename()
    if file_path:
        path =  "\r\nselected file " + file_path
        app_ui.append_log(log_window, path)

def boot_lock():
    fsm_thread.fsm_transition_boot_lock()

def upload_file():
    fsm_thread.fsm_transition_upload_file()

def stop_thread():
    fsm_thread.stop_thread()

# Function to handle application exit
def on_close():
    stop_thread()
    root.destroy()
    print('thread: destroyed')

root = app_ui.create_gui()

def start():
    # Start the background thread for finite state machine
    fsm_thread.start()

    # Create GUI buttons and assign functions to call on user 'click'
    app_ui.create_buttons(root, browse_file, boot_lock, upload_file)
    
    global log_window
    log_window = app_ui.create_log_window(root)

    app_ui.append_log(log_window, "Tool version 0.1.0, Build date 26 Mar 2024")
    app_ui.append_log(log_window, "Software usage is restricted to Accolade service engineers or representatives only")

    # Bind the GUI closing event for destroying thread
    app_ui.bind_close_event(root, on_close)

    # Run the GUI
    app_ui.run_gui(root)
