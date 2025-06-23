@echo off
setlocal

rem Get the current directory 
for %%I in (.) do set CURDIR=%%~nxI

set ROOT=..\..
set IMGNAME=logo

if not exist %IMGNAME%.bmp (
    echo %IMGNAME%.bmp is missing
    pause
    exit /b 1
)

if exist %IMGNAME%.tim del %IMGNAME%.tim
if exist %IMGNAME%.h del %IMGNAME%.h

%ROOT%\bin\TIM\BMP2TIM.EXE %IMGNAME%.bmp %IMGNAME%.tim
if not exist %IMGNAME%.tim (
    echo BMP2TIM failed. No TIM created.
    pause
    exit /b 1
)

%ROOT%\bin\bin2h.exe %IMGNAME%.tim %IMGNAME%.h image
if not exist %IMGNAME%.h (
    echo BIN2H failed. No header created.
    pause
    exit /b 1
)

call %ROOT%\BUILD.bat %CURDIR% main.c

pause
