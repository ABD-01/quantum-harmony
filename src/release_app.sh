#!/bin/bash
# ----------------------------------------------------------------------------
# [Author] Accolade Electronics <www.accoladeelectronics.com>
# [Date]   01 Apr 2024
#          
#          Description:
#          Invoke this script manually for creating windows executable
#          
#          Dependencies: pip3 install pyinstaller for creating exe file
#                        7z must be installed for creating zip file
# ----------------------------------------------------------------------------

export PATH="$PATH:/c/Program Files/7-Zip"

echo "Enter the plain release version e.g. 0.2"

read VERSION

echo ""
echo "[i] RELEASING uCOMMANDER VERSION v${VERSION}"

pyinstaller --log-level ERROR --onefile --add-data="aepl.ico;." --add-data="background.gif;." --add-data="PCANBasic.dll;." --add-data="PCAN-UDS.dll;." --add-data="PCAN-ISO-TP.dll;."  main.py app_logic.py app_ui.py PCAN_UDS_2013.py PCAN_ISO_TP_2016.py PCANBasic.py

exit 0

cp dist/main.exe ./uCommander_v${VERSION}.exe
cp ./doc/release_notes/*.pdf .

7z a uCommander_v${VERSION}.zip uCommander_v${VERSION}.exe release_notes_v${VERSION}.pdf

rm -rf ./artifacts/uCommander/*

cp *.zip ./artifacts/uCommander/

# cleanup
rm -rf *.spec build dist *.exe *.pdf *.zip *.docx

echo "[i] DONE"
