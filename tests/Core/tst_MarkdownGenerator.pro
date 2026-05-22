#-----------------------------------------------------------------------------
# File: tst_MarkdownGenerator.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Anton Hagqvist
# Date: 11.4.2023
#
# Description:
# Qt project file for markdown generator
#-----------------------------------------------------------------------------

TEMPLATE = app

QT += core xml gui widgets testlib printsupport svg
CONFIG += c++17 testcase console

QMAKE_EXPORTED_VARIABLES += MAKE_TESTARGS
MAKE_TESTARGS.name = TESTARGS
MAKE_TESTARGS.value = "-platform offscreen"

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../executable -lIPXACTmodelsd -lKactusAPId

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_MarkdownGeneratord

} else {
    # release mode
    LIBS += \
        -L../../executable -lIPXACTmodels -lKactusAPI

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_MarkdownGenerator
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../
INCLUDEPATH += ../../KactusAPI/include

DEPENDPATH += .
DEPENDPATH += ../../

OBJECTS_DIR += $$DESTDIR
include(tst_MarkdownGenerator.pri)
