# ----------------------------------------------------
# Created by copying data from other .pro-files
# ------------------------------------------------------

TEMPLATE = lib
TARGET = VHDLPackageImport
DESTDIR = ../../executable/Plugins
QT += core xml widgets gui
CONFIG += release
DEFINES += QT_DLL QT_WIDGETS_LIB QT_XML_LIB VHDLPACKAGEIMPORT_LIB
INCLUDEPATH += ./../.. \
    ./GeneratedFiles \
    . \
    ./GeneratedFiles/Debug
LIBS += -L"./../../executable" \
    -lIPXACTmodels
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/debug
OBJECTS_DIR += debug
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(VHDLPackageImport.pri)

target.path = /usr/share/kactus2/plugins
INSTALLS += target

