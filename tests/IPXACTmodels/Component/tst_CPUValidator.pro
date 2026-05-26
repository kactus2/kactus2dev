#-----------------------------------------------------------------------------
# File: tst_CPUValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 18.11.2015
#
# Description:
# Qt project file template for running unit tests for Enumerated Value Validator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_CPUValidator

QT += core gui xml testlib

CONFIG += c++17 testcase console

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../../executable -lIPXACTmodelsd \
        -L../../../executable -lKactusAPId

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_CPUValidatord

} else {
    # release mode
    LIBS += \
        -L../../../executable -lIPXACTmodels \
        -L../../../executable -lKactusAPI

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_CPUValidator
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

include(tst_CPUValidator.pri)
