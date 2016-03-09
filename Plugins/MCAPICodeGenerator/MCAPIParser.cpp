//-----------------------------------------------------------------------------
// File: MCAPIParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 13.10.2014
//
// Description:
// MCAPI parser.
//-----------------------------------------------------------------------------

#include "MCAPIParser.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QMessageBox>

#include <editors/CSourceEditor/CSourceTextEdit.h>

#include "common/dialogs/comboSelector/comboselector.h"

#include "IPXACTmodels/kactusExtensions/SWView.h"
#include "IPXACTmodels/kactusExtensions/SystemView.h"

#include <IPXACTmodels/Design/PortReference.h>

#include <library/LibraryManager/libraryinterface.h>


//-----------------------------------------------------------------------------
// Function: MCAPIParser::MCAPIParser()
//-----------------------------------------------------------------------------
MCAPIParser::MCAPIParser( IPluginUtility* utility ) : utility_( utility )
{
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::~MCAPIParser()
//-----------------------------------------------------------------------------
MCAPIParser::~MCAPIParser()
{
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::getComponentEndpoints()
//-----------------------------------------------------------------------------
QList<MCAPIParser::EndPointData> MCAPIParser::getComponentEndpoints()
{
    return componentEndpoints_;
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::getDesignNodes()
//-----------------------------------------------------------------------------
QList<MCAPIParser::NodeData> MCAPIParser::getDesignNodes()
{
    return designNodes_;
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::getReplacedFiles()
//-----------------------------------------------------------------------------
QStringList MCAPIParser::getReplacedFiles()
{
    return replacedFiles_;
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::parseMCAPIForComponent()
//-----------------------------------------------------------------------------
void MCAPIParser::parseMCAPIForComponent(QSharedPointer<Component> component)
{
    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() == "mcapi")
        {
            componentEndpoints_.append(parseEndpoint(comIf));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::parseTopLevel()
//-----------------------------------------------------------------------------
void MCAPIParser::parseTopLevel(QSharedPointer<Design> design, QSharedPointer<Component> topComponent,
    QSharedPointer<DesignConfiguration const> desgConf)
{
    QList<QSharedPointer<SWInstance> > instances = design->getSWInstances();

    foreach (QSharedPointer<SWInstance> instance, instances)
    {
        VLNV instanceVLNV = *instance->getComponentRef();
        VLNV designVLNV = design->getVlnv();

        QSharedPointer<Document> instanceLibComp = utility_->getLibraryInterface()->getModel(instanceVLNV);
        QSharedPointer<Component> instanceComp = instanceLibComp.dynamicCast<Component>();

        // Check if can generate the component, return if cannot
        if (canGenerateMCAPIComponent(instanceComp))
        {
            NodeData nd; 
            nd.instance = instance;

			// Find the name of the system view pointing to the design configuration.
			QString sysViewName;
			foreach ( QSharedPointer<SystemView> view, topComponent->getSystemViews() )
			{
                if ( view->getHierarchyRef() == desgConf->getVlnv() )
				{
					sysViewName = view->name();
					break;
				}
			}

            // Parse its directory as well.
            QString subDir = "/sw_" + sysViewName + "/" + instance->getInstanceName();
            QString dir = QFileInfo(utility_->getLibraryInterface()->getPath(designVLNV)).absolutePath() + subDir;
            nd.directory = dir;

            QFile file(dir + "/instanceheader.h");

            // If does, put in the list.
            if ( file.exists() )
            {
                replacedFiles_.append(dir + "/instanceheader.h");
            }

            // Mark up the node identifier, as well as the associated domain identifier.
            nd.nodeID = instance->getPropertyValues().value("Node ID");
            nd.domainID = instance->getPropertyValues().value("Domain ID");

            findEndpointDefinitions(design, instance, instanceComp, nd);
                        
            designNodes_.append(nd);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::canGenerateMCAPIComponent()
//-----------------------------------------------------------------------------
bool MCAPIParser::canGenerateMCAPIComponent(QSharedPointer<Component> component)
{
    // Will not generate a null component.
    if ( component == 0 )
    {
        return false;
    }

    // Errors found in interfaces.
    QStringList errorList;
    // Must have at least one MCAPI interface to generate.
    bool hasMcapi = false;

    // Go over each com interface and check for that each required property is set.
    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        // Check API support for generated interface
        if (comIf->getComType().getName().toLower() == "mcapi" )
        {
            hasMcapi = true;
        }

        QSharedPointer<Document> libCom = utility_->getLibraryInterface()->getModel(comIf->getComType());
        QSharedPointer<ComDefinition> comDef = libCom.dynamicCast<ComDefinition>();

        checkRequiredPropertiesSet(component->getVlnv().toString(), comDef, comIf, errorList);
    }

    // If errors exist, print about it and return false.
    if ( hasMcapi && !errorList.isEmpty())
    {
        foreach (QString const& msg, errorList)
        {
            utility_->printError(msg);
        }

        QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
            QObject::tr("The component contained %1 error(s). MCAPI code was not generated.").arg(errorList.size()),
            QMessageBox::Ok, utility_->getParentWidget());
        msgBox.setDetailedText(QObject::tr("The following error(s) were found: \n* ") + errorList.join("\n* "));

        msgBox.exec();
    }

    // If no errors, we can generate the component.
    return errorList.isEmpty() && hasMcapi;
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::checkRequiredPropertiesSet()
//-----------------------------------------------------------------------------
void MCAPIParser::checkRequiredPropertiesSet(QString componentVLNV, QSharedPointer<ComDefinition> comDef,
    QSharedPointer<ComInterface> comIf, QStringList &errorList)
{
    foreach ( QSharedPointer<ComProperty> property, *comDef->getProperties() )
    {
        if ( property->isRequired() && comIf->getPropertyValues().value(property->name()).isEmpty() )
        {
            errorList.append(QObject::tr("Property %1 of COM interface '%2' is not set in component '%3'").
                arg(property->name(),
                comIf->name(),componentVLNV));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::findEndpointDefinitions()
//-----------------------------------------------------------------------------
void MCAPIParser::findEndpointDefinitions(QSharedPointer<const Design> design, QSharedPointer<SWInstance> ourInstance, 
    QSharedPointer<Component> component, NodeData& nodeData)
{
    QPair<QSharedPointer<ComInterface>, PortReference> conPair(QSharedPointer<ComInterface>(), PortReference(""));
    foreach(conPair, findConnectedComInterfaces(design, ourInstance, component))
    {
        // Search for software instance corresponding theirs.
        QSharedPointer<SWInstance> targetInstance = searchInstance(design, conPair.second.getComponentRef());

        // Obtain the component object corresponding the software instance.
        VLNV instanceVLNV = *targetInstance->getComponentRef();
        QSharedPointer<Document> instanceLibComp = utility_->getLibraryInterface()->getModel(instanceVLNV);
        QSharedPointer<Component> instanceComp = instanceLibComp.dynamicCast<Component>();

        if (instanceComp != 0)
        {
            // Get the interfaces.
            QSharedPointer<ComInterface> targetInterface = instanceComp->getComInterface(conPair.second.getPortRef());
            QSharedPointer<ComInterface> ourInterface = conPair.first;

            // Must find it to generate.
            if (targetInterface != 0)
            {
                // Check for sanity.
                checkEndpointIdentifier(targetInterface, targetInstance);
                checkTransferType(ourInstance, ourInterface, targetInstance, targetInterface);
                checkScalarSize(ourInstance, ourInterface, targetInstance, targetInterface);

                // The data needed for "our" endpoint.
                EndPointData ourEnd = parseEndpoint(ourInterface);
                ourEnd.nodeID = ourInstance->getPropertyValues().value("Node ID");
                ourEnd.domainID = ourInstance->getPropertyValues().value("Domain ID");

                // The data needed for "their" endpoint.
                EndPointData theirEnd = parseEndpoint(ourInterface);
                theirEnd.nodeID = targetInstance->getPropertyValues().value("Node ID");
                theirEnd.domainID = targetInstance->getPropertyValues().value("Domain ID");

                // Create the pair endpoints and append to the list
                QPair<EndPointData, EndPointData> pair(ourEnd,theirEnd);
                nodeData.connections.append(pair);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::findConnectedComInterfaces()
//-----------------------------------------------------------------------------
QList<QPair<QSharedPointer<ComInterface>, PortReference> > MCAPIParser::findConnectedComInterfaces(
    QSharedPointer<const Design> design, QSharedPointer<SWInstance> ourInstance, QSharedPointer<Component> component )
{
    QList<QPair<QSharedPointer<ComInterface>, PortReference> > connectedInterfaces;

    // Go through the list of connections in the design to retrieve remote endpoint identifiers.
    foreach ( QSharedPointer<ComInterconnection> connection, design->getComConnections() )
    {
        // See which end on the connection is NOT ours.
        /*if ( ourInstance->getInstanceName() == connection->getStartInterface()->getComponentReference())
        {
            QSharedPointer<ComInterface> ourInterface = component->getComInterface(connection->getStartInterface()->getBusReference());

            // If our interface was not found we should not generate remote endpoint identifier for it.
            if (!ourInterface.isNull())
            {
                QSharedPointer<ActiveInterface> targetInterface = connection->getEndInterface();

                QPair<QSharedPointer<ComInterface>, PortReference> pair(ourInterface,
                    PortReference(targetInterface->getBusReference(), targetInterface->getComponentReference()));
                connectedInterfaces.append(pair);
            }
        }

        if (ourInstance->getInstanceName() == connection->getEndInterface()->getComponentReference())
        {
            QSharedPointer<ComInterface> ourInterface = component->
                getComInterface( connection->getEndInterface()->getBusReference());

            // If our interface was not found we should not generate remote endpoint identifier for it.
            if (!ourInterface.isNull())
            {
                QSharedPointer<ActiveInterface> targetInterface = connection->getStartInterface();

                QPair<QSharedPointer<ComInterface>, PortReference> pair( ourInterface,
                    PortReference(targetInterface->getBusReference(), targetInterface->getComponentReference()));
                connectedInterfaces.append(pair);
            }
        }*/
    }

    return connectedInterfaces;
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::parseEndpoint()
//-----------------------------------------------------------------------------
MCAPIParser::EndPointData MCAPIParser::parseEndpoint(QSharedPointer<ComInterface> comIf)
{
    EndPointData epd;
    epd.name = comIf->name();
    epd.remoteName = comIf->getPropertyValues().value("remote_endpoint_name");
    epd.portID = comIf->getPropertyValues().value("port_id");
    epd.handleName = comIf->getPropertyValues().value("handle_name");
    epd.scalarSize = comIf->getPropertyValues().value("scalar_size");
    epd.transferType = comIf->getTransferType();
    epd.direction = comIf->getDirection();
    return epd;
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::searchInstance()
//-----------------------------------------------------------------------------
QSharedPointer<SWInstance> MCAPIParser::searchInstance(QSharedPointer<const Design> design,
    QString const& instanceName)
{
    foreach (QSharedPointer<SWInstance> instance, design->getSWInstances())
    {
        // If the found instance name is same as target, will generate for it.
        if ( instance->getInstanceName() == instanceName )
        {
            return instance;
        }
    }

    return QSharedPointer<SWInstance>();
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::checkEndpointIdentifier()
//-----------------------------------------------------------------------------
void MCAPIParser::checkEndpointIdentifier(QSharedPointer<ComInterface> targetInterface,
    QSharedPointer<SWInstance> targetInstance)
{
    QString theirPortID = targetInterface->getPropertyValues().value("port_id").toUpper();
    QString theirDomainID = targetInstance->getPropertyValues().value("Domain ID");
    QString theirNodeID = targetInstance->getPropertyValues().value("Node ID");

    if ( theirPortID.isEmpty() || theirDomainID.isEmpty() || theirNodeID.isEmpty() )
    {
        utility_->printError("Could not find whole endpoint identifier for instance "
            + targetInstance->getInstanceName() + " interface " + targetInterface->name()
            + " Found domain: " + theirDomainID + " node: " + theirNodeID + " port: " + theirPortID);
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::checkTransferType()
//-----------------------------------------------------------------------------
void MCAPIParser::checkTransferType(QSharedPointer<SWInstance> ourInstance, 
    QSharedPointer<ComInterface> ourInterface, 
    QSharedPointer<SWInstance> targetInstance, QSharedPointer<ComInterface> targetInterface)
{
    if ( ourInterface->getTransferType() != targetInterface->getTransferType() )
    {
        utility_->printError("Transfer types of connected endpoints did not match! "
            "First instance: " + ourInstance->getInstanceName() + " first interface: "
            + ourInterface->name() + " Second instance: " + targetInstance->getInstanceName() + 
            " second interface " + targetInterface->name());
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::checkScalarSize()
//-----------------------------------------------------------------------------
void MCAPIParser::checkScalarSize(QSharedPointer<SWInstance> ourInstance,
    QSharedPointer<ComInterface> ourInterface, 
    QSharedPointer<SWInstance> targetInstance, QSharedPointer<ComInterface> targetInterface)
{
    QString ourScalarSize = ourInterface->getPropertyValues().value("scalar_size");
    QString theirScalarSize = targetInterface->getPropertyValues().value("scalar_size");

    if ( ourScalarSize != theirScalarSize )
    {
        utility_->printError("Scalar sizes of connected endpoints did not match! "
            "First instance: " + ourInstance->getInstanceName() + " first interface: "
            + ourInterface->name() + " Second instance: " + targetInstance->getInstanceName() + 
            " second interface " + targetInterface->name());
    }
}
