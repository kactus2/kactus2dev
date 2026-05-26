#-----------------------------------------------------------------------------
# File: tst_AddressSpaceReader.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 30.09.2015
#
# Description:
# Qt project file template for running unit tests for AddressSpace reader.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_AddressSpaceReader

QT += core xml testlib
QT -= gui
CONFIG += c++17 testcase console

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../../executable -lIPXACTmodelsd

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_AddressSpaceReaderd

} else {
    # release mode
    LIBS += \
        -L../../../executable -lIPXACTmodels

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_AddressSpaceReader
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

include(tst_AddressSpaceReader.pri)
