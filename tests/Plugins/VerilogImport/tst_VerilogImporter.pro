#-----------------------------------------------------------------------------
# File: tst_VerilogImporter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 23.09.2014
#
# Description:
# Qt project file template for running unit tests for VerilogImporter.
#-----------------------------------------------------------------------------

TEMPLATE = app

QT += core xml gui widgets testlib
CONFIG += c++17 testcase

DEFINES += VERILOGIMPORT_LIB

QMAKE_EXPORTED_VARIABLES += MAKE_TESTARGS
MAKE_TESTARGS.name = TESTARGS
MAKE_TESTARGS.value = "-platform offscreen"

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../../executable -lIPXACTmodelsd \
        -L../../../executable -lKactusAPId

    MOC_DIR += ./GeneratedFiles/Debug
    OBJECTS_DIR += Debug
    TARGET = tst_VerilogImporterd

} else {
    # release mode
    LIBS += \
        -L../../../executable -lIPXACTmodels \
        -L../../../executable -lKactusAPI

    MOC_DIR += ./GeneratedFiles/Release
    OBJECTS_DIR += Release
    TARGET = tst_VerilogImporter
}

INCLUDEPATH += .
INCLUDEPATH += $$PWD/../../../KactusAPI/include/
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

include(tst_VerilogImporter.pri)
