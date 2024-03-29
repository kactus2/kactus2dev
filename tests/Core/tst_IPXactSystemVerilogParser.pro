#-----------------------------------------------------------------------------
# File: tst_IPXactSystemVerilogParser.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 02.12.2014
#
# Description:
# Qt project file template for running unit tests for IPXactSystemVerilogParser.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_IPXactSystemVerilogParser

QT += core xml testlib
QT -= gui widgets
CONFIG += c++11 testcase console

win32:CONFIG(release, debug|release) {
    LIBS += -L$$PWD/../../executable/ -lIPXACTmodels -lKactusAPI
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../../executable/ -lIPXACTmodelsd -lKactusAPId
    DESTDIR = ./debug
}
else:unix {
    LIBS += -L$$PWD/../../executable/ -lIPXACTmodels -lKactusAPI
    DESTDIR = ./release
}

INCLUDEPATH += $$PWD/../../
INCLUDEPATH += $$PWD/../../executable
INCLUDEPATH += $$DESTDIR

DEPENDPATH += $$PWD/../../
DEPENDPATH += $$PWD/../../executable
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_IPXactSystemVerilogParser.pri)
