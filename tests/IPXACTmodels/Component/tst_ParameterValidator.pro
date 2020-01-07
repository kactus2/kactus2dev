#-----------------------------------------------------------------------------
# File: tst_ParameterValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 13.11.2014
#
# Description:
# Qt project file template for running unit tests for ParameterValidator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ParameterValidator

QT += core gui xml testlib

CONFIG += c++11 testcase console

linux-g++ | linux-g++-64 | linux-g++-32 {
 LIBS += -L../../../executable \
     -lIPXACTmodels
}
win64 | win32 {
 LIBS += -L../../../x64/executable \
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
include(tst_ParameterValidator.pri)
