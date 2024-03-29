#-----------------------------------------------------------------------------
# File: tst_CPUValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 18.11.2015
#
# Description:
# Qt project file template for running unit tests for Enumerated Value Validator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_CPUValidator

QT += core gui xml testlib

CONFIG += c++11 testcase console

linux-g++ | linux-g++-64 | linux-g++-32 {
 LIBS += -L../../../executable \
     -lIPXACTmodels -lKactusAPI

}
win64 | win32 {
 LIBS += -L../../../executable \
     -lIPXACTmodelsd -lKactusAPId
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_CPUValidator.pri)
