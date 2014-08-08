#-----------------------------------------------------------------------------
# File: tst_InterfaceDirectionNameSorter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 05.08.2014
#
# Description:
# Qt project file for running unit tests for class InterfaceDirectionNameSorter.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_InterfaceDirectionNameSorter

QT += core xml gui testlib
CONFIG += testcase console

win32:CONFIG(release, debug|release) {
    LIBS += -L$$PWD/../../../../executable -lIPXACTmodels
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../../../../executable -lIPXACTmodelsd
    DESTDIR = ./debug
}
else:unix {
    LIBS += -L$$PWD/../../../../executable -lIPXACTmodels
    DESTDIR = ./release
}

INCLUDEPATH += $$PWD/../../../..
INCLUDEPATH += $$DESTDIR

DEPENDPATH += $$PWD/../../../..
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_InterfaceDirectionNameSorter.pri)
