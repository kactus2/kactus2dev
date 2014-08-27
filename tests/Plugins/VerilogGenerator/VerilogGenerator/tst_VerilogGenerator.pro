#-----------------------------------------------------------------------------
# File: unittest.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 24.27.2014
#
# Description:
# Qt project file for running unit tests for Verilog Generator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_VerilogGenerator

QT += core xml gui testlib
CONFIG += testcase console

LIBS += -L$$PWD/../../../../executable/Plugins -lVerilogGeneratorPlugin

win32:CONFIG(release, debug|release) {
	LIBS += -L$$PWD/../../../../executable/ -lKactus2
    LIBS += -L$$PWD/../../../../executable/ -lIPXACTmodels
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
	LIBS += -L$$PWD/../../../../executable/ -lKactus2_d
    LIBS += -L$$PWD/../../../../executable/ -lIPXACTmodelsd
    DESTDIR = ./debug
}
else:unix {
	LIBS += -L$$PWD/../../../../executable/ -lKactus2
    LIBS += -L$$PWD/../../../../executable/ -lIPXACTmodels
    DESTDIR = ./release
}

INCLUDEPATH += $$PWD/../../../../
INCLUDEPATH += $$PWD/../../../../executable
INCLUDEPATH += $$PWD/../../../../executable/Plugins
INCLUDEPATH += $$DESTDIR

DEPENDPATH += $$PWD/../../../../
DEPENDPATH += $$PWD/../../../../executable
DEPENDPATH += $$PWD/../../../../executable/Plugins
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_VerilogGenerator.pri)
