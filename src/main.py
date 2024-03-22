#!/usr/bin/env python3
'''\

Copyright 2024-2025 Accolade Electronics Pvt. Ltd

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

file        main.py
brief       This is the main script file for launching the service tool UI

date        22 March 2024
author      Eshwar J <eshwar.jorvekar@accoladeelectronics.com>

tested on python 3.11.8 on windows 11 x64

# dependencies
pip3 install tk

'''

import threading        # for threading
import time             # for sleep
import ui               # for GUI constructs

# this thread is responsible for UDS communication
class FsmThread(threading.Thread):
    def __init__(self, stop_event):
        super(FsmThread, self).__init__()
        self._stop_event = stop_event
        self._lock = threading.Lock()
        self._state = 'idle'

    def run(self):
        while not self._stop_event.is_set():
            with self._lock:
                if self._state == 'run':
                    print("thread: running")
                elif self._state == 'idle':
                    print("thread: idle")
            time.sleep(0.5)
        print("thread: exit")

    def start_thread(self):
        with self._lock:
            self._state = 'run'

    def idle_thread(self):
        with self._lock:
            self._state = 'idle'

    def stop_thread(self):
        self._stop_event.set()
        self.join()

# Create a stop event to signal the thread to stop
stop_event = threading.Event()

# Create an instance of the thread
fsm_thread = FsmThread(stop_event)

def start_thread():
    fsm_thread.start_thread()

def idle_thread():
    fsm_thread.idle_thread()

def stop_thread():
    fsm_thread.stop_thread()

# Function to handle application exit
def on_close():
    stop_thread()
    root.destroy()

root = ui.create_gui()

def main():
    # Start the thread
    fsm_thread.start()

    # Create GUI buttons and assign functions to call
    ui.create_buttons(root, start_thread, idle_thread)

    # Bind the closing event
    ui.bind_close_event(root, on_close)

    # Run the GUI
    ui.run_gui(root)

if __name__ == "__main__":
    main()
