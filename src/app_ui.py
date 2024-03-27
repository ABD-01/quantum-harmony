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
    icon_path = "aepl.ico"

    # Set the window title icon
    root.iconbitmap(icon_path)

    # todo: set the taskbar icon from pyinstaller

    # Make the window non-resizable
    root.resizable(False, False)

    # Load the background image
    background_image = tk.PhotoImage(file="background.gif")

    # Create a label to display the background image
    background_label = tk.Label(root, image=background_image)
    background_label.place(x=0, y=0, relwidth=1, relheight=1)

    # To prevent garbage collection of the image object
    background_label.image = background_image

    return root

def create_buttons(root, browse_file, boot_lock, upload_file):
    button_font = ("Fira Sans", 12)
    start_button = tk.Button(root, text="Browse", font=button_font, bg="#04508e", fg="white", command=browse_file)
    start_button.place(x=20, y=100)

    idle_button = tk.Button(root, text="Boot lock", font=button_font, bg="#04508e", fg="white", command=boot_lock)
    idle_button.place(x=100, y=100)

    idle_button = tk.Button(root, text="Upload", font=button_font, bg="#04508e", fg="white", command=upload_file)
    idle_button.place(x=195, y=100)

def create_log_window(root):
    log_font = ("Consolas", 8)

    # Create a frame for the log window
    log_frame = tk.Frame(root, bg="#d9e6ee")
    log_frame.place(x=10, y=290, width=580, height=100)  # Place at the bottom of the UI

    # Create a Text widget to display log messages
    log_window = tk.Text(log_frame, wrap="word", font=log_font, state="disabled", bg="#d9e6ee", fg="black")
    log_window.pack(side="left", fill="both", expand=True)

    # Create a Scrollbar for the Text widget
    scrollbar = tk.Scrollbar(log_frame, orient="vertical", command=log_window.yview)
    scrollbar.pack(side="right", fill="y")

    # Configure the Text widget to use the Scrollbar
    log_window.configure(yscrollcommand=scrollbar.set)

    return log_window

def append_log(log_window, message):
    # Enable editing
    log_window.configure(state="normal")
    
    # Insert message
    log_window.insert("end", message + "\n")
    
    # Disable editing
    log_window.configure(state="disabled")
    
    # Scroll to the end
    log_window.see("end")

def bind_close_event(root, on_close):
    # Bind the closing event to the function
    root.protocol("WM_DELETE_WINDOW", on_close)
    
def run_gui(root):
    # Start the tkinter event loop
    root.mainloop()