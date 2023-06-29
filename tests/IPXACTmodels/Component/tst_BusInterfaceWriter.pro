#-----------------------------------------------------------------------------
# File: tst_BusInterfaceWriter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 22.09.2015
#
# Description:
# Qt project file template for running unit tests for bus interface writer.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_BusInterfaceWriter

QT += core xml testlib
QT -= gui

CONFIG += c++11 testcase console

LIBS += -L../../../executable -lIPXACTmodels

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_BusInterfaceWriter.pri)
