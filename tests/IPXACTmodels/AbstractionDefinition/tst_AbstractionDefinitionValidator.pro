#-----------------------------------------------------------------------------
# File: tst_AbstractionDefinitionValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 04.12.2015
#
# Description:
# Qt project file for running unit tests for AbstractionDefinition Validator.
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
    TARGET = tst_AbstractionDefinitionValidatord

} else {
    # release mode
    LIBS += \
        -L../../../executable -lIPXACTmodels -lKactusAPI

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_AbstractionDefinitionValidator
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR
include(tst_AbstractionDefinitionValidator.pri)
