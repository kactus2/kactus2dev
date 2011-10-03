#-------------------------------------------------
#
# Project created by QtCreator 2011-01-21T13:24:46
#
#-------------------------------------------------

CONFIG      += designer plugin

TEMPLATE    = lib
DESTDIR = $$[QT_INSTALL_PLUGINS]/designer

SOURCES += \
    attributemodel.cpp \
    attributeboxplugin.cpp \
    attributebox.cpp \
    ../../dialogs/attributeDialog/attributedialog.cpp

HEADERS  += \
    attributeboxplugin.h \
    attributebox.h \
    attributemodel.h \
    ../../dialogs/attributeDialog/attributedialog.h

FORMS +=

INCLUDEPATH += ../../../
