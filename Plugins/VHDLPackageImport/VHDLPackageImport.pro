# ----------------------------------------------------
# Created by copying data from other .pro-files
# ------------------------------------------------------

TEMPLATE = lib
TARGET = VHDLPackageImport

QT += core xml widgets gui
CONFIG += c++11 release

DEFINES += VHDLPACKAGEIMPORT_LIB

INCLUDEPATH += ./../.. \
    ./generatedFiles \
    . \
    ./generatedFiles/Debug
LIBS += -L"./../../executable" \
    -lIPXACTmodels

DESTDIR = ../../executable/Plugins

DEPENDPATH += .
MOC_DIR += ./generatedFiles/debug
OBJECTS_DIR += debug
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(VHDLPackageImport.pri)

target.path = $$plugin_path
INSTALLS += target

