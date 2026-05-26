#-----------------------------------------------------------------------------
# File: tst_OtherClockDriverWriter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 02.10.2015
#
# Description:
# Qt project file template for running unit tests for OtherClockDriver writer.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_OtherClockDriverWriter

QT += core xml testlib
QT -= gui

CONFIG += c++17 testcase console

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../../executable -lIPXACTmodelsd

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_OtherClockDriverWriterd

} else {
    # release mode
    LIBS += \
        -L../../../executable -lIPXACTmodels

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_OtherClockDriverWriter
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

include(tst_OtherClockDriverWriter.pri)
