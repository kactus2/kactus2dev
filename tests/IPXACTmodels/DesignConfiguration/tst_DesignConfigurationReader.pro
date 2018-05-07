#-----------------------------------------------------------------------------
# File: DesignConfigurationReader.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 11.08.2015
#
# Description:
# Qt project file for running unit tests for a designConfigurationReader.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_DesignConfigurationReader

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
include(tst_DesignConfigurationReader.pri)
