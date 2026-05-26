#-----------------------------------------------------------------------------
# File: tst_VHDLimport.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 05.09.2014
#
# Description:
# Qt project file template for running unit tests for VHDLimport.
#-----------------------------------------------------------------------------

TEMPLATE = app

QT += core xml gui widgets testlib
CONFIG += c++17 testcase

DEFINES += VHDLIMPORT_LIB

QMAKE_EXPORTED_VARIABLES += MAKE_TESTARGS
MAKE_TESTARGS.name = TESTARGS
MAKE_TESTARGS.value = "-platform offscreen"

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
    TARGET = tst_VHDLimportd

} else {
    # release mode 
    LIBS += \
        -L../../../executable -lIPXACTmodels \
        -L../../../executable -lKactusAPI
    
    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_VHDLimport
}

INCLUDEPATH += .
INCLUDEPATH += ../../../KactusAPI/include

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR
include(tst_VHDLimport.pri)
