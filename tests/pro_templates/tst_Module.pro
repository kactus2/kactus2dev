#-----------------------------------------------------------------------------
# File: tst_<Module>.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Esko Pekkarinen
# Date: 28.03.2018
#
# Description:
# Qt project file template for running unit tests for a single module.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_<Module>

QT += core xml gui testlib
CONFIG += c++11 testcase console

# Remove the following, if IP-XACT models library is not needed.
linux-g++ | linux-g++-64 | linux-g++-32 {
 LIBS += -L../../../x64/executable \
     -lIPXACTmodels 
}
win64 | win32 {
 LIBS += -L../../../x64/executable \
     -lIPXACTmodelsd
}

DESTDIR = ./release

INCLUDEPATH += $$DESTDIR

DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_<Module>.pri)
