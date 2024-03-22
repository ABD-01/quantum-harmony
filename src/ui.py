'''\

Copyright 2024-2025 Accolade Electronics Pvt. Ltd

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

file        ui.py
brief       This is the source file for the GUI constructs

date        22 March 2024
author      Eshwar J <eshwar.jorvekar@accoladeelectronics.com>

'''

import tkinter as tk

def create_gui():
    # Create a tkinter GUI
    root = tk.Tk()
    root.title("Accolade Service Tool")
    root.geometry("600x400")

    return root

def create_buttons(root, start_thread, idle_thread):
    start_button = tk.Button(root, text="Start", command=start_thread)
    start_button.pack()

    idle_button = tk.Button(root, text="Idle", command=idle_thread)
    idle_button.pack()

def bind_close_event(root, on_close):
    # Bind the closing event to the function
    root.protocol("WM_DELETE_WINDOW", on_close)
    
def run_gui(root):
    # Start the tkinter event loop
    root.mainloop()