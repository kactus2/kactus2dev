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

#include <common/NameGenerationPolicy.h>

#include <KactusAPI/include/ListParameterFinder.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>

//-----------------------------------------------------------------------------
// Function: SWStackParser::MakefileParser()
//-----------------------------------------------------------------------------
SWStackParser::SWStackParser(LibraryInterface* library, QSharedPointer<Component> topComponent,
    QSharedPointer<Design> design, QSharedPointer<DesignConfiguration> designConf) :
    library_(library),
    topComponent_(topComponent),
    design_(design),
    designConf_(designConf),
    parsedData_(QSharedPointer<QList<QSharedPointer<MakeFileData> > >(new QList<QSharedPointer<MakeFileData> >)),
    generalFileSet_()
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
QSharedPointer<FileSet> SWStackParser::getGeneralFileSet()
{
    return generalFileSet_;
}

//-----------------------------------------------------------------------------
// Function: SWStackParser::parse()
//-----------------------------------------------------------------------------
void SWStackParser::parse(QString const& sysViewName)
{
    // Fabricate name for the fileSet of design configuration specific files.
    QString generalFileSetName = NameGenerationPolicy::systemViewFilesetName(sysViewName);

	// Obtain the the fileSet by name and set it as a source file group.
	QSharedPointer<FileSet> fileSet = topComponent_->getFileSet(generalFileSetName);

	if (!fileSet)
	{
		QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets = topComponent_->getFileSets();
		fileSet = QSharedPointer<FileSet>(new FileSet(generalFileSetName, "sourceFiles"));
		fileSets->append(fileSet);
	}

    fileSet->setGroups("sourceFiles");

    // This is also a fileSet referenced by the makefile generation
    generalFileSet_ = fileSet;

    // The base directory for the software.
    QString basePath = sysViewName + "/";

    // Check if the main makefile and the launcher files already exits.
    QString mainMakeDir = basePath + "Makefile";

    // We also need to know if the file exists
    QString launcherDir = basePath + "launcher.sh";

    foreach (QSharedPointer<ComponentInstance> softInstance, *design_->getComponentInstances())
    {
        // The VLNV and the component of the instance are needed.
        QSharedPointer<VLNV> softwareVLNV = softInstance->getComponentRef();
        QSharedPointer<Component> softComponent = library_->getModel(*softwareVLNV).dynamicCast<Component>();

        // Lower levels of a stack do not get their own makefiles.
        if (!isTopOfStack(softInstance, softComponent))
        {
			continue;
		}

        // Initialize the data of makefile to collection of parsed entities.
        QSharedPointer<MakeFileData> makeData(new MakeFileData);

		// Parse files of the underlying hardware.
		QSharedPointer<ComponentInstance> hardInstance;

		// See if the design contains a hardware instance corresponding the mapped hardware instance.
		foreach (QSharedPointer<ComponentInstance> instance, *design_->getComponentInstances())
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
		QString hardViewName = QString::fromStdString(designConf_->getActiveView(hardInstance->getInstanceName().toStdString()));
		QSharedPointer<View> hardView = hardComponent->getModel()->findView(hardViewName);

		// If not found, skip.
		if (!hardView)
		{
			continue;
        }

        // Must have a component instantiation.
        QSharedPointer<ComponentInstantiation> hardInstantiation = hardComponent->getModel()->
            findComponentInstantiation(hardView->getComponentInstantiationRef());

        if (!hardInstantiation)
        {
            return;
        }

		// Get the hardware data.
		makeData->hardPart = QSharedPointer<StackPart>(new StackPart);
		makeData->hardPart->component = hardComponent;
		makeData->hardPart->instanceName = hardInstance->getInstanceName();
		makeData->hardPart->view = hardView;
        makeData->hardPart->instantiation = hardInstantiation;

		// This is also the point where hardware build command is decided.
		// TODO: Ask from user which one.
		if (hardInstantiation->getDefaultFileBuilders()->count() > 0)
		{
			makeData->hardPart->buildCmd = hardInstantiation->getDefaultFileBuilders()->first();
		}

		// No hardware build command means no makefile.
		if (!makeData->hardPart->buildCmd)
		{
			continue;
		}

		// Parse its files, as well as files of any instance in the underlying stack.
		parseStackObjects(softComponent, softInstance, makeData, sysViewName);

		// Empty stack means no makefile.
		if (makeData->parts.count() < 1)
		{
			continue;
		}

		// Now add the hardware to the list of parts as well.
		makeData->parts.append(makeData->hardPart);

		// We need a file set for makefile. It shall be the header set of the topmost instance.
		makeData->instanceFileSet = makeData->parts.first()->instanceHeaders;

		// Since every software stack gets its own makefile, naming is after the instance name.
		makeData->name = makeData->parts.first()->instanceName;

		// We a name for the file, possibly via subfolder,
		makeData->makeName = makeData->name + "/" + makeData->name + ".mak";

		// Finally, append to the list of parsed stuff.
		parsedData_->append(makeData);
	}
}

