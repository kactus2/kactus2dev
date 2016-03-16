#-----------------------------------------------------------------------------
# ComponentPort: tst_ComponentPortValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 25.11.2015
#
# Description:
# Qt project ComponentPort template for running unit tests for Enumerated Value Validator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ComponentPortValidator

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

MOC_DIR += ./generatedComponentPorts
UI_DIR += ./generatedComponentPorts
RCC_DIR += ./generatedComponentPorts
include(tst_ComponentPortValidator.pri)
