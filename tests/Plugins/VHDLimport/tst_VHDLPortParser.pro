#-----------------------------------------------------------------------------
# File: tst_VHDLPortParser.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 22.09.2014
#
# Description:
# Qt project file template for running unit tests for VHDLPortParser.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_VHDLPortParser

QT += core xml testlib
CONFIG += c++11 testcase console

win32:CONFIG(release, debug|release) {
    LIBS += -L$$PWD/../../../x64/executable/ -lIPXACTmodels
    LIBS += -L$$PWD/../../../x64/executable/ -lKactusAPI
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../../../x64/executable/ -lIPXACTmodelsd
    LIBS += -L$$PWD/../../../x64/executable/ -lKactusAPId
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
include(tst_VHDLPortParser.pri)
