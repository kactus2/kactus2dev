REM
REM Run windeployqt on release build
REM
@echo off

call CreateHelp.bat

echo Running windeployqt
windeployqt executable/Kactus2.exe --no-translations
windeployqt executable/IPXACTmodels.dll --no-translations