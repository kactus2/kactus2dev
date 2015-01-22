#-----------------------------------------------------------------------------
# File: tst_ParameterReferenceTree.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 21.01.2015
#
# Description:
# Qt project file template for running unit tests for a ParameterReferenceTree.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ParameterReferenceTree

QT += core xml gui testlib widgets
CONFIG += testcase console

win32:CONFIG(release, debug|release) {
    LIBS += -L$$PWD/../../../../executable/ -lIPXACTmodels
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../../../../executable/ -lIPXACTmodelsd
    DESTDIR = ./debug
}
else:unix {
    LIBS += -L$$PWD/../../../../executable/ -lIPXACTmodels
    DESTDIR = ./release
}

INCLUDEPATH += $$PWD/../../../../
INCLUDEPATH += $$DESTDIR

DEPENDPATH += $$PWD/../../../../
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_ParameterReferenceTree.pri)
