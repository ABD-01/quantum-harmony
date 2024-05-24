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

PROJECT_DIRECTORY=$(pwd)
echo $PROJECT_DIRECTORY
cd src/

sed -i "s/\(build \)[[:digit:]]\+ [[:alpha:]]\+ [[:digit:]]\+/\1$(date +"%d %B %Y")/" app_ui.py
VERSION=$(grep version config.toml | cut -d '"' -f2) # -f4 for config.json

echo ""
echo "[i] RELEASING uCOMMANDER VERSION v${VERSION}"

pyinstaller --name uCommander_v${VERSION}.exe --log-level WARN \
    --onefile --noconsole --noconfirm --clean \
    --icon resources/aepl.ico \
    --add-data="resources/aepl.ico;resources" \
    --add-data="resources/background.gif;resources" \
    --add-data="uds_stack/PCANBasic.dll;uds_stack" \
    --add-data="uds_stack/PCANBasic.py;uds_stack" \
    --add-data="uds_stack/PCAN-UDS.dll;uds_stack" \
    --add-data="uds_stack/PCAN_UDS_2013.py;uds_stack" \
    --add-data="uds_stack/PCAN-ISO-TP.dll;uds_stack" \
    --add-data="uds_stack/PCAN_ISO_TP_2016.py;uds_stack" \
    --add-data="resources/fonts/fira-sans.regular.ttf;resources/fonts" \
    --add-data="resources/fonts/whitrabt.ttf;resources/fonts" \
    --add-data="resources/fonts/TitilliumWeb-Regular.ttf;resources/fonts" \
    --add-data="config.toml;." \
    --hidden-import platform \
    main.py

cd $PROJECT_DIRECTORY
WORKING_DIRECTORY=./artifacts/uCommander
if [ -d "$WORKING_DIRECTORY" ]; then
    echo "[i] Removing old artifacts"
    rm -rf $WORKING_DIRECTORY
fi
mkdir -p $WORKING_DIRECTORY

echo "[i] Copying artifacts"
cp -v ./src/dist/uCommander_v${VERSION}.exe $WORKING_DIRECTORY/
cp -v ./doc/release_notes/*.pdf $WORKING_DIRECTORY/

cd $WORKING_DIRECTORY

7z a uCommander_v${VERSION}.zip uCommander_v${VERSION}.exe release_notes_v*.pdf

# cleanup
cd $PROJECT_DIRECTORY
echo "[i] CLEANUP"

rm -rf $WORKING_DIRECTORY/*.exe $WORKING_DIRECTORY/*.pdf src/dist src/build src/*.spec

echo "[i] DONE"
