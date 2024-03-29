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

TARGET = tst_AddressBlockGraphItem

DEFINES+=KACTUS2_EXPORTS

QT += core xml gui testlib widgets
CONFIG += c++11 testcase console

QMAKE_EXPORTED_VARIABLES += MAKE_TESTARGS
MAKE_TESTARGS.name = TESTARGS
MAKE_TESTARGS.value = "-platform offscreen"

win32:CONFIG(release, debug|release) {
    LIBS += -L$$PWD/../../executable/ -lIPXACTmodels -lKactusAPI
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../../executable/ -lIPXACTmodelsd -lKactusAPId
    DESTDIR = ./debug
}
else:unix {
    LIBS += -L$$PWD/../../executable/ -lIPXACTmodels -lKactusAPI
    DESTDIR = ./release
}

INCLUDEPATH += $$PWD/../../
INCLUDEPATH += $$PWD/../../executable
INCLUDEPATH += $$DESTDIR

DEPENDPATH += $$PWD/../../
DEPENDPATH += $$PWD/../../executable
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_AddressBlockGraphItem.pri)
