#-----------------------------------------------------------------------------
# File: unittest.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 20.05.2015
#
# Description:
# Qt project file template for running unit tests for HierarchicalSaveBuildStrategy.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_HierarchicalSaveBuildStrategy

QT += core xml gui testlib widgets
CONFIG += c++17 testcase console

QMAKE_EXPORTED_VARIABLES += MAKE_TESTARGS
MAKE_TESTARGS.name = TESTARGS
MAKE_TESTARGS.value = "-platform offscreen"

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../executable -lIPXACTmodelsd

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_HierarchicalSaveBuildStrategyd

} else {
    # release mode
    LIBS += \
        -L../../executable -lIPXACTmodels

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_HierarchicalSaveBuildStrategy
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../

DEPENDPATH += .
DEPENDPATH += ../../

OBJECTS_DIR += $$DESTDIR
include(tst_HierarchicalSaveBuildStrategy.pri)
