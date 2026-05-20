#-----------------------------------------------------------------------------
# File: tst_LinuxDeviceTreeGenerator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 08.05.2018
#
# Description:
# Qt project file template for running unit tests for a LinuxDeviceTreeGenerator.
#-----------------------------------------------------------------------------

TEMPLATE = app

QT += core xml gui testlib widgets
CONFIG += c++17 testcase console

DEFINES += LINUXDEVICETREEGENERATOR_LIB

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../../executable -lIPXACTmodelsd \
        -L../../../executable -lKactusAPId

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_LinuxDeviceTreeGeneratord

} else {
    # release mode
    LIBS += \
        -L../../../executable -lIPXACTmodels \
        -L../../../executable -lKactusAPI

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_LinuxDeviceTreeGenerator
}

INCLUDEPATH += ../../../
INCLUDEPATH += ../../../executable
INCLUDEPATH += $$DESTDIR

DEPENDPATH += ../../../
DEPENDPATH += ../../../executable
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR
include(tst_LinuxDeviceTreeGenerator.pri)
