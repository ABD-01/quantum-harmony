#!/bin/bash
# ----------------------------------------------------------------------------
# [Author] Eshwar J
#          Description:
#          Invoke this script manually for creating windows executable
# ----------------------------------------------------------------------------

# Extract version number from script file.
VERSION=0.1.0

echo ""
echo "[i] RELEASING uCOMMANDER VERSION ${VERSION}"

#cp main.py ucommander_v${VERSION}.pyw
#cp app_logic.py app_logic.pyw
#cp app_ui.py app_ui.pyw

pyinstaller --log-level ERROR --onefile --add-data="aepl.ico;." --add-data="background.gif;." --add-data="PCANBasic.dll;." --add-data="PCAN-UDS.dll;." --add-data="PCAN-ISO-TP.dll;."  main.py app_logic.py app_ui.py PCAN_UDS_2013.py PCAN_ISO_TP_2016.py PCANBasic.py

exit 0

cp dist/* .

cp ./release_notes/* .

tar -czf service_tool_v${VERSION}.tar.gz service_tool_v${VERSION}.exe release_notes.pdf

cp *.tar.gz ./../../artifacts/service_tool/

# cleanup
rm -rf *.pyw *.spec build dist *.exe *.pdf *.tar.gz *.docx

echo "[i] DONE"