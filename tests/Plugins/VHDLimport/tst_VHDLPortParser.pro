#-----------------------------------------------------------------------------
# File: tst_VHDLPortParser.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 22.09.2014
#
# Description:
# Qt project file template for running unit tests for VHDLPortParser.
#-----------------------------------------------------------------------------

TEMPLATE = app

QT += core xml testlib
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
    DESTDIR += Debug
    TARGET = tst_VHDLPortParserd

} else {
    # release mode 
    LIBS += \
        -L../../../executable -lIPXACTmodels \
        -L../../../executable -lKactusAPI
    
    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_VHDLPortParser
}

OBJECTS_DIR = $$DESTDIR

INCLUDEPATH += .
INCLUDEPATH += ../../../KactusAPI/include

DEPENDPATH += .
DEPENDPATH += ../../../

include(tst_VHDLPortParser.pri)
