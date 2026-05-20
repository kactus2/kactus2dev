#-----------------------------------------------------------------------------
# File: tst_BusDefinitionValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 17.11.2015
#
# Description:
# Qt project file template for running unit tests for BusDefinitionValidator.
#-----------------------------------------------------------------------------

TEMPLATE = app

QT += core gui xml testlib

CONFIG += c++17 testcase console

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../../executable -lIPXACTmodelsd -lKactusAPId

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_BusDefinitionValidatord

} else {
    # release mode
    LIBS += \
        -L../../../executable -lIPXACTmodels -lKactusAPId

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_BusDefinitionValidator
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR
include(tst_BusDefinitionValidator.pri)
