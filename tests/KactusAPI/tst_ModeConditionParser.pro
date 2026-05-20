#-----------------------------------------------------------------------------
# File: tst_ModeConditionParser.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Esko Pekkarinen
# Date: 16.10.2023
#
# Description:
# Qt project file for running unit tests for ModeConditionParser.
#-----------------------------------------------------------------------------

TEMPLATE = app

QT += core xml testlib
QT -= gui widgets
CONFIG += c++17 testcase console

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../executable -lIPXACTmodelsd \
        -L../../executable -lKactusAPId

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_ModeConditionParserd

} else {
    # release mode
    LIBS += \
        -L../../executable -lIPXACTmodels \
        -L../../executable -lKactusAPI

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_ModeConditionParser
}

INCLUDEPATH += ../../
INCLUDEPATH += ../../executable
INCLUDEPATH += $$DESTDIR

DEPENDPATH += ../../
DEPENDPATH += ../../executable
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR
include(tst_ModeConditionParser.pri)
