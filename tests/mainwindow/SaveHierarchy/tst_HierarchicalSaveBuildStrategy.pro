#-----------------------------------------------------------------------------
# File: unittest.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 20.05.2015
#
# Description:
# Qt project file template for running unit tests for HierarchicalSaveBuildStrategy.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_HierarchicalSaveBuildStrategy

QT += core xml gui testlib widgets
CONFIG += testcase console

win32:CONFIG(release, debug|release) {
    LIBS += -L$$PWD/../../../executable/ -lIPXACTmodels
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../../../executable/ -lIPXACTmodelsd
    DESTDIR = ./debug
}
else:unix {
    LIBS += -L$$PWD/../../../executable/ -lIPXACTmodels
    DESTDIR = ./release
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_HierarchicalSaveBuildStrategy.pri)
