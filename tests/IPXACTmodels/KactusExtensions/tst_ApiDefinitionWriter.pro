#-----------------------------------------------------------------------------
# File: tst_ApiDefinitionWriter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 11.12.2015
#
# Description:
# Qt project file template for running unit tests for ApiDefinition writer.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ApiDefinitionWriter

QT += core xml testlib
QT -= gui

CONFIG += c++11 testcase console

LIBS += -L../../../executable -lIPXACTmodels

DESTDIR = ./release

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_ApiDefinitionWriter.pri)
