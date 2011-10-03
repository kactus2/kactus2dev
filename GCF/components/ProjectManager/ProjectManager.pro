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
INCLUDEPATH    += . interfaces source ../Utils/interfaces

MOC_DIR         = $${GCF_TEMP_DIR}/ProjectManager
OBJECTS_DIR     = $${GCF_TEMP_DIR}/ProjectManager
UI_HEADERS_DIR  = $${GCF_TEMP_DIR}/ProjectManager
RCC_DIR         = $${GCF_TEMP_DIR}/ProjectManager
UI_DIR          = $${GCF_TEMP_DIR}/ProjectManager

# Input
HEADERS +=	Project.h \
		ProjectManagerComponent.h

SOURCES += 	Project.cpp \
		ProjectManagerComponent.cpp

RESOURCES += ProjectManager.qrc



