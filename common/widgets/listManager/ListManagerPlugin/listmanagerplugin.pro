TEMPLATE = lib
HEADERS = ../listmanager.h \
    listmanagerplugin.h \
    ../listmanagermodel.h
SOURCES = ../listmanager.cpp \
    listmanagerplugin.cpp \
    ../listmanagermodel.cpp
TARGET = listmanager

CONFIG      += designer plugin
DESTDIR = $$[QT_INSTALL_PLUGINS]/designer

INCLUDEPATH += ../../../../
