import sys,os
from tkinter import *
from tkinter import ttk
from tkinter import font        # for fonts

import sv_ttk

def main():
    load_fonts()
    app = Tk() 

    app.title('Accolade Service Tool')
    app.geometry('600x400')

    # icon_path = PhotoImage(get_resource_path('resources/aepl.ico'))
    # app.iconphoto(False, icon_path)

    frame1 = ttk.LabelFrame(app, text="uCommander")
    frame1.grid(row=0, column=0, columnspan=2, sticky='nsew', padx=15, pady=5, ipadx=5, ipady=5) 

    sw_label = Label(frame1, text='uCommander v{}'.format('0.2.3'), font=('White Rabbit', 24))
    sw_label.pack()
    version_label = Label(frame1, text='build 17 Apr 2024', font=('White Rabbit', 9))
    version_label.pack()


    frame2 = ttk.LabelFrame(app, text="Can Settings", width=278) 
    frame2.grid(row=1, column=0, sticky='nswe', padx=(15,7), pady=5, ipadx=5, ipady=5) 

    label_font = font.Font(frame2, family='Fira Sans', size=10, weight='normal')

    label = ttk.Label(frame2, text="Project")
    label.grid(row=0, column=0, sticky='ew', padx=5, pady=5)

    project_combobox = ttk.Combobox(frame2, values=["Project 1", "Project 2", "Project 3"])
    project_combobox.grid(row=0, column=1, sticky='ew', padx=5, pady=5)


    label = ttk.Label(frame2, text="Bit rate")
    label.grid(row=1, column=0, sticky='ew', padx=5, pady=5)

    bitrate_combobox = ttk.Combobox(frame2, values=["500 kBit/s", "1 MBit/s"], state='readonly')
    bitrate_combobox.current(0)
    bitrate_combobox.grid(row=1, column=1, sticky='ew', padx=5, pady=5)

    label = ttk.Label(frame2, text="Tester id")
    label.grid(row=2, column=0, sticky='ew', padx=5, pady=5)

    testerid_enrty = ttk.Entry(frame2)
    testerid_enrty.grid(row=2, column=1, sticky='ew', padx=5, pady=5)

    label = ttk.Label(frame2, text="ECU id")
    label.grid(row=3, column=0, sticky='ew', padx=5, pady=5)

    ecuid_enrty = ttk.Entry(frame2)
    ecuid_enrty.grid(row=3, column=1, sticky='ew', padx=5, pady=5)

    connect_button = ttk.Button(frame2, text="Connect")
    connect_button.grid(row=4, column=0, columnspan=2, sticky='ew', padx=5, pady=5)

    frame2.columnconfigure([0, 1], weight=1)
    frame2.rowconfigure([0, 1, 2, 3, 4], weight=1)


    frame3 = ttk.LabelFrame(app, text="ECU Information", width=278)
    frame3.grid(row=1, column=1, sticky='nsew', padx=(7,15), pady=5, ipadx=5, ipady=5)

    bootlock_button = ttk.Button(frame3, text="Bootlock")
    bootlock_button.grid(row=4, column=0, sticky='ew', padx=5, pady=5)

    resetecu_button = ttk.Button(frame3, text="Reset ECU")
    resetecu_button.grid(row=4, column=1, sticky='ew', padx=5, pady=5)

    frame3.columnconfigure([0, 1], weight=1)
    frame3.rowconfigure([0, 1, 2, 3, 4], weight=1)


    frame4 = ttk.LabelFrame(app, text="File Upload")
    frame4.grid(row=2, column=0, columnspan=2, sticky='nsew', padx=15,
                pady=15, ipadx=5, ipady=5)

    filename_entry = ttk.Entry(frame4)
    filename_entry.grid(row=0, column=0, columnspan=4, sticky='ew', padx=5, pady=2)

    browse_button = ttk.Button(frame4, text="Browse")
    browse_button.grid(row=0, column=4, sticky='ew', padx=5, pady=2)
    upload_button = ttk.Button(frame4, text="Upload")
    upload_button.grid(row=0, column=5, sticky='ew', padx=5, pady=2)

    progressbar = ttk.Progressbar(frame4, orient="horizontal", mode="determinate")
    progressbar.grid(row=1, column=0, columnspan=6, sticky='ew', padx=5,
                     pady=0)
    progressbar.start(10)

    frame4.columnconfigure([0, 1, 2, 3, 4, 5], weight=1)
    frame4.rowconfigure([0, 1], weight=1)


    # sv_ttk.set_theme("light")

    app.columnconfigure(0, weight=1)
    app.columnconfigure(1, weight=1)
    app.rowconfigure([0,2], weight=1)
    app.rowconfigure(1, weight=3)
    app.mainloop() 


def load_fonts():
    _load_font(get_resource_path("resources/fonts/fira-sans.regular.ttf"))
    _load_font(get_resource_path("resources/fonts/TitilliumWeb-Regular.ttf"))
    _load_font(get_resource_path("resources/fonts/whitrabt.ttf"))
    
def _load_font(font_path: str, private: bool = False, enumerable: bool = False) -> bool:
    """ Function taken from: https://stackoverflow.com/questions/11993290/truly-custom-font-in-tkinter/30631309#30631309 """

    from ctypes import windll, byref, create_unicode_buffer, create_string_buffer

    FR_PRIVATE = 0x10
    FR_NOT_ENUM = 0x20

    if isinstance(font_path, bytes):
        path_buffer = create_string_buffer(font_path)
        add_font_resource_ex = windll.gdi32.AddFontResourceExA
    elif isinstance(font_path, str):
        path_buffer = create_unicode_buffer(font_path)
        add_font_resource_ex = windll.gdi32.AddFontResourceExW
    else:
        raise TypeError('font_path must be of type bytes or str')

    flags = (FR_PRIVATE if private else 0) | (FR_NOT_ENUM if not enumerable else 0)
    num_fonts_added = add_font_resource_ex(byref(path_buffer), flags, 0)
    return bool(min(num_fonts_added, 1))


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


if __name__ == '__main__':
    main()
