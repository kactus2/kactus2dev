#-----------------------------------------------------------------------------
# File: tst_ChoiceValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 07.12.2015
#
# Description:
# Qt project file template for running unit tests for ChoiceValidator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ChoiceValidator

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
include(tst_ChoiceValidator.pri)
