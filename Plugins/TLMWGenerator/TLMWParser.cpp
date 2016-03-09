//-----------------------------------------------------------------------------
// File: TLMWParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 13.7.2015
//
// Description:
// TLMW parser.
//-----------------------------------------------------------------------------

#include "TLMWParser.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QMessageBox>

#include <editors/CSourceEditor/CSourceTextEdit.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/kactusExtensions/ComInterface.h>
#include <IPXACTmodels/kactusExtensions/SystemView.h>

//-----------------------------------------------------------------------------
// Function: TLMWParser::TLMWParser()
//-----------------------------------------------------------------------------
TLMWParser::TLMWParser(IPluginUtility* utility) : utility_(utility)
{
}

//-----------------------------------------------------------------------------
// Function: TLMWParser::~TLMWParser()
//-----------------------------------------------------------------------------
TLMWParser::~TLMWParser()
{
}

//-----------------------------------------------------------------------------
// Function: TLMWParser::getComponentEndpoints()
//-----------------------------------------------------------------------------
QList<TLMWParser::EndPointData> TLMWParser::getComponentEndpoints() const
{
    return componentEndpoints_;
}

//-----------------------------------------------------------------------------
// Function: TLMWParser::getDesignNodes()
//-----------------------------------------------------------------------------
QList<TLMWParser::NodeData> TLMWParser::getDesignNodes() const
{
    return designNodes_;
}

//-----------------------------------------------------------------------------
// Function: TLMWParser::getReplacedFiles()
//-----------------------------------------------------------------------------
QStringList TLMWParser::getReplacedFiles() const
{
    return replacedFiles_;
}

