#-----------------------------------------------------------------------------
# File: tst_ApiDefinitionWriter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Janne Virtanen
# Date: 11.12.2015
#
# Description:
# Qt project file template for running unit tests for ApiDefinition writer.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ApiDefinitionWriter

QT += core xml gui testlib
CONFIG += c++11 testcase console

win32:CONFIG(release, debug|release) {
    DESTDIR = ./release
	LIBS += -L$$PWD/../../../executable/ -lIPXACTmodels
}
else:win32:CONFIG(debug, debug|release) {
    DESTDIR = ./debug
	LIBS += -L$$PWD/../../../executable/ -lIPXACTmodelsd
}
else:unix {
    DESTDIR = ./release
	LIBS += -L$$PWD/../../../executable/ -lIPXACTmodels
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_ApiDefinitionWriter.pri)
