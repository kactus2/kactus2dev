# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Tools.
# ------------------------------------------------------

TEMPLATE = lib
TARGET = KactusAPI
DESTDIR = ../executable
CONFIG += c++17 release
QT += xml widgets
DEFINES +=  KACTUS2_EXPORTS
LIBS += -L"../executable" \
    -lIPXACTmodels 
	
INCLUDEPATH += ./.. \
  ./include

DEPENDPATH += .
MOC_DIR += .
OBJECTS_DIR += release
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(KactusAPI.pri)


target.path = $$lib_path
INSTALLS += target
