@echo off
setlocal

cd /d "%~dp0"

if "%1"=="" (
    echo Please run this script by calling BUILD.bat from a project folder in src
    pause
    exit /b 1
)

set ROOT=%~dp0
set PROJECT=%1
set SRC=src\%PROJECT%\%2
set OUTDIR=build\%PROJECT%
set TMPDIR=tmp

if exist %OUTDIR% rmdir /s /q %OUTDIR%
if exist %TMPDIR% rmdir /s /q %TMPDIR%
mkdir %OUTDIR%
mkdir %TMPDIR%

bin\PSYQ\CCPSX.EXE -O2 -G0 -Xo$80010000 -Wall -o %TMPDIR%\MAIN.CPE %SRC%
if errorlevel 1 (
    echo CCPSX failed.
    pause
    exit /b 1
)

bin\PSYQ\CPE2X.EXE %TMPDIR%\MAIN.CPE /CE
if errorlevel 1 (
    echo CPE2X failed.
    pause
    exit /b 1
)

move /Y %TMPDIR%\MAIN.CPE %OUTDIR%\MAIN.CPE >nul
move /Y %TMPDIR%\MAIN.EXE %OUTDIR%\MAIN.EXE >nul

copy /Y templates\SYSTEM.CNF %OUTDIR%\SYSTEM.CNF >nul
copy /Y templates\MAIN.CTI %OUTDIR%\MAIN.CTI >nul

pushd %OUTDIR%
%ROOT%bin\BUILDCD\BUILDCD.EXE -l -iDISC.IMG MAIN.CTI
if errorlevel 1 (
    echo BUILDCD failed.
    pause
    popd
    exit /b 1
)
%ROOT%bin\BUILDCD\STRIPISO.EXE S 2352 DISC.IMG DISC.ISO
if errorlevel 1 (
    echo STRIPISO failed.
    pause
    popd
    exit /b 1
)
if exist %ROOT%bin\BUILDCD\PSXLICENSE.EXE (
    %ROOT%bin\BUILDCD\PSXLICENSE.EXE /eu /i DISC.ISO
)
popd

rmdir /s /q %TMPDIR%

