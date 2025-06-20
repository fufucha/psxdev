@echo off
setlocal

rem Get the current directory
for %%I in (.) do set CURDIR=%%~nxI

set ROOT=..\..

call %ROOT%\BUILD.bat %CURDIR% main.c
pause
