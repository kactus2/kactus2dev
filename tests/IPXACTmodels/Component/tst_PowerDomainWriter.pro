#-----------------------------------------------------------------------------
# File: tst_PowerDomainWriter.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Esko Pekkarinen
# Date: 05.10.2023
#
# Description:
# Qt project file for running unit tests for PowerDomain writer.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_PowerDomainWriter

QT += core xml testlib
QT -= gui

CONFIG += c++11 testcase console

LIBS += -L../../../executable -lIPXACTmodels

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_PowerDomainWriter.pri)
