#-----------------------------------------------------------------------------
# File: tst_ComponentParameterModel.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 24.7.2014
#
# Description:
# Qt project file template for running unit tests for ComponentParameterModel.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ComponentParameterModel

QT += core gui xml testlib

CONFIG += c++11 testcase console
DEFINES += IPXACTMODELS_LIB

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
INCLUDEPATH += $$DESTDIR

DEPENDPATH += $$PWD/../../
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_ComponentParameterModel.pri)
