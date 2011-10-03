TEMPLATE = lib
HEADERS = ../namegroupbox.h \
    namegroupboxplugin.h
SOURCES = ../namegroupbox.cpp \
    namegroupboxplugin.cpp
TARGET = namegroupbox

CONFIG      += designer plugin
DESTDIR = $$[QT_INSTALL_PLUGINS]/designer

INCLUDEPATH += ../../../../
