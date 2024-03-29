'''\
Copyright 2024-2025 Accolade Electronics Pvt. Ltd

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

file        app_ui.py
brief       This is the source file for the GUI constructs

date        22 March 2024
author      Eshwar J <eshwar.jorvekar@accoladeelectronics.com>
'''

import tkinter as tk

def create_gui():
    # Create a tkinter GUI
    root = tk.Tk()
    root.title("uCommander")
    root.geometry("600x400")
    icon_path = getResourcePath(r"aepl.ico")

    # Set the window title icon
    root.iconbitmap(icon_path)

    # todo: set the taskbar icon from pyinstaller

    # Make the window non-resizable
    root.resizable(False, False)

    # Load the background image
    background_image = tk.PhotoImage(file=getResourcePath(r"background.gif"))

    # Create a label to display the background image
    background_label = tk.Label(root, image=background_image)
    background_label.place(x=0, y=0, relwidth=1, relheight=1)

    # To prevent garbage collection of the image object
    background_label.image = background_image

    version_label = tk.Label(root, text="v0.1.0 (built 29 Mar 2024)", font=("Fira Sans", 8), bg="white")
    version_label.place(x=455, y=380)

    print('app_ui    : created ui root')

    return root

def create_buttons(root, browse_file, boot_lock, upload_file, reset_ecu):
    button_font = ("Fira Sans", 12)
    
    global g_browse_btn
    global g_boot_lock_btn
    global g_upload_btn
    global g_reset_ecu_btn

    g_browse_btn = tk.Button(root, text="Browse", font=button_font, bg="#04508e", fg="white", activebackground="yellow", command=browse_file)
    g_browse_btn.place(x=20, y=100)

    g_boot_lock_btn = tk.Button(root, text="Boot lock", font=button_font, bg="#04508e", fg="white", activebackground="yellow", command=boot_lock)
    g_boot_lock_btn.place(x=100, y=100)

    g_upload_btn = tk.Button(root, text="Upload", font=button_font, bg="#04508e", fg="white", activebackground="yellow", command=upload_file)
    g_upload_btn.place(x=195, y=100)

    g_reset_ecu_btn = tk.Button(root, text="Reset ECU", font=button_font, bg="#04508e", fg="white", activebackground="yellow", command=reset_ecu)
    g_reset_ecu_btn.place(x=275, y=100)

    set_btn_enabled('BROWSE_BTN', True)
    set_btn_enabled('BOOT_LOCK_BTN', False)
    set_btn_enabled('UPLOAD_BTN', False)
    set_btn_enabled('RESET_ECU_BTN', False)

    print('app_ui    : created buttons')

def set_btn_enabled(btn, state):
    if(state == True):
        state = tk.NORMAL
    else:
        state = tk.DISABLED
        
    if(btn == 'BROWSE_BTN'):
        g_browse_btn.config(state=state)
    if(btn == 'BOOT_LOCK_BTN'):
        g_boot_lock_btn.config(state=state)
    if(btn == 'UPLOAD_BTN'):
        g_upload_btn.config(state=state)
    if(btn == 'RESET_ECU_BTN'):
        g_reset_ecu_btn.config(state=state)

def create_log_window(root):
    log_font = ("Consolas", 8)

    # Create a frame for the log window
    log_frame = tk.Frame(root, bg="#d9e6ee")
    log_frame.place(x=20, y=280, width=560, height=100)  # Place at the bottom of the UI

    # Create a Text widget to display log messages
    global g_log_window
    g_log_window = tk.Text(log_frame, wrap="word", font=log_font, state="disabled", bg="#d9e6ee", fg="black")
    g_log_window.pack(side="left", fill="both", expand=True)

    # Create a Scrollbar for the Text widget
    scrollbar = tk.Scrollbar(log_frame, orient="vertical", command=g_log_window.yview)
    scrollbar.pack(side="right", fill="y")

    # Configure the Text widget to use the Scrollbar
    g_log_window.configure(yscrollcommand=scrollbar.set)

    print('app_ui    : created log window')

    return g_log_window

def append_log( message):
    # Enable editing
    g_log_window.configure(state="normal")
    
    # Insert message
    g_log_window.insert("end", message + "\n")
    
    # Disable editing
    g_log_window.configure(state="disabled")
    
    # Scroll to the end
    g_log_window.see("end")

def bind_close_event(root, on_close):
    # Bind the closing event to the function
    root.protocol("WM_DELETE_WINDOW", on_close)

    print('app_ui    : binded on_close event')
    
def run_gui(root):
    print('app_ui    : starting ui thread')

    # Start the tkinter event loop
    root.mainloop()

########################################################################## (TMP)
    
# def getResourcePath(self, relative_path):
#     if getattr(sys, 'frozen', False):
#         return os.path.join(sys._MEIPASS, relative_path)

#     return os.path.join(os.path.abspath('.'), relative_path)

import os
import sys

def getResourcePath(relative_path):
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