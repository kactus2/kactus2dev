######################################################################
# Generic Component Framework Library from VCreate Logic
# 
# Provides a Math and XML utils implementation
######################################################################

include(../../config.pri)

TEMPLATE        = lib
CONFIG         += dll 
LIBS           += $${GCF_LIBS}
DEPENDPATH     += . interfaces source
INCLUDEPATH    += . interfaces source

MOC_DIR         = $${GCF_TEMP_DIR}/Utils
OBJECTS_DIR     = $${GCF_TEMP_DIR}/Utils
UI_HEADERS_DIR  = $${GCF_TEMP_DIR}/Utils
RCC_DIR         = $${GCF_TEMP_DIR}/Utils
UI_DIR          = $${GCF_TEMP_DIR}/Utils

# Input
HEADERS += UtilsComponent.h
SOURCES += UtilsComponent.cpp



