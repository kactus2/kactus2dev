#-----------------------------------------------------------------------------
# File: tst_SystemVerilogExpressionParser.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 25.11.2014
#
# Description:
# Qt project file template for running unit tests for SystemVerilogExpressionParser.
#-----------------------------------------------------------------------------

TEMPLATE = app

QT += core xml gui testlib
CONFIG += c++17 testcase console

	
CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../executable -lIPXACTmodelsd -lKactusAPId

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_SystemVerilogExpressionParserd

} else {
    # release mode
    LIBS += \
        -L../../executable -lIPXACTmodels -lKactusAPI

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_SystemVerilogExpressionParser
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../
INCLUDEPATH += ../../KactusAPI/include/

DEPENDPATH += .
DEPENDPATH += ../../

OBJECTS_DIR += $$DESTDIR
include(tst_SystemVerilogExpressionParser.pri)
