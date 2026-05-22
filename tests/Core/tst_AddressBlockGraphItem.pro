#-----------------------------------------------------------------------------
# File: tst_AddressBlockGraphItem.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 20.03.2015
#
# Description:
# Qt project file template for running unit tests for AddressBlockGraphItem.
#-----------------------------------------------------------------------------

TEMPLATE = app

DEFINES+=KACTUS2_EXPORTS

QT += core xml gui testlib widgets
CONFIG += c++17 testcase console

QMAKE_EXPORTED_VARIABLES += MAKE_TESTARGS
MAKE_TESTARGS.name = TESTARGS
MAKE_TESTARGS.value = "-platform offscreen"

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../executable -lIPXACTmodelsd -lKactusAPId

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_AddressBlockGraphItemd

} else {
    # release mode
    LIBS += \
        -L../../executable -lIPXACTmodels -lKactusAPI

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_AddressBlockGraphItem
}

INCLUDEPATH += $$PWD/../../
INCLUDEPATH += $$PWD/../../executable
INCLUDEPATH += $$DESTDIR

DEPENDPATH += $$PWD/../../
DEPENDPATH += $$PWD/../../executable
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

include(tst_AddressBlockGraphItem.pri)
