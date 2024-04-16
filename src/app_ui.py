'''
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
author      Accolade Electronics <www.accoladeelectronics.com>
'''

import tkinter as tk           # for core tk
from tkinter import ttk        # for progressbar

########################################### (GUI creation) ####################################################

def create_gui():
    global root
    # Create a tkinter GUI
    root = tk.Tk()
    root.title('Accolade Service Tool')
    root.geometry('600x400')
    icon_path = get_resource_path(r'resources/aepl.ico')

    # Set the window title icon
    root.iconbitmap(icon_path)

    # todo: set the taskbar icon from pyinstaller

    # Make the window non-resizable
    root.resizable(False, False)

    # Load the background image
    background_image = tk.PhotoImage(file=get_resource_path(r'resources/background.gif'))

    # Create a label to display the background image
    background_label = tk.Label(root, image=background_image)
    background_label.place(x=0, y=0, relwidth=1, relheight=1)

    # To prevent garbage collection of the image object
    background_label.image = background_image

    print('app_ui    : created ui root')

    return root

def on_combobox_selected(event):
    print("app_ui    : selected", g_combobox.get())

def create_labels(root):
    sw_label = tk.Label(root, text='uCommander v0.2', font=('White Rabbit', 24), bg='white')
    sw_label.place(x=200, y=30)

    version_label = tk.Label(root, text='build 15 Apr 2024', font=('White Rabbit', 9), bg='white')
    version_label.place(x=350, y=60)

    label_font = ('Fira Sans', 10)
    label = tk.Label(root, text="Bit rate", font=label_font, bg='white')
    label.place(x=50, y=120)

    label = tk.Label(root, text="Tester id   0x", font=label_font, bg='white')
    label.place(x=50, y=150)

    label = tk.Label(root, text="ECU id        0x", font=label_font, bg='white')
    label.place(x=50, y=180)

    print('app_ui    : created labels')

def create_input_labels(root):
    global g_text_input_tester_id
    global g_text_input_ecu_id

    g_text_input_tester_id = tk.Entry(root, width=10)
    g_text_input_tester_id.place(x=130, y=150)

    g_text_input_ecu_id = tk.Entry(root, width=10)
    g_text_input_ecu_id.place(x=130, y=180)

    print('app_ui    : created input labels')

def create_combobox(root):
    global g_combobox
    options = ["500 kBit/s", "1 MBit/s"]
    g_combobox = ttk.Combobox(root, values=options, width=10, state='readonly')
    g_combobox.place(x=130, y=120)
    g_combobox.set(options[0])  # Set default value

    g_combobox.bind("<<ComboboxSelected>>", on_combobox_selected)

    print('app_ui    : created combobox')

def open_new_window():
    global g_ui_debug_window
    g_ui_debug_window = tk.Toplevel(root)
    g_ui_debug_window.title("uCommander debug")
    g_ui_debug_window.geometry("600x200")
    g_ui_debug_window.resizable(False, False)
    create_log_window(g_ui_debug_window)
    bind_close_event(g_ui_debug_window, on_debug_window_close)

def on_debug_window_close():
    g_debug_checkbox_ticked.set(False)
    toggle_debug_window()

def toggle_debug_window():
    if g_debug_checkbox_ticked.get() == True:
        open_new_window()
    else:
        if g_ui_debug_window:
            g_ui_debug_window.destroy()
            print('app_ui    : destroyed debug window ui root')

def create_checkbox(root):
    global checkbox
    global g_debug_checkbox_ticked

    g_debug_checkbox_ticked = tk.BooleanVar()
    checkbox = tk.Checkbutton(root, text="enable debug", font=('Fira Sans', 8), bg='white',  variable=g_debug_checkbox_ticked, command=toggle_debug_window)
    checkbox.place(x=30, y=380)

