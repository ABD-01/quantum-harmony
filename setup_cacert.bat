:: -----------------------------------------------------------------------------------
:: Author: Muhammed Abdullah
:: Date: 5 July 2024
:: Description:  Invoke this batch to copy CA_cert.pem file into all ACON* directories
:: -----------------------------------------------------------------------------------

@echo off
setlocal
cls

REM %~dp0 returns the full path of the parent folder
set CA_CERT_FILE=%~dp0\CA_cert.pem

if not exist "%CA_CERT_FILE%" (
    echo ERROR: %CA_CERT_FILE% NOT FOUND
    pause
    exit /b 1
)

echo [i] COPYING %CA_CERT_FILE% INTO ALL ACON* DIRECTORIES
echo.

set /a COUNT=0

for /d %%i in (ACON*) do (
    if not exist "%%i\CA_cert.pem" (
        copy "%CA_CERT_FILE%" "%%i\CA_cert.pem" > nul
        echo "CA_cert.pem -> %%i\CA_cert.pem"
        set /a COUNT+=1
    ) else (
        echo "%%i\CA_cert.pem ALREADY EXISTS"
    )
)

echo.
echo [i] CREATED %COUNT% COPIES OF %CA_CERT_FILE%

echo.
echo [i] DONE
echo.

endlocal
pause
