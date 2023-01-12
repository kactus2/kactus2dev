REM
REM Bind HTML pages to context helps. If you modify help texts, you
REM don't need recompilation, if you run this script.
REM Joni-Matti M��tt�, Aug 2012
REM
@echo off
qhelpgenerator Help/kactus2help.qhcp -o Help/Kactus2Help.qhc
xcopy /Y Help\Kactus2Help.qhc executable\Help\
xcopy /Y Help\Kactus2Help.qch executable\Help\
xcopy /Y Help\Kactus2Help.qhc x64\executable\Help\
xcopy /Y Help\Kactus2Help.qch x64\executable\Help\