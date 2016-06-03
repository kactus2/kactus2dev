//-----------------------------------------------------------------------------
// File: SWStackParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 18.05.2016
//
// Description:
// Parses design and its configuration for makefile generation data.
//-----------------------------------------------------------------------------

#include "SWStackParser.h"

#include <QFileInfo>
#include <QSet>

#include <IPXACTmodels/kactusExtensions/ApiInterface.h>

#include "editors/ComponentEditor/common/ListParameterFinder.h"
#include "editors/ComponentEditor/common/IPXactSystemVerilogParser.h"

#include <Plugins/common/NameGenerationPolicy.h>

//-----------------------------------------------------------------------------
// Function: SWStackParser::MakefileParser()
//-----------------------------------------------------------------------------
SWStackParser::SWStackParser( LibraryInterface* library ) : library_( library ), 
	parsedData_( QSharedPointer<QList<QSharedPointer<MakeFileData> > >
	( new QList<QSharedPointer<MakeFileData> > ) )
{
}

//-----------------------------------------------------------------------------
// Function: SWStackParser::~MakefileParser()
//-----------------------------------------------------------------------------
SWStackParser::~SWStackParser()
{
}

//-----------------------------------------------------------------------------
// Function: SWStackParser::getParsedData()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<MakeFileData> > > SWStackParser::getParsedData()
{
    return parsedData_;
}

//-----------------------------------------------------------------------------
// Function: SWStackParser::getGeneralFileSet()
//-----------------------------------------------------------------------------
const QSharedPointer<FileSet>& SWStackParser::getGeneralFileSet()
{
    return generalFileSet_;
}

//-----------------------------------------------------------------------------
// Function: SWStackParser::getReplacedFiles()
//-----------------------------------------------------------------------------
const QStringList& SWStackParser::getReplacedFiles()
{
    return replacedFiles_;
}

//-----------------------------------------------------------------------------
// Function: SWStackParser::parse()
//-----------------------------------------------------------------------------
void SWStackParser::parse(QSharedPointer<Component> topComponent,
    QSharedPointer<DesignConfiguration const> desgConf, QSharedPointer<const Design> design,
    QString sysViewName, QString targetPath)
{
    // Fabricate name for the fileSet of design configuration specific files.
    QString generalFileSetName = NameGenerationPolicy::systemViewFilesetName(sysViewName);

	// Obtain the the fileSet by name and set it as a source file group.
	QSharedPointer<FileSet> fileSet = topComponent->getFileSet(generalFileSetName);

	if (!fileSet)
	{
		QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets = topComponent->getFileSets();
		fileSet = QSharedPointer<FileSet>(new FileSet(generalFileSetName, "sourceFiles"));
		fileSets->append(fileSet);
	}

    fileSet->setGroups("sourceFiles");

    // This is also a fileSet referenced by the makefile generation
    generalFileSet_ = fileSet;

    // The base directory for the software.
    QString basePath = targetPath + "/sw_" + sysViewName + "/";

    // Check if the main makefile and the launcher files already exits.
    QString mainMakeDir = basePath + "Makefile";
    QFile makeFile(mainMakeDir);

    if (makeFile.exists())
    {
        replacedFiles_.append(mainMakeDir);
    }

    // We also need to know if the file exists
    QString launcherDir = basePath + "launcher.sh";
    QFile launcherFile(launcherDir);

    if (launcherFile.exists())
    {
        replacedFiles_.append(launcherDir);
	}

    foreach (QSharedPointer<SWInstance> softInstance, design->getSWInstances())
    {
        // The VLNV and the component of the instance are needed.
        QSharedPointer<VLNV> softwareVLNV = softInstance->getComponentRef();
        QSharedPointer<Component> softComponent = library_->getModel(*softwareVLNV).dynamicCast<Component>();

        // Lower levels of a stack do not get their own makefiles.
        if (!isTopOfStack(design, softInstance, softComponent))
        {
			continue;
		}

        // Initialize the data of makefile to collection of parsed entities.
        QSharedPointer<MakeFileData> makeData( new MakeFileData );

		// Parse files of the underlying hardware.
		QSharedPointer<ComponentInstance> hardInstance;

		// See if the design contains a hardware instance corresponding the mapped hardware instance.
		foreach (QSharedPointer<ComponentInstance> instance, *design->getComponentInstances())
		{
			if (instance->getUuid() == softInstance->getMapping())
			{
				hardInstance = instance;
				break;
			}
		}

		// If not found, skip.
		if (!hardInstance)
		{
			continue;
		}

		// Get the component of the hardware instance.
		QSharedPointer<VLNV> hardwareVLNV = hardInstance->getComponentRef();
		QSharedPointer<Component> hardComponent = library_->getModel(*hardwareVLNV).dynamicCast<Component>();

		// Find the view corresponding the active view name
		QString hardViewName = desgConf->getActiveView(hardInstance->getInstanceName());
		QSharedPointer<SWView> hardView;
		foreach(QSharedPointer<SWView> view, hardComponent->getSWViews())
		{
			if (view->name() == hardViewName)
			{
				hardView = view;
				break;
			}
		}

		// If not found, skip.
		if (!hardView)
		{
			continue;
		}

		// They want the name of the instance.
		QString instanceName = hardInstance->getDisplayName();
		if (instanceName.isEmpty())
		{
			instanceName = hardInstance->getInstanceName();
		}

		// Get the hardware data.
		makeData->hardPart = QSharedPointer<StackPart>( new StackPart );
		makeData->hardPart->component = hardComponent;
		makeData->hardPart->instanceName = hardInstance->getInstanceName();
		makeData->hardPart->view = hardView;

		// This is also the point where hardware build command is decided.
		// TODO: Ask from user which one.
		if ( hardView->getSWBuildCommands()->count() > 0 )
		{
			makeData->hardPart->buildCmd = hardView->getSWBuildCommands()->first();
		}

		// No hardware build command means no makefile.
		if ( !makeData->hardPart->buildCmd )
		{
			continue;
		}

		// Parse its files, as well as files of any instance in the underlying stack.
		parseStackObjects(topComponent, softComponent, softInstance, design, desgConf, makeData, sysViewName);

		// Empty stack means no makefile.
		if ( makeData->parts.count() < 1 )
		{
			continue;
		}

		// Now add the hardware to the list of parts as well.
		makeData->parts.append( makeData->hardPart );

		// We need a file set for makefile. It shall be the header set of the topmost instance.
		makeData->instanceHeaders = makeData->parts.first()->instanceHeaders;

		// Since every software stack gets its own makefile, naming is after the instance name.
		makeData->name = makeData->parts.first()->instanceName;

		// We need the absolute path of the file.
		makeData->targetPath = basePath + makeData->name;
		QString filePath = makeData->targetPath + "/Makefile";

		// We also need to know if the file exists.
		QFile makeFile(filePath);

		// If it does, put in the list: We have to warn user for overriding.
		if (makeFile.exists())
		{
			replacedFiles_.append(filePath);
		}

		// Finally, append to the list of parsed stuff.
		parsedData_->append(makeData);
	}
}

