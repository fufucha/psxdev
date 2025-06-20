@echo off
setlocal

rem Get the current directory
for %%I in (.) do set CURDIR=%%~nxI

set ROOT=..\..
set EMULATOR=%ROOT%\bin\EMU\NO$PSX.EXE
set OUTDIR=%ROOT%\build\%CURDIR%
set ISO=%OUTDIR%\DISC.ISO

if not exist "%ISO%" (
    call %ROOT%\BUILD.bat %CURDIR% main.c
)

if not exist "%ISO%" (
    echo DISC.ISO missing, build failed!
    pause
    exit /b 1
)

if not exist "%EMULATOR%" (
    echo NO$PSX.EXE not found at %EMULATOR%!
    pause
    exit /b 1
)

pushd %OUTDIR%
%EMULATOR% DISC.ISO
popd
