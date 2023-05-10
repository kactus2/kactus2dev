#-----------------------------------------------------------------------------
# File: tst_MemoryViewGenerator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 25.4.2016
#
# Description:
# Qt project file template for running unit tests for a MemoryViewGenerator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_MemoryViewGenerator

QT += core xml testlib widgets
CONFIG += c++11 testcase console

DEFINES += MEMORYVIEWGENERATOR_LIB

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

INCLUDEPATH += $$PWD/../../../
INCLUDEPATH += $$PWD/../../../executable
INCLUDEPATH += $$PWD/../../../KactusAPI/include
INCLUDEPATH += $$DESTDIR

DEPENDPATH += $$PWD/../../../
DEPENDPATH += $$PWD/../../../executable
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_MemoryViewGenerator.pri)
