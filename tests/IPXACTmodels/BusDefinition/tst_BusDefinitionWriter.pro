#-----------------------------------------------------------------------------
# File: tst_BusDefinitionWriter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 05.08.2015
#
# Description:
# Qt project file for running unit tests for BusDefinition Writer.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_BusDefinitionWriter

QT += core xml testlib
QT -= gui

CONFIG += c++11 testcase console

linux-g++ | linux-g++-64 | linux-g++-32 {
 LIBS += -L../../../executable \
     -lIPXACTmodels

}
win64 | win32 {
 LIBS += -L../../../executable/ \
     -lIPXACTmodelsd
}

DESTDIR = ./release

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_BusDefinitionWriter.pri)
