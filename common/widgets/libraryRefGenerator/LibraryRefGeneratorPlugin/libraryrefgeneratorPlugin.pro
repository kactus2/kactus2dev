TEMPLATE = lib
HEADERS = ../libraryrefgenerator.h \
    libraryrefgeneratorplugin.h \
    ../../../../LibraryManager/vlnv.h
SOURCES = ../libraryrefgenerator.cpp \
    libraryrefgeneratorplugin.cpp \
    ../../../../LibraryManager/vlnv.cpp
TARGET = libraryrefgenerator

CONFIG      += designer plugin
DESTDIR = $$[QT_INSTALL_PLUGINS]/designer

INCLUDEPATH += ../../../../
