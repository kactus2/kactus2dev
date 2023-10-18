#-----------------------------------------------------------------------------
# File: tst_ModeConditionParser.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Esko Pekkarinen
# Date: 16.10.2023
#
# Description:
# Qt project file for running unit tests for ModeConditionParser.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ModeConditionParser

QT += core xml testlib
QT -= gui widgets
CONFIG += c++17 testcase console

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
include(tst_ModeConditionParser.pri)
