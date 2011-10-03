######################################################################
# Generic Component Framework Library from VCreate Logic
# 
# Provides a multi-context, list based clipboard 
######################################################################

include(../../config.pri)

TEMPLATE        = lib
CONFIG         += dll help
LIBS           += $${GCF_LIBS}
DEPENDPATH     += . interfaces source ../Utils/interfaces
INCLUDEPATH    += . interfaces source ../Utils/interfaces

MOC_DIR         = $${GCF_TEMP_DIR}/Clipboard
OBJECTS_DIR     = $${GCF_TEMP_DIR}/Clipboard
UI_HEADERS_DIR  = $${GCF_TEMP_DIR}/Clipboard
RCC_DIR         = $${GCF_TEMP_DIR}/Clipboard
UI_DIR          = $${GCF_TEMP_DIR}/Clipboard

HEADERS         += source/ClipboardComponent.h \
                   source/ClipboardWidget.h \
                   source/ClipboardModel.h 
                   
SOURCES         += source/ClipboardComponent.cpp \
                   source/ClipboardWidget.cpp \
                   source/ClipboardModel.cpp 

FORMS           += source/ClipboardWidget.ui

# QT              += webkit


