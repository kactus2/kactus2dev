#-----------------------------------------------------------------------------
# File: unittest.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Janne Virtanen
# Date: 31.8.2016
#
# Description:
# Qt project file for running unit tests for HDL Parser.
#-----------------------------------------------------------------------------

TEMPLATE = app

QT += core xml testlib
CONFIG += c++17 testcase console

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../../executable -lIPXACTmodelsd \
        -L../../../executable -lKactusAPId

    MOC_DIR += ./GeneratedFiles/Debug
    OBJECTS_DIR += Debug
    TARGET = tst_HDLParserd

} else {
    # release mode
    LIBS += \
        -L../../../executable -lIPXACTmodels \
        -L../../../executable -lKactusAPI

    MOC_DIR += ./GeneratedFiles/Release
    OBJECTS_DIR += Release
    TARGET = tst_HDLParser
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

include(tst_HDLParser.pri)
