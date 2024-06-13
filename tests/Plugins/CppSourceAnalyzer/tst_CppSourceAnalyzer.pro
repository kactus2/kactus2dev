#-----------------------------------------------------------------------------
# File: tst_CppSourceAnalyzer.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Anton Hagqvist
# Date: 11.06.2024
#
# Description:
# Qt project file template for running unit tests for CppSourceAnalyzer.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_CppSourceAnalyzer

QT += core xml gui testlib widgets
CONFIG += c++17 testcase console

win32:CONFIG(release, debug|release) {
    LIBS += -L$$PWD/../../../executable/ -lIPXACTmodels
    LIBS += -L$$PWD/../../../executable/ -lKactusAPI
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../../../executable/ -lIPXACTmodelsd
    LIBS += -L$$PWD/../../../executable/ -lKactusAPId
    DESTDIR = ./debug
}
else:unix {
    LIBS += -L$$PWD/../../../executable/ -lIPXACTmodels
    LIBS += -L$$PWD/../../../executable/ -lKactusAPI
    DESTDIR = ./release
}

DESTDIR = ./release

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += $$PWD/../../../
INCLUDEPATH += $$PWD/../../../executable

DEPENDPATH += $$PWD/../../../
DEPENDPATH += $$PWD/../../../executable
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_CppSourceAnalyzer.pri)
