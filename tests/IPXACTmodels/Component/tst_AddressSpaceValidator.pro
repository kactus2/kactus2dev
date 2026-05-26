#-----------------------------------------------------------------------------
# File: tst_AddressSpaceValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 30.11.2015
#
# Description:
# Qt project file template for running unit tests for AddressSpaceValidator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_AddressSpaceValidator

QT += core gui xml testlib

CONFIG += c++17 testcase console

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../../executable -lIPXACTmodelsd \
        -L../../../executable -lKactusAPId

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_AddressSpaceValidatord

} else {
    # release mode
    LIBS += \
        -L../../../executable -lIPXACTmodels \
        -L../../../executable -lKactusAPI

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_AddressSpaceValidator
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../
INCLUDEPATH += ../../../KactusAPI/include

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

include(tst_AddressSpaceValidator.pri)
