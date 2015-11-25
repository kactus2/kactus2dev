#-----------------------------------------------------------------------------
# File: tst_MemoryMapBaseValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 25.11.2015
#
# Description:
# Qt project file template for running unit tests for MemoryMapBaseValidator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_MemoryMapBaseValidator

QT += core xml gui widgets testlib
CONFIG += testcase console
DEFINES += IPXACTMODELS_LIB

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
INCLUDEPATH += ../../../../../

DEPENDPATH += .
DEPENDPATH += ../../../../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_MemoryMapBaseValidator.pri)
