#-----------------------------------------------------------------------------
# File: tst_ViewComparator.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 24.10.2014
#
# Description:
# Qt project file template for running unit tests for a ComponentComparator.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ViewComparator

QT += core xml gui testlib
CONFIG += c++17 testcase console

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../executable -lIPXACTmodelsd

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_ViewComparatord

} else {
    # release mode
    LIBS += \
        -L../../executable -lIPXACTmodels

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_ViewComparator
}

INCLUDEPATH += ../../
INCLUDEPATH += ../../executable
INCLUDEPATH += $$DESTDIR

DEPENDPATH += ../../
DEPENDPATH += ../../executable

OBJECTS_DIR += $$DESTDIR

include(tst_ViewComparator.pri)
