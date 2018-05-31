#-----------------------------------------------------------------------------
# File: tst_LinuxDeviceTreeGenerator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 08.05.2018
#
# Description:
# Qt project file template for running unit tests for a LinuxDeviceTreeGenerator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_LinuxDeviceTreeGenerator

QT += core xml gui testlib widgets
CONFIG += c++11 testcase console

DEFINES += LINUXDEVICETREEGENERATOR_LIB

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

INCLUDEPATH += $$PWD/../../../
INCLUDEPATH += $$PWD/../../../executable
INCLUDEPATH += $$DESTDIR

DEPENDPATH += $$PWD/../../../
DEPENDPATH += $$PWD/../../../executable
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_LinuxDeviceTreeGenerator.pri)
