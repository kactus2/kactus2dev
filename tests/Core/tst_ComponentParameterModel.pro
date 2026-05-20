#-----------------------------------------------------------------------------
# File: tst_ComponentParameterModel.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 24.7.2014
#
# Description:
# Qt project file template for running unit tests for ComponentParameterModel.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ComponentParameterModel

QT += core gui xml testlib widgets

CONFIG += c++17 testcase console
DEFINES += IPXACTMODELS_LIB

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../executable -lIPXACTmodelsd -lKactusAPId

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_ComponentParameterModeld

} else {
    # release mode
    LIBS += \
        -L../../executable -lIPXACTmodels -lKactusAPI

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_ComponentParameterModel
}

INCLUDEPATH += $$PWD/../../
INCLUDEPATH += $$DESTDIR

DEPENDPATH += $$PWD/../../
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR
include(tst_ComponentParameterModel.pri)