//-----------------------------------------------------------------------------
// Function: SWStackParser::isTopOfStack()
//-----------------------------------------------------------------------------
bool SWStackParser::isTopOfStack(QSharedPointer<ComponentInstance> softInstance, QSharedPointer<Component> softComponent)
{
    foreach (QSharedPointer<ApiInterconnection> connection, design_->getApiConnections())
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
void SWStackParser::parseStackObjects(QSharedPointer<Component> softComponent,
    QSharedPointer<ComponentInstance> softInstance, QSharedPointer<MakeCommon::MakeFileData> makeData, 
    QString const& systemViewName)
{
    // Skip if already parsed
    if (makeData->parsedInstances.contains( softInstance ))
    {
        return;
	}

	// There may be only one active software view.
	QString softViewName = QString::fromStdString(designConf_->getActiveView(softInstance->getInstanceName().toStdString()));

    // It must correspond an actual view in the component.
	QSharedPointer<View> softView = softComponent->getModel()->findView(softViewName);

	if (!softView)
	{
		return;
	}

    // Must have a component instantiation.
    QSharedPointer<ComponentInstantiation> softInstantiation = softComponent->getModel()->
        findComponentInstantiation(softView->getComponentInstantiationRef());

    if (!softInstantiation)
    {
        return;
    }

	// Find build command of matching file type from the software view.
	QSharedPointer<FileBuilder> softViewBuildCmd;

	foreach(QSharedPointer<FileBuilder> buildCmd, *softInstantiation->getDefaultFileBuilders())
	{
		if (buildCmd->getFileType() == makeData->hardPart->buildCmd->getFileType())
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
    stackPart->instantiation = softInstantiation;
	stackPart->buildCmd = softViewBuildCmd;

	// Add to the lists.
	makeData->parts.append(stackPart);
	makeData->parsedInstances.append(softInstance);

	// Its flags are to be used.
	makeData->componentInstantiationFlags.append(softViewBuildCmd->getFlags());

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
	QSharedPointer<FileSet> fileSet = topComponent_->getFileSet(fileSetName);

	// Create and add to the top component if needed.
	if (!fileSet)
	{
		fileSet = QSharedPointer<FileSet>(new FileSet(fileSetName, "sourceFiles"));
		topComponent_->getFileSets()->append(fileSet);
	}

	// Save for later use.
	stackPart->instanceHeaders = fileSet;

    // Go through the list of connections in the design to retrieve remote endpoint identifiers.
    foreach (QSharedPointer<ApiInterconnection> connection, design_->getApiConnections())
    {
        QSharedPointer<ApiInterface> ourInterface;
        QSharedPointer<ApiInterface> theirInterface;
        QSharedPointer<ComponentInstance> theirInstance;
        QSharedPointer<Component> theirComponent;

        // We must find the API interfaces of the both ends, and the software instance and component of the
        // end that is NOT us.
        if (softInstance->getInstanceName() == connection->getStartInterface()->getComponentReference())
        {
            ourInterface = softComponent->getApiInterface(connection->getStartInterface()->getBusReference());
        }
        else
        {
            theirComponent = searchSWComponent(connection->getStartInterface()->getComponentReference(), theirInstance);
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
                theirComponent = searchSWComponent(activeInterface->getComponentReference(), theirInstance);
                theirInterface = theirComponent->getApiInterface(activeInterface->getBusReference());
            }
        }

        // If both ends are found and are pointing the right way, we may include it to the stack.
        if (!ourInterface.isNull() && ourInterface->getDependencyDirection() == DEPENDENCY_REQUESTER && 
            !theirInterface.isNull() && theirInterface->getDependencyDirection() == DEPENDENCY_PROVIDER &&
			!theirInstance.isNull())
        {
			parseStackObjects(theirComponent, theirInstance, makeData, systemViewName);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SWStackParser::searchSWComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> SWStackParser::searchSWComponent(QString instanceName,
    QSharedPointer<ComponentInstance>& targetInstance)
{
    // Go through the software instances of the design, finding the right one.
    foreach (QSharedPointer<ComponentInstance> instance, *design_->getComponentInstances())
    {
        // If the found instance name is same as target, it is what we seek.
        if (instance->getInstanceName() == instanceName)
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