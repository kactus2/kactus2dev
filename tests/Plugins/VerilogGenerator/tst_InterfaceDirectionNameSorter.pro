#-----------------------------------------------------------------------------
# File: tst_InterfaceDirectionNameSorter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 05.08.2014
#
# Description:
# Qt project file for running unit tests for class InterfaceDirectionNameSorter.
#-----------------------------------------------------------------------------

TEMPLATE = app

QT += core xml gui testlib
CONFIG += c++17 testcase console

DEFINES += VERILOGGENERATORPLUGIN_LIB

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../../executable -lIPXACTmodelsd

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_InterfaceDirectionNameSorterd

} else {
    # release mode
    LIBS += \
        -L../../../executable -lIPXACTmodels

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_InterfaceDirectionNameSorter
}

INCLUDEPATH += ../../../
INCLUDEPATH += ../../../executable
INCLUDEPATH += .

DEPENDPATH += ../../../
DEPENDPATH += ../../../executable
DEPENDPATH += .

OBJECTS_DIR = $$DESTDIR
include(tst_InterfaceDirectionNameSorter.pri)
