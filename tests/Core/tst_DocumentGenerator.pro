#-----------------------------------------------------------------------------
# File: unittest.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 24.7.2014
#
# Description:
# Qt project file template for running unit tests for document generator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_DocumentGenerator

QT += core xml gui widgets testlib printsupport svg
CONFIG += c++11 testcase console

QMAKE_EXPORTED_VARIABLES += MAKE_TESTARGS
MAKE_TESTARGS.name = TESTARGS
MAKE_TESTARGS.value = "-platform offscreen"

win32:CONFIG(release, debug|release) {
    LIBS += -L$$PWD/../../executable/ -lIPXACTmodels
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../../executable/ -lIPXACTmodelsd
    DESTDIR = ./debug
}
else:unix {
    LIBS += -L$$PWD/../../executable/ -lIPXACTmodels
    DESTDIR = ./release
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../

DEPENDPATH += .
DEPENDPATH += ../../

OBJECTS_DIR += $$DESTDIR


MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_DocumentGenerator.pri)
