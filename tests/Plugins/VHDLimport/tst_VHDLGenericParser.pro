#-----------------------------------------------------------------------------
# File: tst_VHDLGenericParser.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 22.09.2014
#
# Description:
# Qt project file template for running unit tests for VHDLGenericParser.
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
    DESTDIR += Debug
    TARGET = tst_VHDLGenericParserd

} else {
    # release mode 
    LIBS += \
        -L../../../executable -lIPXACTmodels \
        -L../../../executable -lKactusAPI
    
    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_VHDLGenericParser
}

INCLUDEPATH += .
INCLUDEPATH += ../../../
INCLUDEPATH += ../../../KactusAPI/include

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR = $$DESTDIR
include(tst_VHDLGenericParser.pri)
