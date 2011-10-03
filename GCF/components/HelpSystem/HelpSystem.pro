######################################################################
# Generic Component Framework Library from VCreate Logic
# 
# Provides the QtHelp module in a prepackaged component.
######################################################################

include(../../config.pri)

TEMPLATE        = lib
CONFIG         += dll help
LIBS           += $${GCF_LIBS}
DEPENDPATH     += . interfaces source
INCLUDEPATH    += . interfaces source 

MOC_DIR         = $${GCF_TEMP_DIR}/HelpSystem
OBJECTS_DIR     = $${GCF_TEMP_DIR}/HelpSystem
UI_HEADERS_DIR  = $${GCF_TEMP_DIR}/HelpSystem
RCC_DIR         = $${GCF_TEMP_DIR}/HelpSystem
UI_DIR          = $${GCF_TEMP_DIR}/HelpSystem

HEADERS         += source/HelpSystemComponent.h \
                   source/HelpView.h \
                   source/TopicChooserDialog.h \
                   source/HelpSystemConfigurationPage.h
                   
SOURCES         += source/HelpSystemComponent.cpp \
                   source/HelpView.cpp \
                   source/TopicChooserDialog.cpp \
                   source/HelpSystemConfigurationPage.cpp

FORMS           += source/TopicChooserDialog.ui \
                   source/HelpSystemConfigurationPage.ui

# QT              += webkit

RESOURCES       += HelpSystem.qrc

