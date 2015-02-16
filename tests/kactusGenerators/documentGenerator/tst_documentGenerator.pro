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

TARGET = tst_documentGenerator

QT += core xml gui widgets testlib printsupport
CONFIG += testcase console

win32:CONFIG(release, debug|release) {
    LIBS += -L$$PWD/../../../executable/ -lIPXACTmodels
	LIBS += -L$$PWD/../../../executable/ -lKactus2
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../../../executable/ -lIPXACTmodelsd
	LIBS += -L$$PWD/../../../executable/ -lKactus2_d
    DESTDIR = ./debug
}
else:unix {
    LIBS += -L$$PWD/../../../executable/ -lIPXACTmodels
	LIBS += -L$$PWD/../../../executable/ -lKactus2
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
include(tst_documentGenerator.pri)
