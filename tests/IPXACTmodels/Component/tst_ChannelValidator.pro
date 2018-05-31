#-----------------------------------------------------------------------------
# File: tst_ChannelValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 19.11.2015
#
# Description:
# Qt project file template for running unit tests for Channel Validator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ChannelValidator
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
include(tst_ChannelValidator.pri)
