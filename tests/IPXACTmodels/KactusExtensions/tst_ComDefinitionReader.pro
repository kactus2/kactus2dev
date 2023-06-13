#-----------------------------------------------------------------------------
# File: tst_ComDefinitionReader.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 28.01.2016
#
# Description:
# Qt project file template for running unit tests for ComDefinition reader.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ComDefinitionReader

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
include(tst_ComDefinitionReader.pri)
