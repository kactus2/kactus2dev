#-----------------------------------------------------------------------------
# File: tst_OtherClockDriverReader.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 02.10.2015
#
# Description:
# Qt project file template for running unit tests for OtherClockDriver reader.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_OtherClockDriverReader

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
include(tst_OtherClockDriverReader.pri)
