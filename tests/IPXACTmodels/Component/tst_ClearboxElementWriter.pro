#-----------------------------------------------------------------------------
# File: tst_ClearboxElementWriter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Anton Hagqvist
# Date: 10.06.2026
#
# Description:
# Unit test project for ClearboxElement writer.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ClearboxElementWriter

QT += core xml testlib
QT -= gui

CONFIG += c++17 testcase console

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../../executable -lIPXACTmodelsd

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_ClearboxElementWriterd

} else {
    # release mode
    LIBS += \
        -L../../../executable -lIPXACTmodels

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_ClearboxElementWriter
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

include(tst_ClearboxElementWriter.pri)
