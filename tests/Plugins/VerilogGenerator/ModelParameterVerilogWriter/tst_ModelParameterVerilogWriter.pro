#-----------------------------------------------------------------------------
# File: tst_ModelParameterVerilogWriter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 24.27.2014
#
# Description:
# Qt project file for running unit tests for VerilogParameter.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ModelParameterVerilogWriter

QT += core xml gui testlib
CONFIG += testcase console debug

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
include(tst_ModelParameterVerilogWriter.pri)
