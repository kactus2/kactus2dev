#-----------------------------------------------------------------------------
# File: tst_CatalogReader.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 01.02.2017
#
# Description:
# Qt project file for running unit tests for Catalog Reader.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_CatalogReader

QT += core xml testlib
QT -= gui

CONFIG += c++11 testcase console
DEFINES += IPXACTMODELS_LIB

DESTDIR = ./release

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_CatalogReader.pri)
