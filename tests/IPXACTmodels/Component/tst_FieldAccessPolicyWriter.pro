#-----------------------------------------------------------------------------
# File: tst_FieldAccessPolicyWriter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Anton Hagqvist
# Date: 2.8s.2023
#
# Description:
# Qt project file template for running unit tests for FieldAccessPolicyWriter.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_FieldAccessPolicyWriter

QT += core xml testlib
QT -= gui

CONFIG += c++17 testcase console

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../../executable -lIPXACTmodelsd

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_FieldAccessPolicyWriterd

} else {
    # release mode
    LIBS += \
        -L../../../executable -lIPXACTmodels

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_FieldAccessPolicyWriter
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

include(tst_FieldAccessPolicyWriter.pri)
