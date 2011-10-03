######################################################################
# Generic Component Framework Library from VCreate Logic
# 
# Provides a simple, reusable, script editor component. Using this
# component you can quickly make your applications scriptable. 
# 
######################################################################

include(../../config.pri)

TEMPLATE        = lib
CONFIG         += dll 
LIBS           += $${GCF_LIBS}
DEPENDPATH     += . interfaces source
INCLUDEPATH    += . interfaces source 

# Interfaces from other components
INCLUDEPATH    += ../PropertyEditor/interfaces
INCLUDEPATH    += ../ProjectManager/interfaces
INCLUDEPATH    += ../Utils/interfaces

MOC_DIR         = $${GCF_TEMP_DIR}/ScriptEditor
OBJECTS_DIR     = $${GCF_TEMP_DIR}/ScriptEditor
UI_HEADERS_DIR  = $${GCF_TEMP_DIR}/ScriptEditor
RCC_DIR         = $${GCF_TEMP_DIR}/ScriptEditor
UI_DIR          = $${GCF_TEMP_DIR}/ScriptEditor

CONFIG          += uitools
QT              += script

# Input
HEADERS +=  interfaces/IScriptDataType.h \
            interfaces/IScriptEngineManager.h \
            source/LoadFormDialog.h \
            source/ScriptableObjectExplorer.h \
            source/ScriptEditor.h \
            source/ScriptEditorComponent.h \
            source/ScriptEngineManager.h \
            source/ScriptEditorConfigurationPage.h

FORMS   +=  source/FindAndReplaceDialog.ui \
            source/FindTextDialog.ui \
            source/LoadFormDialog.ui \
            source/UnloadFormDialog.ui \
            source/ScriptEditorConfigurationPage.ui

SOURCES +=  source/LoadFormDialog.cpp \
            source/ScriptableObjectExplorer.cpp \
            source/ScriptEditor.cpp \
            source/ScriptEditorComponent.cpp \
            source/ScriptEngineManager.cpp \
            source/ScriptEditorConfigurationPage.cpp

RESOURCES += ScriptEditor.qrc
