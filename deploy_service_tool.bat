@echo off

:: -----------------------------------------------------------------------
:: Author: Muhammed Abdullah
:: Date: 12 Feb 2024
:: Description:  Invoke this batch file for creating windows executable
:: -----------------------------------------------------------------------

for /f "tokens=2 delims='" %%a in ('findstr /r "VERSION=.[\d.]*" service_tool.py') do set VERSION=%%a

cls

echo.
echo [i] RELEASING SERVICE TOOL VERSION %VERSION%

pyinstaller -n service_tool_v%VERSION% --noconfirm --clean --log-level=WARN ^
--noconsole ^
--onefile ^
--add-data="resources/thumbnail.ico;resources" ^
--add-data="resources/loading.gif;resources" ^
--add-data="service_tool.ui;." ^
--add-data="resources/serial_upload.dll;resources" ^
--icon=resources/thumbnail.ico ^
service_tool.py

echo.
echo [i] CREATING  ZIP ARCHIVE service_tool_v%VERSION%.tar.gz

copy dist\service_tool_v%VERSION%.exe .
copy release_notes\release_notes.pdf .

tar -czvf service_tool_v%VERSION%.tar.gz service_tool_v%VERSION%.exe release_notes.pdf setup_cacert.bat

del /q *.spec *.exe *.pdf *.docx build

echo.
echo  [i] DONE
echo.
pause