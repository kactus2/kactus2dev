#-----------------------------------------------------------------------------
# File: tst_VLNVComparator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 17.10.2014
#
# Description:
# Qt project file template for running unit tests for a VLNVComparator.
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
    TARGET = tst_VLNVComparatord

} else {
    # release mode
    LIBS += \
        -L../../executable -lIPXACTmodels

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_VLNVComparator
}

INCLUDEPATH += ../../
INCLUDEPATH += ../../executable
INCLUDEPATH += ../../executable/Plugins
INCLUDEPATH += $$DESTDIR

DEPENDPATH += ../../
DEPENDPATH += ../../executable
DEPENDPATH += ../../executable/Plugins

OBJECTS_DIR += $$DESTDIR
include(tst_VLNVComparator.pri)
