#-----------------------------------------------------------------------------
# File: tst_ParameterReader.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 03.08.2015
#
# Description:
# Qt project file template for running unit tests for class ParameterReader.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ParameterReader

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
include(tst_ParameterReader.pri)
