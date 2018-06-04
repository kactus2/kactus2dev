#-----------------------------------------------------------------------------
# File: tst_BusInterfaceValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 01.12.2015
#
# Description:
# Qt project file template for running unit tests for BusInterfaceValidator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_BusInterfaceValidator

QT += core xml gui testlib
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
include(tst_BusInterfaceValidator.pri)
