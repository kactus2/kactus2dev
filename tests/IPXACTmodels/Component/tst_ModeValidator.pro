#-----------------------------------------------------------------------------
# File: tst_ModeValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Esko Pekkarinen
# Date: 14.08.2023
#
# Description:
# Qt project file template for running unit tests for Mode Validator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ModeValidator

QT += core gui xml testlib

CONFIG += c++17 testcase console

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../../executable -lIPXACTmodelsd \
        -L../../../executable -lKactusAPId

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_ModeValidatord

} else {
    # release mode
    LIBS += \
        -L../../../executable -lIPXACTmodels \
        -L../../../executable -lKactusAPI

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_ModeValidator
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

include(tst_ModeValidator.pri)
