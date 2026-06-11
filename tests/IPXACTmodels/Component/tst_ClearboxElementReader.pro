#-----------------------------------------------------------------------------
# File: tst_ClearboxElementReader.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Anton Hagqvist
# Date: 10.06.2026
#
# Description:
# Unit test project for ClearboxElement reader.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ClearboxElementReader

QT += core xml testlib
QT -= gui

CONFIG += c++17 testcase console

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../../executable -lIPXACTmodelsd

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_ClearboxElementReaderd

} else {
    # release mode
    LIBS += \
        -L../../../executable -lIPXACTmodels

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_ClearboxElementReader
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

include(tst_ClearboxElementReader.pri)
