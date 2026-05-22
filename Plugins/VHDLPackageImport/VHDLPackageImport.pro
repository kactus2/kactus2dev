# ----------------------------------------------------
# Created by copying data from other .pro-files
# ------------------------------------------------------

TEMPLATE = lib

QT += core xml widgets gui
CONFIG += c++11

DEFINES += VHDLPACKAGEIMPORT_LIB

INCLUDEPATH += ./../.. \
    ./GeneratedFiles \
    . \

CONFIG(debug, debug|release) {
    # debug mode
    LIBS += \
        -L../../executable -lIPXACTmodelsd

    MOC_DIR += ./GeneratedFiles/Debug
    OBJECTS_DIR += Debug
    TARGET = VHDLPackageImportd

} else {
    # release mode 
    LIBS += \
        -L../../executable -lIPXACTmodels
    
    MOC_DIR += ./GeneratedFiles/Release
    OBJECTS_DIR += Release
    TARGET = VHDLPackageImport
}

DESTDIR = ../../executable/Plugins

DEPENDPATH += .
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(VHDLPackageImport.pri)

target.path = $$plugin_path
INSTALLS += target

