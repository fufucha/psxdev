@echo off
setlocal

rem Get the current directory 
for %%I in (.) do set CURDIR=%%~nxI

set ROOT=..\..
set SOUNDNAME=bach_gv5

if not exist %SOUNDNAME%.wav (
    echo %SOUNDNAME%.wav is missing
    pause
    exit /b 1
)

if exist %SOUNDNAME%.vag del %SOUNDNAME%.vag
if exist %SOUNDNAME%.raw del %SOUNDNAME%.raw
if exist %SOUNDNAME%.h del %SOUNDNAME%.h

set PATH=%PATH%;%ROOT%\bin\SOUND

echo.
echo [WARNING] Please wait until WAV2VAG has completed, then press any key to continue...
echo.

%ROOT%\bin\SOUND\WAV2VAG.EXE %SOUNDNAME%
pause

if not exist %SOUNDNAME%.vag (
    echo WAV2VAG failed. No VAG created.
    pause
    exit /b 1
)

%ROOT%\bin\bin2h.exe %SOUNDNAME%.vag %SOUNDNAME%.h %SOUNDNAME%_vag
if not exist %SOUNDNAME%.h (
    echo BIN2H failed. No header created.
    pause
    exit /b 1
)

call %ROOT%\BUILD.bat %CURDIR% main.c
pause
