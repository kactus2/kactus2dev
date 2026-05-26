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

QT += core xml gui testlib widgets
CONFIG += c++17 testcase console

DEFINES += MEMORYMAPHEADERGENERATOR_LIB

QMAKE_EXPORTED_VARIABLES += MAKE_TESTARGS
MAKE_TESTARGS.name = TESTARGS
MAKE_TESTARGS.value = "-platform offscreen"

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../../executable -lIPXACTmodelsd \
        -L../../../executable -lKactusAPId

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_MemoryMapHeaderGeneratord

} else {
    # release mode
    LIBS += \
        -L../../../executable -lIPXACTmodels \
        -L../../../executable -lKactusAPI

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_MemoryMapHeaderGenerator
}

INCLUDEPATH += ../../../
INCLUDEPATH += ../../../executable
INCLUDEPATH += ../../../executable/Plugins
INCLUDEPATH += ../../../executable/Plugins/MemoryMapHeaderGenerator
INCLUDEPATH += ../../../KactusAPI/include
INCLUDEPATH += $$DESTDIR

DEPENDPATH += ../../../
DEPENDPATH += ../../../executable
DEPENDPATH += ../../../executable/Plugins
DEPENDPATH += ../../../executable/Plugins/MemoryMapHeaderGenerator
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

include(tst_MemoryMapHeaderGenerator.pri)
