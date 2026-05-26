#-----------------------------------------------------------------------------
# File: tst_VerilogParameterParser.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 24.7.2014
#
# Description:
# Qt project file template for running unit tests for a single module.
#-----------------------------------------------------------------------------

TEMPLATE = app

QT += core xml gui testlib
CONFIG += c++17 testcase console

INCLUDEPATH += ../../../ \
    ./GeneratedFiles \
    .

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../../executable -lIPXACTmodelsd \
        -L../../../executable -lKactusAPId

    MOC_DIR += ./GeneratedFiles/Debug
    OBJECTS_DIR += Debug
    TARGET = tst_VerilogParameterParserd

} else {
    # release mode 
    LIBS += \
        -L../../../executable -lIPXACTmodels \
        -L../../../executable -lKactusAPI
    
    MOC_DIR += ./GeneratedFiles/Release
    OBJECTS_DIR += Release
    TARGET = tst_VerilogParameterParser
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../KactusAPI/include

DEPENDPATH += .
DEPENDPATH += ../../../

include(tst_VerilogParameterParser.pri)
