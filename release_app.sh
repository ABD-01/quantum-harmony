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

VERSION=$(grep VERSION __version__.py | cut -d "'" -f2)

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
    --hidden-import platform \
    main.py

# exit 0
cd $PROJECT_DIRECTORY
WORKING_DIRECTORY=./artifacts/uCommander_v$VERSION
mkdir -p $WORKING_DIRECTORY

echo "[i] Copying artifacts"
cp -v ./src/dist/uCommander_v${VERSION}.exe $WORKING_DIRECTORY/
cp -v ./doc/release_notes/*.pdf $WORKING_DIRECTORY/

cd $WORKING_DIRECTORY

7z a uCommander_v${VERSION}.zip uCommander_v${VERSION}.exe release_notes_v${VERSION}.pdf

# rm -rf ./artifacts/uCommander/*

# cp *.zip ./artifacts/uCommander/

# cleanup
cd $PROJECT_DIRECTORY
echo "[i] CLEANUP"

rm -rf $WORKING_DIRECTORY/*.exe $WORKING_DIRECTORY/*.pdf src/dist src/build src/*.spec

echo "[i] DONE"
