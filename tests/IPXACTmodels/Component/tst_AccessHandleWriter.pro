#-----------------------------------------------------------------------------
# File: tst_AccessHandleWriter.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Anton Hagqvist
# Date: 02.06.2026
#-----------------------------------------------------------------------------

TEMPLATE = app
TARGET = tst_AccessHandleWriter

QT += core xml testlib
QT -= gui

CONFIG += c++17 testcase console

CONFIG(debug, debug|release) {
    LIBS += \
        -L../../../executable -lIPXACTmodelsd 

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_AccessHandleWriterd
} else {
    LIBS += \
        -L../../../executable -lIPXACTmodels
    
    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_AccessHandleWriter
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += ../../../

DEPENDPATH += .
DEPENDPATH += ../../../

include(tst_AccessHandleWriter.pri)
