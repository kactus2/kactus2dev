#-----------------------------------------------------------------------------
# File: tst_VerilogHeaderWriter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 04.08.2014
#
# Description:
# Qt project file for running unit tests for VerilogHeaderWriter.
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
    TARGET = tst_VerilogHeaderWriterd

} else {
    # release mode
    LIBS += \
        -L../../../executable -lIPXACTmodels

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_VerilogHeaderWriter
}

INCLUDEPATH += ../../../
INCLUDEPATH += .

DEPENDPATH += ../../../
DEPENDPATH += .

OBJECTS_DIR = $$DESTDIR

include(tst_VerilogHeaderWriter.pri)
