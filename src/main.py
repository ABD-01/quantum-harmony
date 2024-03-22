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
import tkinter as tk    # GUI library

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

def main():
    # Start the thread
    fsm_thread.start()

    # Create a tkinter GUI
    root = tk.Tk()
    root.title("Accolade Service Tool")

# GUI constructs
    # Set the size of the GUI window
    root.geometry("600x400")

    # Start button
    start_button = tk.Button(root, text="Start", command=start_thread)
    start_button.pack()

    # Idle button
    idle_button = tk.Button(root, text="Idle", command=idle_thread)
    idle_button.pack()

    # Function to handle application exit
    def on_close():
        stop_thread()
        root.destroy()

    # Bind the closing event to the function
    root.protocol("WM_DELETE_WINDOW", on_close)

    # Start the tkinter event loop
    root.mainloop()

if __name__ == "__main__":
    main()
