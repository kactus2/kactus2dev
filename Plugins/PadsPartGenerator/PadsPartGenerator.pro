# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

TEMPLATE = lib
TARGET = PadsPartGenerator

QT += core xml widgets gui
CONFIG += c++11 release
DEFINES += QT_DLL QT_WIDGETS_LIB QT_XML_LIB

INCLUDEPATH += ./generatedFiles \
    . \
    ./generatedFiles/Release \
    $(QTDIR)/../qtxmlpatterns/include/QtXmlPatterns \
    $(QTDIR)/../qtxmlpatterns/include \
    ./../..

LIBS += -L"./../../executable" \
    -lIPXACTmodels \
    -lKactusAPI

DESTDIR = ../../executable/Plugins

DEPENDPATH += .
MOC_DIR += ./generatedFiles/release
OBJECTS_DIR += release
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles
include(PadsPartGenerator.pri)

target.path = $$plugin_path
INSTALLS += target
