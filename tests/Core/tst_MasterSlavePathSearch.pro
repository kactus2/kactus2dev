#-----------------------------------------------------------------------------
# File: tst_MasterSlavePathSearch.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Mikko Teuho
# Date: 26.03.2018
#
# Description:
# Qt project file for running unit tests in memory designer.
#-----------------------------------------------------------------------------

TEMPLATE = app

QT += core xml gui testlib
CONFIG += c++17 testcase console

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../executable -lIPXACTmodelsd

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_MasterSlavePathSearchd

} else {
    # release mode
    LIBS += \
        -L../../executable -lIPXACTmodels

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_MasterSlavePathSearch
}

INCLUDEPATH += $$PWD/../../
INCLUDEPATH += $$DESTDIR

DEPENDPATH += $$PWD/../../
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR
include(tst_MasterSlavePathSearch.pri)
