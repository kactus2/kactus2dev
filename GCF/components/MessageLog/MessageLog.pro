######################################################################
# Generic Component Framework Library from VCreate Logic
# 
# Provides a project manager component. Most useful in applications
# that save/load data from XML files. Different aspects of the 
# project will be stored under different XML elements
######################################################################

include(../../config.pri)

TEMPLATE        = lib
CONFIG         += dll 
LIBS           += $${GCF_LIBS}
DEPENDPATH     += . interfaces source
INCLUDEPATH    += . interfaces source 

MOC_DIR         = $${GCF_TEMP_DIR}/MessageLog
OBJECTS_DIR     = $${GCF_TEMP_DIR}/MessageLog
UI_HEADERS_DIR  = $${GCF_TEMP_DIR}/MessageLog
RCC_DIR         = $${GCF_TEMP_DIR}/MessageLog
UI_DIR          = $${GCF_TEMP_DIR}/MessageLog

HEADERS         += MessageLogComponent.h
SOURCES         += MessageLogComponent.cpp
RESOURCES       += MessageLog.qrc