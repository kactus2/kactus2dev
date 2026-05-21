#-----------------------------------------------------------------------------
# File: tst_BusInterfacePortMapValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Esko Pekkarinen
# Date: 28.03.2018
#
# Description:
# Qt project file template for running unit tests for a single module.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_BusInterfacePortMapValidator

QT += core xml gui testlib
CONFIG += c++17 testcase console

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../../executable -lIPXACTmodelsd \
        -L../../../executable -lKactusAPId

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_BusInterfacePortMapValidatord

} else {
    # release mode
    LIBS += \
        -L../../../executable -lIPXACTmodels \
        -L../../../executable -lKactusAPI

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_BusInterfacePortMapValidator
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

include(tst_BusInterfacePortMapValidator.pri)
