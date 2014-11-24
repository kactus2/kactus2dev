#-----------------------------------------------------------------------------
# File: tst_ParameterValidator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 13.11.2014
#
# Description:
# Qt project file template for running unit tests for ParameterValidator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ParameterValidator

QT += core xml gui testlib
CONFIG += testcase console

DEFINES += IPXACTMODELS_LIB

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

INCLUDEPATH += $$PWD/../../
INCLUDEPATH += $$PWD/../../executable
INCLUDEPATH += $$PWD/../../executable/Plugins
INCLUDEPATH += $$DESTDIR

DEPENDPATH += $$PWD/../../
DEPENDPATH += $$PWD/../../executable
DEPENDPATH += $$PWD/../../executable/Plugins
DEPENDPATH += .
INCLUDEPATH += $$DESTDIR

DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_ParameterValidator.pri)
