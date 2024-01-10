#-----------------------------------------------------------------------------
# File: tst_OtherClockDriverWriter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 02.10.2015
#
# Description:
# Qt project file template for running unit tests for OtherClockDriver writer.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_OtherClockDriverWriter

QT += core xml testlib
QT -= gui

CONFIG += c++11 testcase console

linux-g++ | linux-g++-64 | linux-g++-32 {
 LIBS += -L../../../executable \
     -lIPXACTmodels
}
win64 | win32 {
 LIBS += -L../../../executable \
     -lIPXACTmodelsd
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_OtherClockDriverWriter.pri)
