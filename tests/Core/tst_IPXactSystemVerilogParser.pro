#-----------------------------------------------------------------------------
# File: tst_IPXactSystemVerilogParser.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 02.12.2014
#
# Description:
# Qt project file template for running unit tests for IPXactSystemVerilogParser.
#-----------------------------------------------------------------------------

TEMPLATE = app

QT += core xml testlib
QT -= gui widgets
CONFIG += c++17 testcase console

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../executable -lIPXACTmodelsd -lKactusAPId

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_IPXactSystemVerilogParserd

} else {
    # release mode
    LIBS += \
        -L../../executable -lIPXACTmodels -lKactusAPI

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_IPXactSystemVerilogParserd
}

INCLUDEPATH += $$PWD/../../
INCLUDEPATH += $$PWD/../../executable
INCLUDEPATH += $$DESTDIR

DEPENDPATH += $$PWD/../../
DEPENDPATH += $$PWD/../../executable
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR
include(tst_IPXactSystemVerilogParser.pri)
