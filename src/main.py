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
brief       This is the main script file for launching the service tool application

date        22 March 2024
author      Accolade Electronics <www.accoladeelectronics.com>

tested on python 3.11.8 on windows 11 x64

# dependencies
pip3 install tk
pip3 install toml

# run the script to launch app
launch_app.sh
'''

import app_logic

def main():
    app_logic.start()

if __name__ == "__main__":
    main()
