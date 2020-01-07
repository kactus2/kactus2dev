#-----------------------------------------------------------------------------
# Instantiations: tst_InstantiationsValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 23.11.2015
#
# Description:
# Qt project Instantiations template for running unit tests for Enumerated Value Validator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_InstantiationsValidator

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
include(tst_InstantiationsValidator.pri)
