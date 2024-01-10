#-----------------------------------------------------------------------------
# File: tst_PowerDomainReader.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Esko Pekkarinen
# Date: 04.10.2023
#
# Description:
# Qt project file for running unit tests for PowerDomain reader.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_PowerDomainReader

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

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles

include(tst_PowerDomainReader.pri)
