#-----------------------------------------------------------------------------
# File: tst_MemoryMapValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 25.11.2015
#
# Description:
# Qt project file template for running unit tests for MemoryMapValidator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_MemoryMapValidator

QT += core gui xml testlib

CONFIG += c++11 testcase console

linux-g++ | linux-g++-64 | linux-g++-32 {
 LIBS += -L../../../executable \
     -lIPXACTmodels \
     
}
win64 | win32 {
 LIBS += -L../../../executable \
     -lIPXACTmodelsd \
     -lKactusAPId
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_MemoryMapValidator.pri)
