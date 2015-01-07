#-----------------------------------------------------------------------------
# File: tst_AppendingEditor.pro
#-----------------------------------------------------------------------------
# Project: Kactus 2
# Author: Esko Pekkarinen
# Date: 18.12.2014
#
# Description:
# Qt project file template for running unit tests for AppendingEditor.
#-----------------------------------------------------------------------------

TEMPLATE = app

TARGET = tst_AppendingEditor

QT += core xml gui testlib widgets
CONFIG += testcase console

win32:CONFIG(release, debug|release) {
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
    DESTDIR = ./debug
}
else:unix {
    DESTDIR = ./release
}

INCLUDEPATH += $$PWD/../../../../
INCLUDEPATH += $$DESTDIR

DEPENDPATH += $$PWD/../../../../
DEPENDPATH += .

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_AppendingEditor.pri)
