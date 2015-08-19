#-----------------------------------------------------------------------------
# File: tst_AbstractionDefinitionWriter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 05.08.2015
#
# Description:
# Qt project file for running unit tests for AbstractionDefinition Writer.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_AbstractionDefinitionWriter

QT += core xml gui testlib
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

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_AbstractionDefinitionWriter.pri)
