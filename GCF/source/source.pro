include(../config.pri)

TEMPLATE        = lib
win32:CONFIG   += dll 
TARGET          = GCF

MOC_DIR         = $${GCF_TEMP_DIR}/gcf
OBJECTS_DIR     = $${GCF_TEMP_DIR}/gcf
UI_HEADERS_DIR  = $${GCF_TEMP_DIR}/gcf
RCC_DIR         = $${GCF_TEMP_DIR}/gcf
UI_DIR          = $${GCF_TEMP_DIR}/gcf

DEFINES        += VCL_MAKE_DLL

# Input
HEADERS += $${GCF_BASE}/include/GCF/ComponentFactory.h \
           $${GCF_BASE}/include/GCF/ComponentGui.h \
           $${GCF_BASE}/include/GCF/ComponentLoaderComponent.h \
           $${GCF_BASE}/include/GCF/ConfigurationDialog.h \
           $${GCF_BASE}/include/GCF/ConfigurationDialogComponent.h \
           $${GCF_BASE}/include/GCF/DataStore.h \
           $${GCF_BASE}/include/GCF/Common.h \
           $${GCF_BASE}/include/GCF/SignalMultiplexer.h \
           $${GCF_BASE}/include/GCF/AbstractComponent.h \
           $${GCF_BASE}/include/GCF/IComponentPlugin.h \
           $${GCF_BASE}/include/GCF/Menu.h \
           $${GCF_BASE}/include/GCF/MenuStrip.h \
           $${GCF_BASE}/include/GCF/ComponentSettings.h \ 
           $${GCF_BASE}/include/GCF/ActionDocumentMapper.h \ 
           $${GCF_BASE}/include/GCF/ActionContainerWidget.h \
           $${GCF_BASE}/source/ComponentFactoryTable

FORMS   += $${GCF_BASE}/source/ComponentLoaderConfigPage.ui \
           $${GCF_BASE}/source/ConfigurationDialog.ui \
           $${GCF_BASE}/source/ComponentPropertyEditorPage.ui \
           $${GCF_BASE}/source/AddPropertyDialog.ui

SOURCES += $${GCF_BASE}/source/ComponentFactory.cpp \
           $${GCF_BASE}/source/ComponentGui.cpp \
           $${GCF_BASE}/source/ComponentLoaderComponent.cpp \
           $${GCF_BASE}/source/ConfigurationDialog.cpp \
           $${GCF_BASE}/source/ConfigurationDialogComponent.cpp \
           $${GCF_BASE}/source/DataStore.cpp \
           $${GCF_BASE}/source/SignalMultiplexer.cpp \
           $${GCF_BASE}/source/Menu.cpp \
           $${GCF_BASE}/source/MenuStrip.cpp \
           $${GCF_BASE}/source/AbstractComponent.cpp \
           $${GCF_BASE}/source/ComponentSettings.cpp \ 
           $${GCF_BASE}/source/ActionDocumentMapper.cpp \
           $${GCF_BASE}/source/ActionContainerWidget.cpp 

RESOURCES += $${GCF_BASE}/source/source.qrc

# IPC Stuff
HEADERS += $${GCF_BASE}/include/GCF/Message.h \
           $${GCF_BASE}/include/GCF/AppAccessPoint.h \
           $${GCF_BASE}/include/GCF/RemoteAppAccessPoint.h \
           $${GCF_BASE}/include/GCF/RemoteObject.h \
           $${GCF_BASE}/include/GCF/AppAccessPointComponent.h \
           $${GCF_BASE}/include/GCF/MethodInvocationHelper.h \
           $${GCF_BASE}/include/GCF/GenericSignalSpy.h \
           $${GCF_BASE}/include/GCF/RemoteAppDiscovery.h 

SOURCES += $${GCF_BASE}/source/Message.cpp \
           $${GCF_BASE}/source/AppAccessPoint.cpp \
           $${GCF_BASE}/source/RemoteAppAccessPoint.cpp \
           $${GCF_BASE}/source/RemoteObject.cpp \
           $${GCF_BASE}/source/AppAccessPointComponent.cpp \
           $${GCF_BASE}/source/MethodInvocationHelper.cpp \
           $${GCF_BASE}/source/GenericSignalSpy.cpp \
           $${GCF_BASE}/source/RemoteAppDiscovery.cpp
           
FORMS   += $${GCF_BASE}/source/AppAccessPointConfigPage.ui \
           $${GCF_BASE}/source/AddRemoteAppDialog.ui


