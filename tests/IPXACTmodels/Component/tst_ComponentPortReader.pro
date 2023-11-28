#-----------------------------------------------------------------------------
# File: tst_ComponentPortReader.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 14.09.2015
#
# Description:
# Qt project file template for running unit tests for component port reader.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ComponentPortReader

QT += core xml testlib
QT -= gui

CONFIG += c++17 testcase console

linux-g++ | linux-g++-64 | linux-g++-32 {
 LIBS += -L../../../executable \
     -lIPXACTmodels

}
win64 | win32 {
 LIBS += -L../../../executable \
     -lIPXACTmodelsd
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_ComponentPortReader.pri)
