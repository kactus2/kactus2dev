#-----------------------------------------------------------------------------
# File: tst_ParameterWriter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 31.07.2015
#
# Description:
# Qt project file template for running unit tests for class ParameterWriter.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ParameterWriter

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
include(tst_ParameterWriter.pri)
