#-----------------------------------------------------------------------------
# File: tst_ChoiceWriter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 21.09.2015
#
# Description:
# Qt project file template for running unit tests for Choice writer.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ChoiceWriter

QT += core xml testlib
QT -= gui

CONFIG += c++11 testcase console
DEFINES += IPXACTMODELS_LIB

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_ChoiceWriter.pri)
