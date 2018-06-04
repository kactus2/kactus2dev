#-----------------------------------------------------------------------------
# File: tst_AddressBlockValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 23.11.2015
#
# Description:
# Qt project file template for running unit tests for AddressBlockValidator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_AddressBlockValidator

QT += core gui xml testlib

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
include(tst_AddressBlockValidator.pri)
