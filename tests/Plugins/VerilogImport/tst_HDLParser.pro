#-----------------------------------------------------------------------------
# File: unittest.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Janne Virtanen
# Date: 31.8.2016
#
# Description:
# Qt project file for running unit tests for HDL Parser.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_HDLParser

QT += core xml testlib
CONFIG += c++11 testcase console

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

INCLUDEPATH += $$PWD/../../../
INCLUDEPATH += $$PWD/../../../executable
INCLUDEPATH += $$PWD/../../../executable/Plugins
INCLUDEPATH += $$PWD/../../../KactusAPI/include
INCLUDEPATH += $$DESTDIR

DEPENDPATH += $$PWD/../../../
DEPENDPATH += $$PWD/../../../executable
DEPENDPATH += $$PWD/../../../executable/Plugins
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_HDLParser.pri)
