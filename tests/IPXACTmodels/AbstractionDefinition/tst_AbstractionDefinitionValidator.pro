#-----------------------------------------------------------------------------
# File: tst_AbstractionDefinitionValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 04.12.2015
#
# Description:
# Qt project file for running unit tests for AbstractionDefinition Validator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_AbstractionDefinitionValidator

QT += core gui xml testlib

CONFIG += c++11 testcase console

linux-g++ | linux-g++-64 | linux-g++-32 {
 LIBS += -L../../../executable \
     -lIPXACTmodels -lKactusAPI

}
win64 | win32 {
 LIBS += -L$$PWD/../../../executable/ \
     -lIPXACTmodelsd -lKactusAPId
}

DESTDIR = ./release

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_AbstractionDefinitionValidator.pri)
