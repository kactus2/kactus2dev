#-----------------------------------------------------------------------------
# File: tst_FieldGraphItem.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 28.04.2015
#
# Description:
# Qt project file template for running unit tests for AddressBlockGraphItem.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_FieldGraphItem

DEFINES+=KACTUS2_EXPORTS

QT += core xml gui testlib widgets
CONFIG += testcase console

win32:CONFIG(release, debug|release) {
    LIBS += -L$$PWD/../../../../../../executable/ -lIPXACTmodels
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../../../../../../executable/ -lIPXACTmodelsd
    DESTDIR = ./debug
}
else:unix {
    LIBS += -L$$PWD/../../../../../../executable/ -lIPXACTmodels
    DESTDIR = ./release
}

INCLUDEPATH += $$PWD/../../../../../../
INCLUDEPATH += $$PWD/../../../../../../executable
INCLUDEPATH += $$DESTDIR

DEPENDPATH += $$PWD/../../../../../../
DEPENDPATH += $$PWD/../../../../../../executable
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_FieldGraphItem.pri)
