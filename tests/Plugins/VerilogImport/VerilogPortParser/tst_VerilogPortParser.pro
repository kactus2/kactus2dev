#-----------------------------------------------------------------------------
# File: tst_VerilogPortParser.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 24.09.2014
#
# Description:
# Qt project file template for running unit tests for a VerilogPortParser.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_VerilogPortParser

QT += core xml gui testlib
CONFIG += testcase console

win32:CONFIG(release, debug|release) {
    LIBS += -L$$PWD/../../../../executable/ -lIPXACTmodels
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../../../../executable/ -lIPXACTmodelsd
    DESTDIR = ./debug
}
else:unix {
    LIBS += -L$$PWD/../../../../executable/ -lIPXACTmodels
    DESTDIR = ./release
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../../

DEPENDPATH += .
DEPENDPATH += ../../../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_VerilogPortParser.pri)
