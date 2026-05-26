#-----------------------------------------------------------------------------
# File: tst_IndirectInterfaceWriter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 07.08.2017
#
# Description:
# Qt project file template for running unit tests for a single module.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_IndirectInterfaceWriter

QT += core xml testlib
QT -= gui

CONFIG += c++17 testcase console

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../../executable -lIPXACTmodelsd

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_IndirectInterfaceWriterd

} else {
    # release mode
    LIBS += \
        -L../../../executable -lIPXACTmodels

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_IndirectInterfaceWriter
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

include(tst_IndirectInterfaceWriter.pri)
