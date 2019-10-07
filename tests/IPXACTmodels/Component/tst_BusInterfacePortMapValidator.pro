#-----------------------------------------------------------------------------
# File: tst_BusInterfacePortMapValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Esko Pekkarinen
# Date: 28.03.2018
#
# Description:
# Qt project file template for running unit tests for a single module.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_BusInterfacePortMapValidator

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
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../


MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_BusInterfacePortMapValidator.pri)
