# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

TEMPLATE = lib
TARGET = MemoryViewGenerator

QT += core xml widgets gui
CONFIG += c++11 plugin release

DEFINES += MEMORYVIEWGENERATOR_LIB

INCLUDEPATH += ./generatedFiles \
    . \
    ./../.. \
    $(QTDIR)/../qttools/include \
    ./generatedFiles/Release
LIBS += -L"./../../executable" \
    -lIPXACTmodels \
    -lKactusAPI

DEPENDPATH += . \
    ./../../.. \

DESTDIR = ../../executable/Plugins

MOC_DIR += ./generatedFiles/release
OBJECTS_DIR += release
UI_DIR += ./generatedFiles
RCC_DIR += ./generatedFiles

include(MemoryViewGenerator.pri)

target.path = $$plugin_path
INSTALLS += target
