#-----------------------------------------------------------------------------
# Instantiations: tst_InstantiationsValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 23.11.2015
#
# Description:
# Qt project Instantiations template for running unit tests for Enumerated Value Validator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_InstantiationsValidator

QT += core xml gui widgets testlib
CONFIG += testcase console
DEFINES += IPXACTMODELS_LIB

win32:CONFIG(release, debug|release) {
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
    DESTDIR = ./debug
}
else:unix {
    DESTDIR = ./release
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../../../

DEPENDPATH += .
DEPENDPATH += ../../../../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedInstantiationss
UI_DIR += ./generatedInstantiationss
RCC_DIR += ./generatedInstantiationss
include(tst_InstantiationsValidator.pri)
