#-----------------------------------------------------------------------------
# File: DesignConfigurationWriter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 3.8.2015
#
# Description:
# Qt project file for running unit tests for design configuration writer.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_DesignConfigurationWriter

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
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_DesignConfigurationWriter.pri)
