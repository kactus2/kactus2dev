#-----------------------------------------------------------------------------
# File: tst_SystemVerilogExpressionParser.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 25.11.2014
#
# Description:
# Qt project file template for running unit tests for SystemVerilogExpressionParser.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_SystemVerilogExpressionParser

QT += core xml gui testlib
CONFIG += c++11 testcase console

win32:CONFIG(release, debug|release) {
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
    DESTDIR = ./debug
}
else:unix {
    DESTDIR = ./release
	}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../
INCLUDEPATH += ../../KactusAPI/include/

DEPENDPATH += .
DEPENDPATH += ../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_SystemVerilogExpressionParser.pri)
