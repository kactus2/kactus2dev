TEMPLATE = app

TARGET = tst_DocumentGenerator

QT += core xml gui widgets testlib printsupport svg
CONFIG += c++11 testcase console

QMAKE_EXPORTED_VARIABLES += MAKE_TESTARGS
MAKE_TESTARGS.name = TESTARGS
MAKE_TESTARGS.value = "-platform offscreen"

win32:CONFIG(release, debug|release) {
    LIBS += -L../../executable/ -lIPXACTmodels -lKactusAPI
    DESTDIR = ./release
}
else:win32:CONFIG(debug, debug|release) {
    LIBS += -L../../executable/ -lIPXACTmodelsd -lKactusAPId
    DESTDIR = ./debug
}
else:unix {
    LIBS += -L../../executable/ -lIPXACTmodels -lKactusAPI
    DESTDIR = ./release
}

#DESTDIR = ./release

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += $$PWD/../../
INCLUDEPATH += $$PWD/../../KactusAPI/include

DEPENDPATH += .
DEPENDPATH += ../../

OBJECTS_DIR += $$DESTDIR

MOC_DIR += ./generatedFiles
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(tst_DocumentGenerator.pri)
