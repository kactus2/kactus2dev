#-----------------------------------------------------------------------------
# File: tst_AddressSpaceValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 30.11.2015
#
# Description:
# Qt project file template for running unit tests for AddressSpaceValidator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_AddressSpaceValidator

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
INCLUDEPATH += ../../../KactusAPI/include

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_AddressSpaceValidator.pri)
