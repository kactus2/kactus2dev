#-----------------------------------------------------------------------------
# File: tst_VerilogSourceAnalyzer.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 07.04.2015
#
# Description:
# Qt project file for running unit tests for VerilogSourceAnalyzer.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_VerilogSourceAnalyzer

DEFINES += VERILOGSOURCEANALYZER_LIB

QT += core xml gui testlib widgets
CONFIG += c++11 testcase console

QMAKE_EXPORTED_VARIABLES += MAKE_TESTARGS
MAKE_TESTARGS.name = TESTARGS
MAKE_TESTARGS.value = "-platform offscreen"

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

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../
INCLUDEPATH += $$PWD/../../../KactusAPI/include

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_VerilogSourceAnalyzer.pri)
