#-----------------------------------------------------------------------------
# View: tst_ViewValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 24.11.2015
#
# Description:
# Qt project View template for running unit tests for Enumerated Value Validator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ViewValidator

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

MOC_DIR += ./generatedViews
UI_DIR += ./generatedViews
RCC_DIR += ./generatedViews
include(tst_ViewValidator.pri)
