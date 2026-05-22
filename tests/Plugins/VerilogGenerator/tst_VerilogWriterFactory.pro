#-----------------------------------------------------------------------------
# File: unittest.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Janne Virtanen
# Date: 26.01.2017
#
# Description:
# Qt project file for running unit tests for VerilogWriterFactory.
#-----------------------------------------------------------------------------

TEMPLATE = app

QT += core xml gui testlib
CONFIG += c++17 testcase console

DEFINES += VERILOGGENERATORPLUGIN_LIB

CONFIG (debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../../executable -lIPXACTmodelsd \
        -L../../../executable -lKactusAPId

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_VerilogWriterFactoryd

} else {
    # release mode
    LIBS += \
        -L../../../executable -lIPXACTmodels \
        -L../../../executable -lKactusAPI

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_VerilogWriterFactory
}

INCLUDEPATH += ../../../
INCLUDEPATH += ../../../executable
INCLUDEPATH += ../../../executable/Plugins
INCLUDEPATH += ../../../KactusAPI/include
INCLUDEPATH += .

DEPENDPATH += ../../../
DEPENDPATH += ../../../executable
DEPENDPATH += ../../../executable/Plugins
DEPENDPATH += .

OBJECTS_DIR = $$DESTDIR

include(tst_VerilogWriterFactory.pri)
