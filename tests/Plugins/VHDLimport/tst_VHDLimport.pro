#-----------------------------------------------------------------------------
# File: tst_VHDLimport.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 05.09.2014
#
# Description:
# Qt project file template for running unit tests for VHDLimport.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_VHDLimport

QT += core xml gui widgets testlib
CONFIG += c++11 testcase

DEFINES += VHDLIMPORT_LIB

QMAKE_EXPORTED_VARIABLES += MAKE_TESTARGS
MAKE_TESTARGS.name = TESTARGS
MAKE_TESTARGS.value = "-platform offscreen"

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
include(tst_VHDLimport.pri)
