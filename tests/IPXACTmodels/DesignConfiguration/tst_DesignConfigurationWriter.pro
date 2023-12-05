#-----------------------------------------------------------------------------
# File: DesignConfigurationWriter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 3.8.2015
#
# Description:
# Qt project file for running unit tests for design configuration writer.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_DesignConfigurationWriter

QT += core xml testlib
QT -= gui

CONFIG += c++11 testcase console

linux-g++ | linux-g++-64 | linux-g++-32 {
 LIBS += -L../../../executable \
     -lIPXACTmodels

}
win64 | win32 {
 LIBS += -L../../../executable \
     -lIPXACTmodelsd
}

DESTDIR = ./release

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_DesignConfigurationWriter.pri)
