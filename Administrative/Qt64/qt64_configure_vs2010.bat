@echo off
set QTDIR=%CD%\qtbase
set QMAKESPEC=win32-msvc2010
set PATH=%QTDIR%\bin;%PATH%
configure -debug-and-release -opensource -platform win32-msvc2010 -mp -make examples -make tools