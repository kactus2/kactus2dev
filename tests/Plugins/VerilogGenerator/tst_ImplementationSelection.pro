#-----------------------------------------------------------------------------
# File: tst_ImplementationSelection.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Janne Virtanen
# Date: 19.01.2017
#
# Description:
# Qt project file for running unit tests for Verilog implementation selection.
#-----------------------------------------------------------------------------

TEMPLATE = app

QT += core xml gui testlib
CONFIG += c++17 testcase console

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../../executable -lIPXACTmodelsd

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_ImplementationSelectiond

} else {
    # release mode
    LIBS += \
        -L../../../executable -lIPXACTmodels

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_ImplementationSelection
}

INCLUDEPATH += ../../../
INCLUDEPATH += ../../../executable
INCLUDEPATH += ../../../executable/Plugins
INCLUDEPATH += .

DEPENDPATH += ../../../
DEPENDPATH += ../../../executable
DEPENDPATH += ../../../executable/Plugins
DEPENDPATH += .

OBJECTS_DIR = $$DESTDIR
include(tst_ImplementationSelection.pri)
