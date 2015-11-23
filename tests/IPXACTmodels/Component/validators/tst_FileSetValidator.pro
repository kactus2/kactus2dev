#-----------------------------------------------------------------------------
# FileSet: tst_FileSetValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 20.11.2015
#
# Description:
# Qt project FileSet template for running unit tests for Enumerated Value Validator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_FileSetValidator

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
INCLUDEPATH += ../../../../

DEPENDPATH += .
DEPENDPATH += ../../../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFileSets
UI_DIR += ./generatedFileSets
RCC_DIR += ./generatedFileSets
include(tst_FileSetValidator.pri)