//-----------------------------------------------------------------------------
// Function: TLMWParser::parseTLMWForComponent()
//-----------------------------------------------------------------------------
void TLMWParser::parseTLMWForComponent(QSharedPointer<Component> component)
{
    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() == "tlmw")
        {
            componentEndpoints_.append(EndPointData());
            EndPointData& epd = componentEndpoints_.last();

            parseEndpoint(epd, comIf);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: TLMWParser::parseTopLevel()
//-----------------------------------------------------------------------------
void TLMWParser::parseTopLevel(QSharedPointer<Design> design, QSharedPointer<Component> topComponent,
    QSharedPointer<DesignConfiguration const> desgConf)
{
    foreach (QSharedPointer<SWInstance> instance, design->getSWInstances())
    {
        VLNV instanceVLNV = *instance->getComponentRef();
        VLNV designVLNV = design->getVlnv();

        QSharedPointer<Document> instanceLibComp = utility_->getLibraryInterface()->getModel(instanceVLNV);
        QSharedPointer<Component> instanceComp = instanceLibComp.dynamicCast<Component>();

        // Check if can generate the component, return if cannot
        if (canGenerateTLMWComponent(instanceComp))
        {
            NodeData nd;
            nd.instance = instance;

			// Find the name of the system view pointing to the design configuration.
			QString sysViewName;

			foreach (QSharedPointer<SystemView> view, topComponent->getSystemViews())
			{
				if (view->getHierarchyRef() == desgConf->getVlnv())
				{
					sysViewName = view->name();
					break;
				}
			}

            // Parse its directory as well.
            QString subDir = "/sw_" + sysViewName + "/" + instance->getInstanceName();
            QString dir = QFileInfo(utility_->getLibraryInterface()->getPath(designVLNV)).absolutePath() + subDir;
            nd.directory = dir;

            QFile file(dir + "/TLMWheader.h");

            // If does, put in the list.
            if (file.exists())
            {
                replacedFiles_.append(dir + "/TLMWheader.h");
            }

			findEndpointDefinitions(design, instance, instanceComp, nd);

			designNodes_.append(nd);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: TLMWParser::canGenerateTLMWComponent()
//-----------------------------------------------------------------------------
bool TLMWParser::canGenerateTLMWComponent(QSharedPointer<Component> component)
{
    // Will not generate a null component.
    if (component == 0)
    {
        return false;
    }

    // Errors found in interfaces.
    QStringList errorList;
    // Must have at least one TLMW interface to generate.
    bool hasTLMW = false;

    // Go over each com interface and check for that each required property is set.
    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        // Check API support for generated interface
        if (comIf->getComType().getName().toLower() == "tlmw")
        {
            hasTLMW = true;
        }

        QSharedPointer<Document> libCom = utility_->getLibraryInterface()->getModel(comIf->getComType());
        QSharedPointer<ComDefinition> comDef = libCom.dynamicCast<ComDefinition>();

		// Also check that required properties are set.
		if (comDef != NULL)
		{
			checkRequiredPropertiesSet(component->getVlnv().toString(), comDef, comIf, errorList);
		}
		else
		{
			errorList.append(QObject::tr("Component '%3' had a null com definition.").arg(
                component->getVlnv().toString()));
		}
    }

    // If errors exist, print about it and return false.
    if (hasTLMW && !errorList.isEmpty())
    {
        foreach (QString const& msg, errorList)
        {
            utility_->printError(msg);
        }

        QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
            QObject::tr("The component contained %1 error(s). TLMW header was not generated.").arg(errorList.size()),
            QMessageBox::Ok, utility_->getParentWidget());
        msgBox.setDetailedText(QObject::tr("The following error(s) were found: \n* ") + errorList.join("\n* "));

        msgBox.exec();
    }

    // If no errors, we can generate the component.
    return errorList.isEmpty() && hasTLMW;
}

//-----------------------------------------------------------------------------
// Function: TLMWParser::checkRequiredPropertiesSet()
//-----------------------------------------------------------------------------
void TLMWParser::checkRequiredPropertiesSet(QString const& componentVLNV, QSharedPointer<ComDefinition> comDef,
    QSharedPointer<ComInterface> comIf, QStringList& errorList)
{
    foreach (QSharedPointer<ComProperty> property, *comDef->getProperties())
    {
        if (property->isRequired() && comIf->getPropertyValues().value(property->name()).isEmpty())
        {
            errorList.append(QObject::tr("Property %1 of COM interface '%2' is not set in component '%3'").arg(
                property->name(), comIf->name(), componentVLNV));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: TLMWParser::findEndpointDefinitions()
//-----------------------------------------------------------------------------
void TLMWParser::findEndpointDefinitions(QSharedPointer<const Design> design, QSharedPointer<SWInstance> ourInstance, 
    QSharedPointer<Component> ourComponent, NodeData& nodeData)
{
	// Go through the list of connections in the design to retrieve remote endpoint identifiers.
	foreach (QSharedPointer<ComInterconnection> connection, design->getComConnections())
	{
		QSharedPointer<ComInterface> ourInterface;
		QSharedPointer<ActiveInterface> theirInterfaceRef;

        if (ourInstance->getInstanceName() == connection->getStartInterface()->getComponentReference())
        {
            ourInterface = ourComponent->getComInterface(connection->getStartInterface()->getBusReference());
        }
        else
        {
            theirInterfaceRef = connection->getStartInterface();
        }

        foreach(QSharedPointer<ActiveInterface> activeInterface, *connection->getActiveInterfaces())
        {
            if (ourInstance->getInstanceName() == activeInterface->getComponentReference())
            {
                ourInterface = ourComponent->getComInterface(activeInterface->getBusReference());
            }
            else
            {
                theirInterfaceRef = activeInterface;
            }
        }

		// Search for software instance corresponding theirs.
		QSharedPointer<SWInstance> targetInstance = searchInstance(design, theirInterfaceRef->getComponentReference());

        // Obtain the component object corresponding the software instance.
        VLNV instanceVLNV = *targetInstance->getComponentRef();
        QSharedPointer<Document> instanceLibComp = utility_->getLibraryInterface()->getModel(instanceVLNV);
        QSharedPointer<Component> theirComponent = instanceLibComp.dynamicCast<Component>();

        if (theirComponent)
        {
            QSharedPointer<ComInterface> theirInterface = theirComponent->getComInterface(theirInterfaceRef->getBusReference());

            // Must exist to generate.
            if (ourInterface != 0 && theirInterface != 0)
            {
                // Check for sanity.
                checkTransferSize(ourInterface, theirInterface, ourInstance, targetInstance);

                // The data needed for "our" endpoint.
                EndPointData ourEnd;
                parseEndpoint(ourEnd, ourInterface);

                // Create pair: our endpoint and connection name and add it to the list.
                QPair<EndPointData,QString> pair(ourEnd,connection->name());
                nodeData.connections.append(pair);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: TLMWParser::parseEndpoint()
//-----------------------------------------------------------------------------
void TLMWParser::parseEndpoint(EndPointData &epd, QSharedPointer<ComInterface> comIf)
{
    epd.name = comIf->name();
	epd.transferSize = comIf->getPropertyValues().value("max_transfer_size");
}

//-----------------------------------------------------------------------------
// Function: TLMWParser::searchInstance()
//-----------------------------------------------------------------------------
QSharedPointer<SWInstance> TLMWParser::searchInstance(QSharedPointer<const Design> design, QString instanceName)
{
    foreach (QSharedPointer<SWInstance> instance, design->getSWInstances())
    {
        // If the found instance name is same as target, will generate for it.
        if (instance->getInstanceName() == instanceName)
        {
            return instance;
        }
    }

    return QSharedPointer<SWInstance>();
}

//-----------------------------------------------------------------------------
// Function: TLMWParser::checkTransferSize()
//-----------------------------------------------------------------------------
void TLMWParser::checkTransferSize(QSharedPointer<ComInterface> ourInterface, 
    QSharedPointer<ComInterface> targetInterface, QSharedPointer<SWInstance> ourInstance, 
    QSharedPointer<SWInstance> targetInstance)
{
    QString ourTransferSize = ourInterface->getPropertyValues().value("max_transfer_size");
    QString theirTransferSize = targetInterface->getPropertyValues().value("max_transfer_size");

    if (ourTransferSize != theirTransferSize)
    {
        utility_->printError("Maximum transfer sizes of connected ports did not match! "
            "First instance: " + ourInstance->getInstanceName() + " first interface: " +
            ourInterface->name() + " Second instance: " + targetInstance->getInstanceName() + 
            " second interface " + targetInterface->name());
    }
}
