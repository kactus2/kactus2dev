TEMPLATE = app

TARGET = tst_DocumentGenerator

QT += core xml gui widgets testlib printsupport svg
CONFIG += c++17 testcase console

QMAKE_EXPORTED_VARIABLES += MAKE_TESTARGS
MAKE_TESTARGS.name = TESTARGS
MAKE_TESTARGS.value = "-platform offscreen"

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../executable -lIPXACTmodelsd -lKactusAPId

    MOC_DIR += ./GeneratedFiles/Debug
    DESTDIR += Debug
    TARGET = tst_DocumentGeneratord

} else {
    # release mode
    LIBS += \
        -L../../executable -lIPXACTmodels -lKactusAPI

    MOC_DIR += ./GeneratedFiles/Release
    DESTDIR += Release
    TARGET = tst_DocumentGenerator
}

INCLUDEPATH += $$DESTDIR
INCLUDEPATH += $$PWD/../../
INCLUDEPATH += $$PWD/../../KactusAPI/include

DEPENDPATH += .
DEPENDPATH += ../../

OBJECTS_DIR += $$DESTDIR

include(tst_DocumentGenerator.pri)
