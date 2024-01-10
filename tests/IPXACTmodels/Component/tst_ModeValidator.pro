#-----------------------------------------------------------------------------
# File: tst_ModeValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Esko Pekkarinen
# Date: 14.08.2023
#
# Description:
# Qt project file template for running unit tests for Mode Validator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ModeValidator

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
include(tst_ModeValidator.pri)