//-----------------------------------------------------------------------------
// Function: SWStackParser::isTopOfStack()
//-----------------------------------------------------------------------------
bool SWStackParser::isTopOfStack(QSharedPointer<const Design> design, QSharedPointer<SWInstance> softInstance,
	QSharedPointer<Component> softComponent)
{
    foreach (QSharedPointer<ApiInterconnection> connection, design->getApiConnections())
    {
        QSharedPointer<ApiInterface> ourInterface;

        if (softInstance->getInstanceName() == connection->getStartInterface()->getComponentReference())
        {
            ourInterface = softComponent->getApiInterface(connection->getStartInterface()->getBusReference());
        }
        else
        {
            // See if one of the ends is ours.
            foreach (QSharedPointer<ActiveInterface> activeInterface, *connection->getActiveInterfaces())
            {
                if (softInstance->getInstanceName() == activeInterface->getComponentReference())
                {
                    ourInterface = softComponent->getApiInterface(activeInterface->getBusReference());
                    break;
                }
            }
        }

        // If found and it turns out that we are an active provider, we are not on the top of a stack.
        if (!ourInterface.isNull() && ourInterface->getDependencyDirection() == DEPENDENCY_PROVIDER)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: SWStackParser::parseStackObjects()
//-----------------------------------------------------------------------------
void SWStackParser::parseStackObjects(QSharedPointer<Component> topComponent,
	QSharedPointer<Component> softComponent, QSharedPointer<SWInstance> softInstance,
	QSharedPointer<const Design> design, QSharedPointer<DesignConfiguration const> desgConf,
	QSharedPointer<MakeFileData> makeData, QString& systemViewName)
{
    // Skip if already parsed
    if ( makeData->parsedInstances.contains( softInstance ) )
    {
        return;
	}

	// There may be only one active software view.
	QString softViewName = desgConf->getActiveView(softInstance->getInstanceName());

	// Fetch it from the component.
	QSharedPointer<SWView> softView;
	foreach(QSharedPointer<SWView> view, softComponent->getSWViews())
	{
		if (view->name() == softViewName)
		{
			softView = view;
			break;
		}
	}

	// If it does not exist, no further processing may done.
	if (!softView)
	{
		return;
	}

	// Find build command of matching file type from the software view.
	QSharedPointer<SWFileBuilder> softViewBuildCmd;

	foreach( QSharedPointer<SWFileBuilder> buildCmd, *softView->getSWBuildCommands() )
	{
		if ( buildCmd->getFileType() == makeData->hardPart->buildCmd->getFileType() )
		{
			softViewBuildCmd = buildCmd;

			break;
		}
	}

	// If no suitable command exists, the software view is unusable for us.
	if (!softViewBuildCmd)
	{
		return;
	}

	// Add the instance as a new part of the stack.
	QSharedPointer<StackPart> stackPart = QSharedPointer<StackPart>( new StackPart );
	stackPart->instanceName = softInstance->getInstanceName();
	stackPart->component = softComponent;
	stackPart->view = softView;
	stackPart->buildCmd = softViewBuildCmd;

	// Add to the lists.
	makeData->parts.append(stackPart);
	makeData->parsedInstances.append(softInstance);

	// Its flags are to be used.
	makeData->softViewFlags.append(softViewBuildCmd->getFlags());

	// The top component of the design may contain header files specific to the instance.
	// The path leading to the design.
	QString fileSetName = softInstance->getFileSetRef();

	// Create a new fileSet, if no reference exist.
	if (fileSetName.isEmpty())
	{
		// If not, make a new one.
		fileSetName = NameGenerationPolicy::instanceFilesetName(systemViewName, softInstance->getInstanceName());
		softInstance->setFileSetRef(fileSetName);
	}

	// Obtain the the fileSet by name.
	QSharedPointer<FileSet> fileSet = topComponent->getFileSet(fileSetName);

	// Create and add to the top component if needed.
	if (!fileSet)
	{
		fileSet = QSharedPointer<FileSet>(new FileSet(fileSetName, "sourceFiles"));
		topComponent->getFileSets()->append(fileSet);
	}

	// Save for later use.
	stackPart->instanceHeaders = fileSet;

    // Go through the list of connections in the design to retrieve remote endpoint identifiers.
    foreach (QSharedPointer<ApiInterconnection> connection, design->getApiConnections())
    {
        QSharedPointer<ApiInterface> ourInterface;
        QSharedPointer<ApiInterface> theirInterface;
        QSharedPointer<SWInstance> theirInstance;
        QSharedPointer<Component> theirComponent;

        // We must find the API interfaces of the both ends, and the software instance and component of the
        // end that is NOT us.
        if (softInstance->getInstanceName() == connection->getStartInterface()->getComponentReference())
        {
            ourInterface = softComponent->getApiInterface(connection->getStartInterface()->getBusReference());
        }
        else
        {
            theirComponent = searchSWComponent(design,
				connection->getStartInterface()->getComponentReference(), theirInstance);
            theirInterface = theirComponent->getApiInterface(connection->getStartInterface()->getBusReference());
        }

        foreach(QSharedPointer<ActiveInterface> activeInterface, *connection->getActiveInterfaces())
        {
            if (softInstance->getInstanceName() == activeInterface->getComponentReference())
            {
                ourInterface = softComponent->getApiInterface(activeInterface->getBusReference());
            }
            else
            {
                theirComponent = searchSWComponent(design, activeInterface->getComponentReference(),
                    theirInstance);
                theirInterface = theirComponent->getApiInterface(activeInterface->getBusReference());
            }
        }

        // If both ends are found and are pointing the right way, we may include it to the stack.
        if (!ourInterface.isNull() && ourInterface->getDependencyDirection() == DEPENDENCY_REQUESTER && 
            !theirInterface.isNull() && theirInterface->getDependencyDirection() == DEPENDENCY_PROVIDER &&
			!theirInstance.isNull())
        {
			parseStackObjects(topComponent, theirComponent, theirInstance, design, desgConf, makeData,
				systemViewName);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SWStackParser::searchSWComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> SWStackParser::searchSWComponent( QSharedPointer<const Design> design,
	QString instanceName, QSharedPointer<SWInstance>& targetInstance)
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
    QSharedPointer<Document> instanceLibComp = library_->getModel(*instanceVLNV);
    QSharedPointer<Component> instanceComp = instanceLibComp.dynamicCast<Component>();

    return instanceComp;
}