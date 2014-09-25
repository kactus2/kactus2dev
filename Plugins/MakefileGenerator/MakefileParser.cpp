//-----------------------------------------------------------------------------
// File: MakefilParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 22.09.2014
//
// Description:
// Makefile parser.
//-----------------------------------------------------------------------------

#include "MakefileParser.h"

#include <IPXACTmodels/fileset.h>
#include <IPXACTmodels/ApiInterface.h>
#include "IPXACTmodels/SystemView.h"

//-----------------------------------------------------------------------------
// Function: MakefileParser::MakefileParser()
//-----------------------------------------------------------------------------
MakefileParser::MakefileParser()
{
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::~MakefileParser()
//-----------------------------------------------------------------------------
MakefileParser::~MakefileParser()
{
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::getParsedData()
//-----------------------------------------------------------------------------
const QList<MakefileParser::MakeFileData>& MakefileParser::getParsedData()
{
    return parsedData_;
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::searchSWComponent()
//-----------------------------------------------------------------------------
void MakefileParser::parse( LibraryInterface* library, QSharedPointer<Component> topComponent,
    QSharedPointer<DesignConfiguration const> desgConf, QSharedPointer<const Design> design)
{
    foreach ( SWInstance softInstance, design->getSWInstances() )
    {
        // The VLNV and the component of the instance are needed.
        const VLNV softwareVLNV = softInstance.getComponentRef();
        QSharedPointer<LibraryComponent> instanceLibSWComp = library->getModel(softwareVLNV);
        QSharedPointer<Component> softComponent = instanceLibSWComp.dynamicCast<Component>();

        // Lower levels of a stack do not get their own instances.
        if ( !isTopOfStack(design, softInstance, softComponent) )
        {
            continue;
        }

        // Initialize the data of make file to collection of parsed entities.
        parsedData_.append(MakeFileData());
        MakeFileData& makeData = parsedData_.last();
        // Since every software instance gets its own makefile, naming is after the instance name.
        makeData.name = softInstance.getInstanceName();

        // The top component of the design may contain header files specific to the instance.
        findInstanceHeaders(library, topComponent, desgConf, softInstance, makeData);

        // Parse files of the underlying hardware.
        QString hwName = softInstance.getMapping();
        QSharedPointer<SWView> hardView;
        hardView = parseHWMakeObjects(design, hwName, desgConf, library, makeData);

        // There may be only one active software view.
        QString softViewName = desgConf->getActiveView(softInstance.getInstanceName());
        QSharedPointer<SWView> softView = softComponent->getSWView(softViewName);

        // Parse its files, as well as files of any instance in the underlying stack.
        parseStackObjects(softComponent, softView, softInstance, design, library, desgConf, makeData);

        // Both component and hardware software views are needed to build.
        findHardwareBuildCommand(makeData, softView, hardView);

        // No need to have the same folder twice in the includes.
        makeData.includeDirectories.removeDuplicates();

        // Same applies to flags
        makeData.softViewFlags.removeDuplicates();
    }
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::isTopOfStack()
//-----------------------------------------------------------------------------
bool MakefileParser::isTopOfStack(QSharedPointer<const Design> design, SWInstance &softInstance, QSharedPointer<Component> softComponent)
{
    bool goesUnder = false;

    foreach ( ApiDependency connection, design->getApiDependencies() )
    {
        QSharedPointer<ApiInterface> ourInterface;

        // See if one of the ends is ours.
        if ( softInstance.getInstanceName() == connection.getInterface1().componentRef )
        {
            ourInterface = softComponent->getApiInterface( connection.getInterface1().apiRef );
        }

        if ( softInstance.getInstanceName() == connection.getInterface2().componentRef )
        {
            ourInterface = softComponent->getApiInterface( connection.getInterface2().apiRef );
        }

        // If found and it turns out that we are an active provider, we are not on the top of a stack.
        if ( !ourInterface.isNull() && ourInterface->getDependencyDirection() == DEPENDENCY_PROVIDER )
        {
            goesUnder = true;
            break;
        }
    }

    return !goesUnder;
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::findInstanceHeaders()
//-----------------------------------------------------------------------------
void MakefileParser::findInstanceHeaders(LibraryInterface* library, QSharedPointer<Component> topComponent,
    QSharedPointer<DesignConfiguration const> desgConf, SWInstance &softInstance, MakeFileData &makeData)
{
    // The path leading to the design.
    QFileInfo componentQfi = QFileInfo(library->getPath(*desgConf->getVlnv()));

    // Find the name of the system view associated with the design.
    QString sysViewName;

    foreach( QSharedPointer<SystemView> view, topComponent->getSystemViews() )
    {
        if ( view->getHierarchyRef() == *desgConf->getVlnv() )
        {
            sysViewName = view->getName();
            break;
        }
    }

    foreach ( QSharedPointer<FileSet> fileSet, topComponent->getFileSets() )
    {
        // The fileSet names of the instance headers are hard coded.
        if ( fileSet->getName() == softInstance.getFileSetRef() )
        {
            foreach( QSharedPointer<File> file, fileSet->getFiles())
            {
                // We are only interested in the actual header files.
                if ( file->getIncludeFile() )
                {
                    // We may assume that a file path is relative to the component path, and thus the include path
                    // is component path + file path.
                    QFileInfo fileQfi = QFileInfo(componentQfi.absolutePath() + "/" + file->getName());
                    makeData.includeDirectories.append(fileQfi.absolutePath());
                }
            }

            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::parseHWMakeObjects()
//-----------------------------------------------------------------------------
QSharedPointer<SWView> MakefileParser::parseHWMakeObjects(QSharedPointer<const Design> design, QString hwName,
    QSharedPointer<DesignConfiguration const> desgConf, LibraryInterface* library, MakeFileData &makeData)
{
    QSharedPointer<SWView> hardView;
    ComponentInstance* hwInstance = 0;

    // See if the design contains a hardware instance corresponding the mapped hardware instance.
    foreach ( ComponentInstance instance, design->getComponentInstances() )
    {
        if ( instance.getUuid() == hwName )
        {
            hwInstance = &instance;
            break;
        }
    }

    // If not found, skip.
    if ( hwInstance != 0 )
    {
        // The active view is the interesting one.
        QString hardViewName = desgConf->getActiveView(hwInstance->getInstanceName());

        // Get the component of the hardware instance.
        const VLNV hardwareVLNV = hwInstance->getComponentRef();
        QSharedPointer<LibraryComponent> instanceLibHWComp = library->getModel(hardwareVLNV);
        QSharedPointer<Component> hardComponent = instanceLibHWComp.dynamicCast<Component>();

        // The get the active software view from it.
        hardView = hardComponent->getSWView(hardViewName);

        // Finally parse the files of the said view.
        parseMakeObjects(library, hardView, hardComponent, makeData, makeData.hwObjects, false);
    }
    
    return hardView;
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::createStackObjects()
//-----------------------------------------------------------------------------
void MakefileParser::parseStackObjects(QSharedPointer<Component> softComponent,
    QSharedPointer<SWView> softView, SWInstance &softInstance, QSharedPointer<const Design> design,
    LibraryInterface* library, QSharedPointer<DesignConfiguration const> desgConf, MakeFileData& makeData)
{
    // Skip if already parsed
    if ( makeData.parsedInstances.contains( softInstance.getInstanceName() ) )
    {
        return;
    }

    // Parse the files of the given software view.
    parseMakeObjects(library, softView, softComponent, makeData, makeData.swObjects, true);

    // Add to the list
    makeData.parsedInstances.append( softInstance.getInstanceName() );

    // Go through the list of connections in the design to retrieve remote endpoint identifiers.
    foreach ( ApiDependency connection, design->getApiDependencies() )
    {
        QSharedPointer<ApiInterface> ourInterface;
        QSharedPointer<ApiInterface> theirInterface;
        SWInstance theirInstance;
        QSharedPointer<Component> theirComponent;

        // We must find the API interfaces of the both ends, and the software instance and component of the
        // end that is NOT us.
        if ( softInstance.getInstanceName() == connection.getInterface1().componentRef )
        {
            ourInterface = softComponent->getApiInterface( connection.getInterface1().apiRef );
            theirComponent = searchSWComponent(library, design, connection.getInterface2().componentRef, theirInstance);

            theirInterface = theirComponent->getApiInterface( connection.getInterface2().apiRef );
        }

        if ( softInstance.getInstanceName() == connection.getInterface2().componentRef )
        {
            ourInterface = softComponent->getApiInterface( connection.getInterface2().apiRef );
            theirComponent = searchSWComponent(library, design, connection.getInterface1().componentRef, theirInstance);

            theirInterface = theirComponent->getApiInterface( connection.getInterface1().apiRef );
        }

        // If both ends are found and are pointing the right way, we may parse the files of its active view.
        if ( !ourInterface.isNull() && ourInterface->getDependencyDirection() == DEPENDENCY_REQUESTER && 
            !theirInterface.isNull() && theirInterface->getDependencyDirection() == DEPENDENCY_PROVIDER )
        {
            QString softViewName = desgConf->getActiveView(theirInstance.getInstanceName());
            QSharedPointer<SWView> theirView = theirComponent->getSWView(softViewName);

            parseStackObjects( theirComponent, theirView, theirInstance, design, library, desgConf, makeData );
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::createMakeObjects()
//-----------------------------------------------------------------------------
void MakefileParser::parseMakeObjects(LibraryInterface* library, QSharedPointer<SWView> view,
    QSharedPointer<Component> component, MakeFileData &makeData, QList<MakeObjectData>& objects, bool pickSWView)
{
    // Go through the fileSets referenced in the software view.
    foreach( QString fsetName, view->getFileSetRefs())
    {
        QSharedPointer<FileSet> fset = component->getFileSet(fsetName);

        // Go through the files in the fileSet.
        foreach( QSharedPointer<File> file, fset->getFiles())
        {
            // Initialize the data to the collection associated with the makefile data.
            objects.append(MakeObjectData());
            MakeObjectData& objectData = objects.last();

            // We may assume that a file path is relative to the component path, and thus the include path
            // is component path + file path.
            QFileInfo componentQfi = QFileInfo(library->getPath(*component->getVlnv()));
            QFileInfo fileQfi = QFileInfo(componentQfi.absolutePath() + "/" + file->getName());

            // Set the needed fields.
            objectData.file = file;
            objectData.fileBuildCmd = file->getBuildcommand();
            objectData.fileName = fileQfi.fileName();
            objectData.path = fileQfi.absolutePath();

            // In case of an include file, its path is added to the collection of the include paths.
            if ( file->getIncludeFile() )
            {
                makeData.includeDirectories.append(fileQfi.absolutePath());
            }

            // A fileSet builder associated with the file type is also a possible field.
            foreach( QSharedPointer<FileBuilder> builder, fset->getDefaultFileBuilders() )
            {
                if ( file->getFileTypes().contains( builder->getFileType() ) )
                {
                    objectData.fileSetBuildCmd = builder;
                    break;
                }
            }

            if( pickSWView )
            {
                // Find build command of matching file type from the software view.
                foreach( QSharedPointer<SWBuildCommand> buildCmd, view->getSWBuildCommands() )
                {
                    if ( file->getFileTypes().contains( buildCmd->getFileType() ) )
                    {
                        objectData.swBuildCmd = buildCmd;

                        // If found, append the flags of the software view for later use.
                        makeData.softViewFlags.append(buildCmd->getFlags());

                        break;
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::searchSWComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> MakefileParser::searchSWComponent(LibraryInterface* library,
    QSharedPointer<const Design> design, QString instanceName, SWInstance& targetInstance)
{
    // Go through the software instances of the design, finding the right one.
    foreach ( SWInstance instance, design->getSWInstances() )
    {
        // If the found instance name is same as target, it is what we seek.
        if ( instance.getInstanceName() == instanceName )
        {
            targetInstance = instance;
            break;
        }
    }

    // Obtain the component of the instance.
    VLNV instanceVLNV = targetInstance.getComponentRef();
    QSharedPointer<LibraryComponent> instanceLibComp = library->getModel(instanceVLNV);
    QSharedPointer<Component> instanceComp = instanceLibComp.dynamicCast<Component>();

    return instanceComp;
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::findHardwareBuildCommand()
//-----------------------------------------------------------------------------
void MakefileParser::findHardwareBuildCommand(MakeFileData &makeData, QSharedPointer<SWView> softView, QSharedPointer<SWView> hardView)
{
    // No software view from hardware means no can do.
    if ( hardView == 0 )
    {
        return;
    }

    // Go through the files parsed from the software view of the software component.
    foreach(MakeObjectData mod, makeData.swObjects)
    {
        // Find build command from the software view of the hardware component matching file type.
        foreach( QSharedPointer<SWBuildCommand> buildCmd, hardView->getSWBuildCommands() )
        {
            if ( mod.file->getFileTypes().contains( buildCmd->getFileType() ) )
            {
                makeData.hwBuildCmd = buildCmd;
                break;
            }
        }
    }

    // Go through the files parsed from the software view of the hardware component.
    foreach(MakeObjectData mod, makeData.hwObjects)
    {
        // Find build command from the software view of the hardware component matching file type.
        foreach( QSharedPointer<SWBuildCommand> buildCmd, hardView->getSWBuildCommands() )
        {
            if ( mod.file->getFileTypes().contains( buildCmd->getFileType() ) )
            {
                makeData.hwBuildCmd = buildCmd;
                break;
            }
        }
    }
}
