#-----------------------------------------------------------------------------
# File: tst_MemoryMapHeaderGenerator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 05.05.2015
#
# Description:
# Qt project file for running unit tests for memory map header generator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_MemoryMapHeaderGenerator

QT += core xml gui testlib widgets
CONFIG += testcase console

DEFINES += MEMORYMAPHEADERGENERATOR_LIB KACTUS2_EXPORTS

win32:CONFIG(release, debug|release) {
    LIBS += -L$$PWD/../../../executable/ -lIPXACTmodels
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../../../executable/ -lIPXACTmodelsd
    DESTDIR = ./debug
}
else:unix {
    LIBS += -L$$PWD/../../../executable/ -lIPXACTmodels
    DESTDIR = ./release
}

INCLUDEPATH += $$PWD/../../../
INCLUDEPATH += $$PWD/../../../executable
INCLUDEPATH += $$PWD/../../../executable/Plugins
INCLUDEPATH += $$PWD/../../../executable/Plugins/MemoryMapHeaderGenerator
INCLUDEPATH += $$DESTDIR

DEPENDPATH += $$PWD/../../../
DEPENDPATH += $$PWD/../../../executable
DEPENDPATH += $$PWD/../../../executable/Plugins
DEPENDPATH += $$PWD/../../../executable/Plugins/MemoryMapHeaderGenerator
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_MemoryMapHeaderGenerator.pri)
