#-----------------------------------------------------------------------------
# File: tst_CppSourceAnalyzer.pro
#-----------------------------------------------------------------------------
# Project: Kactus2
# Author: Anton Hagqvist
# Date: 11.06.2024
#
# Description:
# Qt project file template for running unit tests for CppSourceAnalyzer.
#-----------------------------------------------------------------------------

TEMPLATE = app

QT += core xml gui testlib widgets
CONFIG += c++17 testcase console

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../../executable -lIPXACTmodelsd -lKactusAPId

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_CppSourceAnalyzerd

} else {
    # release mode
    LIBS += \
        -L../../../executable -lIPXACTmodels -lKactusAPI

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_CppSourceAnalyzer
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../
INCLUDEPATH += ../../../executable

DEPENDPATH += ../../../
DEPENDPATH += ../../../executable
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

include(tst_CppSourceAnalyzer.pri)
