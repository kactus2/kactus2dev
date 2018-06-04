#-----------------------------------------------------------------------------
# File: tst_ComponentReader.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 14.10.2015
#
# Description:
# Qt project file template for running unit tests for a ComponentReader.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ComponentReader

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
include(tst_ComponentReader.pri)
