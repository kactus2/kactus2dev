TEMPLATE = lib
HEADERS = ../vlnvgenerator.h \
    vlnvgeneratorplugin.h \
    ../../../../LibraryManager/vlnv.h
SOURCES = ../vlnvgenerator.cpp \
    vlnvgeneratorplugin.cpp \
    ../../../../LibraryManager/vlnv.cpp
TARGET = vlnvgenerator

CONFIG      += designer plugin
DESTDIR = $$[QT_INSTALL_PLUGINS]/designer

INCLUDEPATH += ../../../../
