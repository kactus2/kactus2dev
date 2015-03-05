#-----------------------------------------------------------------------------
# File: tst_ValueFormatter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 05.03.2015
#
# Description:
# Qt project file template for running unit tests for ValueFormatter.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ValueFormatter

QT += core xml gui testlib
CONFIG += testcase console

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
include(tst_ValueFormatter.pri)
