//-----------------------------------------------------------------------------
// File: MakefileParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 22.09.2014
//
// Description:
// Makefile parser.
//-----------------------------------------------------------------------------

#include "MakefileParser.h"

#include <IPXACTmodels/kactusExtensions/ApiInterface.h>
#include <IPXACTmodels/kactusExtensions/ApiInterconnection.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <QSet>
#include "editors/ComponentEditor/common/ListParameterFinder.h"
#include "editors/ComponentEditor/common/IPXactSystemVerilogParser.h"

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
// Function: MakefileParser::getGeneralFileSet()
//-----------------------------------------------------------------------------
const QSharedPointer<FileSet>& MakefileParser::getGeneralFileSet()
{
    return generalFileSet_;
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::getReplacedFiles()
//-----------------------------------------------------------------------------
const QStringList& MakefileParser::getReplacedFiles()
{
    return replacedFiles_;
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::findConflicts()
//-----------------------------------------------------------------------------
QVector<QSet<QSharedPointer<MakefileParser::MakeObjectData> > > MakefileParser::findConflicts() const
{
	QVector<QSet<QSharedPointer<MakeObjectData> > > conflicts;

	foreach( MakeFileData mfd, parsedData_ )
	{
		QList<QSharedPointer<MakeObjectData> > objects = mfd.swObjects;

		// Go on until the list is empty.
		while ( !objects.empty() )
		{
			// Remove the object, so that there will not be a double entry nor self-evaluation.
			QSharedPointer<MakeObjectData> mod = objects.first();
			objects.removeFirst();

			QSet<QSharedPointer<MakeObjectData> > conflictingFiles;

			// The object itself must be one of the sides of the conflict.
			conflictingFiles.insert( mod );

			// Compare it to the others
			for ( int i = 0; i < objects.size(); ++i )
			{
				QSharedPointer<MakeObjectData> modCompare = objects[i];

				// Absolute path is same means that the file is definitely same.
				// Must also consider the filename, as it is omitted from the path.
				if ( modCompare->path == mod->path && modCompare->fileName == mod->fileName )
				{
					// If options concerning the two files are NOT conflicting, there are no conflict.
					bool same = false;

					// First see if the compilers are the same, and the include file setting.
					if ( modCompare->compiler == mod->compiler &&
						modCompare->file->isIncludeFile() == mod->file->isIncludeFile() )
					{
						// Create list of flags.
						QStringList flagList = mod->flags.split(" ", QString::SkipEmptyParts);
						QStringList flagListCompare = modCompare->flags.split(" ", QString::SkipEmptyParts);

						if ( flagList.size() == flagListCompare.size() )
						{
							// Are the same size -> Proceed to check if content is the same.
							flagList.sort(Qt::CaseSensitive);
							flagListCompare.sort(Qt::CaseSensitive);
							same = true;

							// Now go through the lists and see if they are the same.
							for ( int j = 0; j < flagList.size(); ++j )
							{
								if ( flagList[j] != flagListCompare[j] )
								{
									same = false;
									break;
								}
							}
						}
					}

					// Conflict confirmed: Add to the list.
					if ( !same )
					{
						conflictingFiles.insert( modCompare );

						// Also remove from the compared items
						objects.removeAt(i);
						// Item removed, so has to rewind the iterator.
						--i;
					}
				}
			}

			// One side of the conflict is the file compared to others, to has to be two conflictors!
			if ( conflictingFiles.size() > 1 )
			{
				conflicts.append( conflictingFiles );
			}
		}
	}

	return conflicts;
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::searchSWComponent()
//-----------------------------------------------------------------------------
void MakefileParser::parse(LibraryInterface* library, QSharedPointer<Component> topComponent,
    QSharedPointer<DesignConfiguration const> desgConf, QSharedPointer<const Design> design,
    QString sysViewName, QString targetPath /*= ""*/)
{
    // Fabricate name for the fileSet of design-wide files.
    QString fileSetName = sysViewName + "_general_files";

	// Obtain the the fileSet by name and set it as a source file group.
	QSharedPointer<FileSet> fileSet = topComponent->getFileSet(fileSetName);

	if ( !fileSet )
	{
		QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets = topComponent->getFileSets();
		fileSet = QSharedPointer<FileSet>( new FileSet(fileSetName, "sourceFiles") );
		fileSets->append(fileSet);
	}

    fileSet->setGroups("sourceFiles");

    // This is also a fileSet referenced by the makefile generation
    generalFileSet_ = fileSet;

    // The base directory for the software.
    QString basePath = targetPath + "/sw_" + sysViewName + "/";

    // Check if the main make file and the launcher files already exits.
    QString mainMakeDir = basePath + "Makefile";
    QString launcherDir = basePath + "launcher.sh";

    // We also need to know if the file exists
    QFile makeFile(mainMakeDir);

    // If it does, put in the list.
    if ( makeFile.exists() )
    {
        replacedFiles_.append(mainMakeDir);
    }

    // We also need to know if the file exists
    QFile launcherFile(launcherDir);

    // If it does, put in the list.
    if ( launcherFile.exists() )
    {
        replacedFiles_.append(launcherDir);
    }

    foreach ( QSharedPointer<SWInstance> softInstance, design->getSWInstances() )
    {
        // The VLNV and the component of the instance are needed.
		QSharedPointer<VLNV> softwareVLNV = softInstance->getComponentRef();
        QSharedPointer<Document> instanceLibSWComp = library->getModel(*softwareVLNV);
        QSharedPointer<Component> softComponent = instanceLibSWComp.dynamicCast<Component>();

        // Lower levels of a stack do not get their own instances.
        if ( !isTopOfStack(design, softInstance, softComponent) )
        {
            continue;
        }

        // Initialize the data of make file to collection of parsed entities.
        parsedData_.append(MakeFileData());
        MakeFileData& makeData = parsedData_.last();

        // The top component of the design may contain header files specific to the instance.
        findInstanceHeaders(library, topComponent, desgConf, sysViewName, softInstance, makeData);

        // Parse files of the underlying hardware.
        QString hwName = softInstance->getMapping();
        QSharedPointer<SWView> hardView;
        hardView = parseHWMakeObjects(design, hwName, desgConf, library, makeData);

        // There may be only one active software view.
        QString softViewName = desgConf->getActiveView(softInstance->getInstanceName());
		QSharedPointer<SWView> softView;

		foreach( QSharedPointer<SWView> joku, softComponent->getSWViews() )
		{
			if ( joku->name() == softViewName )
			{
				softView = joku;
				break;
			}
		}

		// Null means no action
		if ( !softView )
		{
			continue;
		}

        // Parse its files, as well as files of any instance in the underlying stack.
        parseStackObjects(softComponent, softView, softInstance, design, library, desgConf, makeData);

        // Both component and hardware software views are needed to build.
        findHardwareBuildCommand(makeData, softView, hardView);

		// Now we may check the compiler and flags of individual files...
		foreach ( QSharedPointer<MakeObjectData> mod, makeData.swObjects )
		{
			mod->compiler = getFileCompiler( mod, hardView);
			mod->flags = getFileFlags(softComponent, mod , makeData );
		}

        // Check if software objects found: If none, no need for a make file.
        if ( makeData.swObjects.count() < 1 )
        {
            parsedData_.removeLast();
            continue;
        }

        // No need to have the same folder twice in the includes.
        makeData.includeDirectories.removeDuplicates();

        // Same applies to flags
        makeData.softViewFlags.removeDuplicates();

        // Since every software stack gets its own makefile, naming is after the instance name.
        makeData.name = softInstance->getInstanceName();

        // We may need the absolute path of the file
        QString instancePath = basePath + makeData.name;
        QString dir = instancePath + "/Makefile";

        // We also need to know if the file exists
        QFile makeFile(dir);

        // If it does, put in the list.
        if ( makeFile.exists() )
        {
            replacedFiles_.append(dir);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::isTopOfStack()
//-----------------------------------------------------------------------------
bool MakefileParser::isTopOfStack(QSharedPointer<const Design> design, QSharedPointer<SWInstance> softInstance,
	QSharedPointer<Component> softComponent)
{
    bool goesUnder = false;

    foreach ( QSharedPointer<ApiInterconnection> connection, design->getApiConnections() )
    {
        QSharedPointer<ApiInterface> ourInterface;

        // See if one of the ends is ours.
        if ( softInstance->getInstanceName() == connection->getInterface1()->getComponentReference() )
        {
            ourInterface = softComponent->getApiInterface( connection->getInterface1()->getBusReference() );
        }

        if ( softInstance->getInstanceName() == connection->getInterface2()->getComponentReference() )
        {
            ourInterface = softComponent->getApiInterface( connection->getInterface2()->getBusReference() );
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
    QSharedPointer<DesignConfiguration const> desgConf, QString sysViewName, QSharedPointer<SWInstance> softInstance,
    MakeFileData &makeData)
{
    // The path leading to the design.
    QFileInfo componentQfi = QFileInfo(library->getPath(desgConf->getVlnv()));

    QString fileSetName = softInstance->getFileSetRef();

    // Create a new fileSet, if no reference exist
    if ( fileSetName.isNull() )
    {
        // If not, make a new one.
        fileSetName = sysViewName + "_" + softInstance->getInstanceName() + "_headers";
        softInstance->setFileSetRef( fileSetName );
    }

	// Obtain the the fileSet by name and set it as a source file group.
	QSharedPointer<FileSet> fileSet = topComponent->getFileSet(fileSetName);

	if ( !fileSet )
	{
		QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets = topComponent->getFileSets();
		fileSet = QSharedPointer<FileSet>( new FileSet(fileSetName, "sourceFiles") );
		fileSets->append(fileSet);
	}

	fileSet->setGroups("sourceFiles");

    // This is also a fileSet referenced by the makefile generation
	makeData.fileSet = fileSet;

	// They want the name of the instance.
	QString instanceName = softInstance->getDisplayName();

	if ( instanceName.isEmpty() )
	{
		instanceName = softInstance->getInstanceName();
	}

	// Finally parse the files of the said view.
	parseFileSet(fileSet, instanceName, makeData.swObjects, library, topComponent, makeData, false, QSharedPointer<SWView>());
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::parseHWMakeObjects()
//-----------------------------------------------------------------------------
QSharedPointer<SWView> MakefileParser::parseHWMakeObjects(QSharedPointer<const Design> design, QString hwName,
    QSharedPointer<DesignConfiguration const> desgConf, LibraryInterface* library, MakeFileData &makeData)
{
    QSharedPointer<SWView> hardView;
    QSharedPointer<ComponentInstance> hwInstance;

    // See if the design contains a hardware instance corresponding the mapped hardware instance.
    foreach ( QSharedPointer<ComponentInstance> instance, *design->getComponentInstances() )
    {
        if ( instance->getUuid() == hwName )
        {
            hwInstance = instance;
            break;
        }
    }

    // If not found, skip.
    if ( hwInstance != 0 )
    {
        // The active view is the interesting one.
        QString hardViewName = desgConf->getActiveView(hwInstance->getInstanceName());

        // Get the component of the hardware instance.
        QSharedPointer<VLNV> hardwareVLNV = hwInstance->getComponentRef();
        QSharedPointer<Document> instanceLibHWComp = library->getModel(*hardwareVLNV);
        QSharedPointer<Component> hardComponent = instanceLibHWComp.dynamicCast<Component>();

		foreach( QSharedPointer<SWView> joku, hardComponent->getSWViews() )
		{
			if ( joku->name() == hardViewName )
			{
				hardView = joku;
				break;
			}
		}

		// Null means no action.
		if ( !hardView )
		{
			return hardView;
		}

		// They want the name of the instance.
		QString instanceName = hwInstance->getDisplayName();

		if ( instanceName.isEmpty() )
		{
			instanceName = hwInstance->getInstanceName();
		}

        // Finally parse the files of the said view.
        parseMakeObjects(library, hardView, instanceName, hardComponent, makeData, makeData.swObjects, false);
    }
    
    return hardView;
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::parseStackObjects()
//-----------------------------------------------------------------------------
void MakefileParser::parseStackObjects(QSharedPointer<Component> softComponent,
    QSharedPointer<SWView> softView, QSharedPointer<SWInstance> softInstance, QSharedPointer<const Design> design,
    LibraryInterface* library, QSharedPointer<DesignConfiguration const> desgConf, MakeFileData& makeData)
{
    // Skip if already parsed
    if ( makeData.parsedInstances.contains( softInstance->getInstanceName() ) )
    {
        return;
	}

	// They want the name of the instance.
	QString instanceName = softInstance->getDisplayName();

	if ( instanceName.isEmpty() )
	{
		instanceName = softInstance->getInstanceName();
	}

    // Parse the files of the given software view.
    parseMakeObjects(library, softView, instanceName, softComponent, makeData, makeData.swObjects, true);

    // Add to the list
    makeData.parsedInstances.append( instanceName );

    // Go through the list of connections in the design to retrieve remote endpoint identifiers.
    foreach ( QSharedPointer<ApiInterconnection> connection, design->getApiConnections() )
    {
        QSharedPointer<ApiInterface> ourInterface;
        QSharedPointer<ApiInterface> theirInterface;
        QSharedPointer<SWInstance> theirInstance;
        QSharedPointer<Component> theirComponent;

        // We must find the API interfaces of the both ends, and the software instance and component of the
        // end that is NOT us.
        if ( softInstance->getInstanceName() == connection->getInterface1()->getComponentReference() )
        {
            ourInterface = softComponent->getApiInterface( connection->getInterface1()->getBusReference() );
            theirComponent = searchSWComponent(library, design, connection->getInterface2()->getComponentReference(), theirInstance);

            theirInterface = theirComponent->getApiInterface( connection->getInterface2()->getBusReference() );
        }

        if ( softInstance->getInstanceName() == connection->getInterface2()->getComponentReference() )
        {
            ourInterface = softComponent->getApiInterface( connection->getInterface2()->getBusReference() );
            theirComponent = searchSWComponent(library, design, connection->getInterface1()->getComponentReference(), theirInstance);

            theirInterface = theirComponent->getApiInterface( connection->getInterface1()->getBusReference() );
        }

        // If both ends are found and are pointing the right way, we may parse the files of its active view.
        if ( !ourInterface.isNull() && ourInterface->getDependencyDirection() == DEPENDENCY_REQUESTER && 
            !theirInterface.isNull() && theirInterface->getDependencyDirection() == DEPENDENCY_PROVIDER &&
			!theirInstance.isNull() )
        {
			// They must have an active view!
			QString softViewName = desgConf->getActiveView(theirInstance->getInstanceName());
			QSharedPointer<SWView> theirView;

			foreach( QSharedPointer<SWView> joku, theirComponent->getSWViews() )
			{
				if ( joku->name() == softViewName )
				{
					theirView = joku;
					break;
				}
			}

			// Null means no action.
			if ( !theirView )
			{
				break;
			}

            parseStackObjects( theirComponent, theirView, theirInstance, design, library, desgConf, makeData );
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::parseMakeObjects()
//-----------------------------------------------------------------------------
void MakefileParser::parseMakeObjects(LibraryInterface* library, QSharedPointer<SWView> view, QString instanceName,
    QSharedPointer<Component> component, MakeFileData &makeData, QList<QSharedPointer<MakeObjectData> >& objects, bool pickSWView)
{
    // Go through the fileSets referenced in the software view.
    foreach( QString fsetName, view->getFileSetRefs())
    {
        QSharedPointer<FileSet> fset = component->getFileSet(fsetName);

		// Skip, if no such file set exist!
		if ( fset )
		{
			parseFileSet(fset, instanceName, objects, library, component, makeData, pickSWView, view);
		}
    }
}

void MakefileParser::parseFileSet(QSharedPointer<FileSet> fset, QString instanceName, QList<QSharedPointer<MakeObjectData> > &objects,
	LibraryInterface* library, QSharedPointer<Component> component, MakeFileData &makeData, bool pickSWView, QSharedPointer<SWView> view)
{
	// Go through the files in the fileSet.
	foreach( QSharedPointer<File> file, *fset->getFiles())
	{
		// Initialize the data to the collection associated with the makefile data.
		QSharedPointer<MakeObjectData> objectData( new MakeObjectData );
		objects.append(objectData);

		// We may assume that a file path is relative to the component path, and thus the include path
		// is component path + file path.
		QFileInfo componentQfi = QFileInfo(library->getPath(component->getVlnv()));
		QFileInfo fileQfi = QFileInfo(componentQfi.absolutePath() + "/" + file->name());

		// Set the needed fields.
		objectData->file = file;
		objectData->fileBuildCmd = file->getBuildCommand();
		objectData->fileName = fileQfi.fileName();
		objectData->path = fileQfi.absolutePath();
		objectData->instanceName = instanceName;
		objectData->fileSet = fset;

		// In case of an include file:
		if ( file->isIncludeFile() )
		{
			// Its path is added to the collection of the include paths.
			makeData.includeDirectories.append(fileQfi.absolutePath());
			// Itself is added to the collection of the include files.
			makeData.includeFiles.append(objectData);
		}

		// A fileSet builder associated with the file type is also a possible field.
		foreach( QSharedPointer<FileBuilder> builder, *fset->getDefaultFileBuilders() )
		{
			if ( file->getFileTypes()->contains( builder->getFileType() ) )
			{
				objectData->fileSetBuildCmd = builder;
				break;
			}
		}

		if( pickSWView )
		{
			// Find build command of matching file type from the software view.
			foreach( QSharedPointer<SWBuildCommand> buildCmd, *view->getSWBuildCommands() )
			{
				if ( file->getFileTypes()->contains( buildCmd->getFileType() ) )
				{
					objectData->swBuildCmd = buildCmd;

					// If found, append the flags of the software view for later use.
					makeData.softViewFlags.append(buildCmd->getFlags());

					break;
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::searchSWComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> MakefileParser::searchSWComponent(LibraryInterface* library,
    QSharedPointer<const Design> design, QString instanceName, QSharedPointer<SWInstance>& targetInstance)
{
    // Go through the software instances of the design, finding the right one.
    foreach ( QSharedPointer<SWInstance> instance, design->getSWInstances() )
    {
        // If the found instance name is same as target, it is what we seek.
        if ( instance->getInstanceName() == instanceName )
        {
            targetInstance = instance;
            break;
        }
    }

    // Obtain the component of the instance.
    QSharedPointer<VLNV> instanceVLNV = targetInstance->getComponentRef();
    QSharedPointer<Document> instanceLibComp = library->getModel(*instanceVLNV);
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
    foreach(QSharedPointer<MakeObjectData> mod, makeData.swObjects)
    {
        // Find build command from the software view of the hardware component matching file type.
        foreach( QSharedPointer<SWBuildCommand> buildCmd, *hardView->getSWBuildCommands() )
        {
            if ( mod->file->getFileTypes()->contains( buildCmd->getFileType() ) )
            {
                makeData.hwBuildCmd = buildCmd;
                break;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MakefileParser::getFileCompiler()
//-----------------------------------------------------------------------------
QString MakefileParser::getFileCompiler(QSharedPointer<MakeObjectData> mod, QSharedPointer<SWView> hardView) const
{
	QString compiler;
	QSharedPointer<SWBuildCommand> swbc;

	// This mesh does following:
	// 1. No file builder -> use fileSet builder
	// 2. No fileSet builder -> use builder of the software view of the software instance
	// 3. If nothing else, use the builder of the software view of the hardware instance
	// 4. However, if the builder of latter two is not off matching file type, it is ignored.
	if ( mod->fileBuildCmd == 0 || mod->fileBuildCmd->getCommand().isEmpty() )
	{
		if ( mod->fileSetBuildCmd == 0 || mod->fileSetBuildCmd->getCommand().isEmpty() )
		{
			if ( mod->swBuildCmd == 0 || mod->swBuildCmd->getCommand().isEmpty() )
			{
				// Find build command from the software view of the hardware component matching file type.
				foreach( QSharedPointer<SWBuildCommand> buildCmd, *hardView->getSWBuildCommands() )
				{
					if ( mod->file->getFileTypes()->contains( buildCmd->getFileType() ) )
					{
						swbc = buildCmd;
						break;
					}
				}
			}
			else
			{
				swbc = mod->swBuildCmd;
			}
		}
		else if ( mod->fileSetBuildCmd != 0 )
		{
			compiler = mod->fileSetBuildCmd->getCommand();
		}
	}
	else
	{
		compiler = mod->fileBuildCmd->getCommand();
	}

	// Verify that file has a type matching the build command.
	if ( swbc != 0 && mod->file->getFileTypes()->contains( swbc->getFileType() ) )
	{
		compiler = swbc->getCommand();
	}

	return compiler;
}

//-----------------------------------------------------------------------------
// Function: MakefileGenerator::getFileFlags()
//-----------------------------------------------------------------------------
QString MakefileParser::getFileFlags(QSharedPointer<Component> component, QSharedPointer<MakefileParser::MakeObjectData> &mod, MakefileParser::MakeFileData &mfd) const
{
	QString cFlags;
	QString fileReplaceFlags;
	QString fileSetReplaceFlags;

	QSharedPointer<ListParameterFinder> finder (new ListParameterFinder());
	finder->setParameterList(component->getParameters());
	QSharedPointer<IPXactSystemVerilogParser> expressionParser (new IPXactSystemVerilogParser(finder));
	
	// At any rate, the file may have its own flags.
	if ( mod->fileBuildCmd != 0 )
	{
		cFlags += mod->fileBuildCmd->getFlags();
		fileReplaceFlags = expressionParser->parseExpression(mod->fileBuildCmd->getReplaceDefaultFlags());
	}

	// See if file set flags replace anything.
	if ( mod->fileSetBuildCmd != 0 )
	{
		fileSetReplaceFlags = expressionParser->parseExpression(mod->fileSetBuildCmd->getReplaceDefaultFlags());
	}

	// This mesh does following:
	// 1. If file does not override flags, may use fileSet flags
	// 2. If fileSet does not override flags, may use software flags
	// 2. If software does not override flags, may use hardware flags
	if ( mod->fileBuildCmd == 0 || fileReplaceFlags != "true" )
	{
		if ( mod->fileSetBuildCmd != 0 )
		{
			cFlags += " " + mod->fileSetBuildCmd->getFlags();
		}

		if ( mod->fileSetBuildCmd == 0 || fileReplaceFlags != "true" )
		{
			if ( mod->swBuildCmd != 0 )
			{
				cFlags += " " + mod->swBuildCmd->getFlags();
			}   

			if ( ( mod->swBuildCmd == 0 || !mod->swBuildCmd->getReplaceDefaultFlags() ) && mfd.hwBuildCmd != 0 )
			{
				cFlags += " " + mfd.hwBuildCmd->getFlags();
			}
		}
	}

	return cFlags;
}