def create_buttons(root, connect_can, boot_lock, reset_ecu, browse_file, upload_file):
    button_font = ('Fira Sans', 12)

    global g_btn_connect
    global g_btn_boot_lock
    global g_btn_reset_ecu
    global g_btn_browse
    global g_btn_upload

    g_btn_connect = tk.Button(root, text='Connect', font=button_font, bg='#04508e', fg='white', activebackground='yellow', command=connect_can)
    g_btn_connect.place(x=50, y=215)

    g_btn_boot_lock = tk.Button(root, text='Boot lock', font=button_font, bg='#04508e', fg='white', activebackground='yellow', command=boot_lock)
    g_btn_boot_lock.place(x=315, y=215)

    g_btn_reset_ecu = tk.Button(root, text='Reset ECU', font=button_font, bg='#04508e', fg='white', activebackground='yellow', command=reset_ecu)
    g_btn_reset_ecu.place(x=410, y=215)

    g_btn_browse = tk.Button(root, text='Browse', font=button_font, bg='#04508e', fg='white', activebackground='yellow', command=browse_file)
    g_btn_browse.place(x=50, y=300)

    g_btn_upload = tk.Button(root, text='Upload', font=button_font, bg='#04508e', fg='white', activebackground='yellow', command=upload_file)
    g_btn_upload.place(x=130, y=300)

    set_btn_enabled('BOOT_LOCK_BTN', False)
    set_btn_enabled('RESET_ECU_BTN', False)
    set_btn_enabled('BROWSE_BTN', False)
    set_btn_enabled('UPLOAD_BTN', False)

    print('app_ui    : created buttons')

def set_btn_enabled(btn, state):
    if(state == True):
        state = tk.NORMAL
    else:
        state = tk.DISABLED

    if(btn == 'CONNECT_BTN'):
        g_btn_connect.config(state=state)
    if(btn == 'BOOT_LOCK_BTN'):
        g_btn_boot_lock.config(state=state)
    if(btn == 'RESET_ECU_BTN'):
        g_btn_reset_ecu.config(state=state)
    if(btn == 'BROWSE_BTN'):
        g_btn_browse.config(state=state)
    if(btn == 'UPLOAD_BTN'):
        g_btn_upload.config(state=state)

def create_progress_bar(root):
    global g_progress_bar
    g_progress_bar = ttk.Progressbar(root, orient='horizontal', length=500, mode='determinate')
    g_progress_bar.place(x=50, y=345)

    print('app_ui    : created progressbar')

def create_log_window(root):
    log_font = ('Consolas', 8)

    # Create a frame for the log window
    log_frame = tk.Frame(root, bg='#d9e6ee')
    log_frame.place(x=0, y=0, width=600, height=200)  # Place at the bottom of the UI

    # Create a Text widget to display log messages
    global g_log_window
    g_log_window = tk.Text(log_frame, wrap='word', font=log_font, state='disabled', bg='black', fg='white')
    g_log_window.pack(side='left', fill='both', expand=True)

    # Create a Scrollbar for the Text widget
    scrollbar = tk.Scrollbar(log_frame, orient='vertical', command=g_log_window.yview)
    scrollbar.pack(side='right', fill='y')

    # Configure the Text widget to use the Scrollbar
    g_log_window.configure(yscrollcommand=scrollbar.set)

    print('app_ui    : created log window')

    return g_log_window

def append_log( message):
    if g_debug_checkbox_ticked.get():
        # Enable editing
        g_log_window.configure(state='normal')
        
        # Insert message
        g_log_window.insert('end', message + '\n')
        
        # Disable editing
        g_log_window.configure(state='disabled')
        
        # Scroll to the end
        g_log_window.see('end')
    else:
        pass

def bind_close_event(root, on_close):
    # Bind the closing event to the provided function
    root.protocol('WM_DELETE_WINDOW', on_close)

    print('app_ui    : binded on_close event')
    
def run_gui(root):
    print('app_ui    : starting ui thread')

    # Start the tkinter event loop
    root.mainloop()

########################################### (helpers for getting embedded binary resources from pyinstaller) ####################################################

import os
import sys
from tkinter import filedialog  # for file browse dialog
from tkinter import messagebox  # for showing dialogs to user

def show_dialog(type, message):
    if type == 'Error':
        messagebox.showerror(type, message)
    elif type == 'Info':
        messagebox.showinfo(type, message)

def browse_file_from_disk():
    # return the file path
    return filedialog.askopenfilename()

def get_resource_path(relative_path):
    '''
    Returns the absolute path of a resource file
    that has been packed within the exe.
    It removes the necessity of the .ui file at the exe path

    The file is stored inside %%TEMP%% directory,
    in a folder named _MEIxxxxxx, where xxxxxx is a
    random number.
    eq: C:/Users/<USERNAME>/AppData/Local/Temp/_MEI187482/resources/background.gif

    Ref: https://stackoverflow.com/a/55085519
    '''
    if getattr(sys, 'frozen', False):
        return os.path.join(sys._MEIPASS, relative_path)

    return os.path.join(os.path.abspath('.'), relative_path)
