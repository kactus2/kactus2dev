#-----------------------------------------------------------------------------
# File: tst_ConstraintSetReader.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Anton Hagqvist
# Date: 15.06.2026
#
# Description:
# Unit test project file for ConstraintSetReader.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_ConstraintSetReader

QT += core xml testlib
QT -= gui

CONFIG += c++17 testcase console

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../../executable -lIPXACTmodelsd

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_ConstraintSetReaderd

} else {
    # release mode
    LIBS += \
        -L../../../executable -lIPXACTmodels

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_ConstraintSetReader
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

OBJECTS_DIR += $$DESTDIR

include(tst_ConstraintSetReader.pri)
