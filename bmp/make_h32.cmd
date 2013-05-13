@echo off
IF "%1"=="" goto usage

set BMP2BIN=bmp2bin_rgb.exe
set BIN2H=bin_to_h.exe

del %~n1.bin
del %~n1.h
%BMP2BIN% %1 %~n1.bin %2
if ERRORLEVEL 1 goto error
%BIN2H% %~n1.bin %~n1
goto exit
:usage
echo USAGE: this.cmd file.bmp
goto exit
:error
echo ERROR
:exit