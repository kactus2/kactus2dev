#-----------------------------------------------------------------------------
# File: tst_AddressSpaceWriter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 01.10.2015
#
# Description:
# Qt project file template for running unit tests for AddressSpace Writer.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_AddressSpaceWriter

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
include(tst_AddressSpaceWriter.pri)
