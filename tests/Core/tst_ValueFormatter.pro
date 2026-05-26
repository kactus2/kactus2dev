#-----------------------------------------------------------------------------
# File: tst_ValueFormatter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 05.03.2015
#
# Description:
# Qt project file template for running unit tests for ValueFormatter.
#-----------------------------------------------------------------------------

TEMPLATE = app

QT += core xml gui testlib
CONFIG += c++17 testcase console
DEFINES += IPXACTMODELS_LIB

CONFIG(debug, debug|release) {
    # debug mode
    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_ValueFormatterd

} else {
    # release mode
    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_ValueFormatter
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../

DEPENDPATH += .
DEPENDPATH += ../../

OBJECTS_DIR += $$DESTDIR
include(tst_ValueFormatter.pri)
