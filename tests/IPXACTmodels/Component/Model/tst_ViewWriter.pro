#-----------------------------------------------------------------------------
# File: tst_ViewWriter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 03.09.2015
#
# Description:
# Qt project file template for running unit tests for View Writer.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ViewWriter

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

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_ViewWriter.pri)
