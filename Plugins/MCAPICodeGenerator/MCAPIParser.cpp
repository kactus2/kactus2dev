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

#include <QMessageBox>
#include <QCoreApplication>

#include <editors/CSourceEditor/CSourceTextEdit.h>

#include <IPXACTmodels/component.h>

#include <library/LibraryManager/libraryinterface.h>

#include "IPXACTmodels/SWView.h"
#include "common/dialogs/comboSelector/comboselector.h"
#include "IPXACTmodels/SystemView.h"

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
const QList<MCAPIParser::EndPointData>& MCAPIParser::getComponentEndpoints()
{
    return componentEndpoints_;
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::getDesignNodes()
//-----------------------------------------------------------------------------
const QList<MCAPIParser::NodeData>& MCAPIParser::getDesignNodes()
{
    return designNodes_;
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::getReplacedFiles()
//-----------------------------------------------------------------------------
const QStringList& MCAPIParser::getReplacedFiles()
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
        if (comIf->getComType().getName().toLower() != "mcapi" )
        {
            continue;
        }

        componentEndpoints_.append(EndPointData());
        EndPointData& epd = componentEndpoints_.last();

        parseEndpoint(epd, comIf);
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::parseTopLevel()
//-----------------------------------------------------------------------------
void MCAPIParser::parseTopLevel(QSharedPointer<Design> design, QSharedPointer<Component> topComponent,
    QSharedPointer<DesignConfiguration const> desgConf)
{
    QList<SWInstance>& instances = design->getSWInstances();

    for ( int i = 0; i < instances.size(); ++i )
    {
        SWInstance& instance = instances[i];
        VLNV instanceVLNV = instance.getComponentRef();
        VLNV* designVLNV = design->getVlnv();

        QSharedPointer<LibraryComponent> instanceLibComp = utility_->getLibraryInterface()->getModel(instanceVLNV);
        QSharedPointer<Component> instanceComp = instanceLibComp.dynamicCast<Component>();

        // Check if can generate the component, return if cannot
        if ( canGenerateMCAPIComponent(instanceComp) )
        {
            designNodes_.append(NodeData());
            NodeData& nd = designNodes_.last();
            nd.instance = instance;

            // Parse its directory as well.
            QString subDir = "/sw/" + instance.getInstanceName();
            QString dir = QFileInfo(utility_->getLibraryInterface()->getPath(*designVLNV)).absolutePath() + subDir;
            nd.directory = dir;

            QFile file(dir + "/instanceheader.h");

            // If does, put in the list.
            if ( file.exists() )
            {
                replacedFiles_.append(dir + "/instanceheader.h");
            }

            // Mark up the node identifier, as well as the associated domain identifier.
            nd.nodeID = instance.getPropertyValues().value("Node ID");
            nd.domainID = instance.getPropertyValues().value("Domain ID");

            findEndpointDefinitions(design, instance, instanceComp, nd);
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

        QSharedPointer<LibraryComponent> libCom = utility_->getLibraryInterface()->getModel(comIf->getComType());
        QSharedPointer<ComDefinition> comDef = libCom.dynamicCast<ComDefinition>();

        checkRequiredPropertiesSet(component->getVlnv()->toString(), comDef, comIf, errorList);
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
    foreach ( QSharedPointer<ComProperty> property, comDef->getProperties() )
    {
        if ( property->isRequired() && comIf->getPropertyValues().value(property->getName()).isEmpty() )
        {
            errorList.append(QObject::tr("Property %1 of COM interface '%2' is not set in component '%3'").
                arg(property->getName(),
                comIf->getName(),componentVLNV));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::findEndpointDefinitions()
//-----------------------------------------------------------------------------
void MCAPIParser::findEndpointDefinitions(QSharedPointer<const Design> design, SWInstance &ourInstance, 
    QSharedPointer<Component> component, NodeData& nodeData)
{
    QPair<QSharedPointer<ComInterface>, ComInterfaceRef> conPair;
    foreach( conPair, findConnectedComInterfaces(design, ourInstance, component) )
    {
        // Search for software instance corresponding theirs.
        SWInstance targetInstance = searchInstance(design, conPair.second.componentRef);

        // Obtain the component object corresponding the software instance.
        VLNV instanceVLNV = targetInstance.getComponentRef();
        QSharedPointer<LibraryComponent> instanceLibComp = utility_->getLibraryInterface()->getModel(instanceVLNV);
        QSharedPointer<Component> instanceComp = instanceLibComp.dynamicCast<Component>();

        if ( instanceComp == 0 )
        {
            continue;
        }

        // Get the interfaces.
        QSharedPointer<ComInterface> targetInterface = instanceComp->getComInterface(conPair.second.comRef);
        QSharedPointer<ComInterface> ourInterface = conPair.first;

        // Must find it to generate.
        if ( targetInterface != 0 )
        {
            // Check for sanity.
            checkEndpointIdentifier(targetInterface, targetInstance);
            checkTransferType(ourInterface, targetInterface, ourInstance, targetInstance);
            checkScalarSize(ourInterface, targetInterface, ourInstance, targetInstance);

            // The data needed for "our" endpoint.
            EndPointData ourEnd;
            parseEndpoint(ourEnd, ourInterface);
            ourEnd.nodeID = ourInstance.getPropertyValues().value("Node ID");
            ourEnd.domainID = ourInstance.getPropertyValues().value("Domain ID");

            // The data needed for "their" endpoint.
            EndPointData theirEnd;
            parseEndpoint(theirEnd, ourInterface);
            theirEnd.nodeID = targetInstance.getPropertyValues().value("Node ID");
            theirEnd.domainID = targetInstance.getPropertyValues().value("Domain ID");

            // Create the pair endpoints and append to the list
            QPair<EndPointData,EndPointData> pair(ourEnd,theirEnd);
            nodeData.connections.append(pair);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::findConnectedComInterfaces()
//-----------------------------------------------------------------------------
QList<QPair<QSharedPointer<ComInterface>, ComInterfaceRef> > MCAPIParser::findConnectedComInterfaces(
    QSharedPointer<const Design> design, SWInstance &ourInstance, QSharedPointer<Component> component )
{
    QList<QPair<QSharedPointer<ComInterface>, ComInterfaceRef> > connectedInterfaces;

    // Go through the list of connections in the design to retrieve remote endpoint identifiers.
    foreach ( ComConnection connection, design->getComConnections() )
    {
        // See which end on the connection is NOT ours.
        if ( ourInstance.getInstanceName() == connection.getInterface1().componentRef )
        {
            QSharedPointer<ComInterface> ourInterface = component->
                getComInterface( connection.getInterface1().comRef );

            // If our interface was not found we should not generate remote endpoint identifier for it.
            if ( !ourInterface.isNull() )
            {
                QPair<QSharedPointer<ComInterface>, ComInterfaceRef> pair( ourInterface,
                    connection.getInterface2() );
                connectedInterfaces.append(pair);
            }
        }

        if ( ourInstance.getInstanceName() == connection.getInterface2().componentRef )
        {
            QSharedPointer<ComInterface> ourInterface = component->
                getComInterface( connection.getInterface2().comRef );

            // If our interface was not found we should not generate remote endpoint identifier for it.
            if ( !ourInterface.isNull() )
            {
                QPair<QSharedPointer<ComInterface>, ComInterfaceRef> pair( ourInterface,
                    connection.getInterface1() );
                connectedInterfaces.append(pair);
            }
        }
    }

    return connectedInterfaces;
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::parseEndpoint()
//-----------------------------------------------------------------------------
void MCAPIParser::parseEndpoint(EndPointData &epd, QSharedPointer<ComInterface> comIf)
{
    epd.name = comIf->getName();
    epd.remoteName = comIf->getPropertyValues().value("remote_endpoint_name");
    epd.portID = comIf->getPropertyValues().value("port_id");
    epd.handleName = comIf->getPropertyValues().value("handle_name");
    epd.scalarSize = comIf->getPropertyValues().value("scalar_size");
    epd.transferType = comIf->getTransferType();
    epd.direction = comIf->getDirection();
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::searchInstance()
//-----------------------------------------------------------------------------
SWInstance MCAPIParser::searchInstance(QSharedPointer<const Design> design, QString instanceName)
{
    SWInstance targetInstance;

    foreach ( SWInstance instance, design->getSWInstances() )
    {
        // If the found instance name is same as target, will generate for it.
        if ( instance.getInstanceName() == instanceName )
        {
            return instance;
        }
    }

    return targetInstance;
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::checkEndpointIdentifier()
//-----------------------------------------------------------------------------
void MCAPIParser::checkEndpointIdentifier(QSharedPointer<ComInterface> targetInterface,
    SWInstance &targetInstance)
{
    QString theirPortID = targetInterface->getPropertyValues().value("port_id").toUpper();
    QString theirDomainID = targetInstance.getPropertyValues().value("Domain ID");
    QString theirNodeID = targetInstance.getPropertyValues().value("Node ID");

    if ( theirPortID.isEmpty() || theirDomainID.isEmpty() || theirNodeID.isEmpty() )
    {
        utility_->printError("Could not find whole endpoint identifier for instance "
            + targetInstance.getInstanceName() + " interface " + targetInterface->getName()
            + " Found domain: " + theirDomainID + " node: " + theirNodeID + " port: " + theirPortID);
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::checkTransferType()
//-----------------------------------------------------------------------------
void MCAPIParser::checkTransferType(QSharedPointer<ComInterface> ourInterface, QSharedPointer<ComInterface>
    targetInterface, SWInstance &ourInstance, SWInstance &targetInstance)
{
    if ( ourInterface->getTransferType() != targetInterface->getTransferType() )
    {
        utility_->printError("Transfer types of connected endpoints did not match! "
            "First instance: " + ourInstance.getInstanceName() + " first interface: "
            + ourInterface->getName() + " Second instance: " + targetInstance.getInstanceName() + 
            " second interface " + targetInterface->getName());
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPIParser::checkScalarSize()
//-----------------------------------------------------------------------------
void MCAPIParser::checkScalarSize(QSharedPointer<ComInterface> ourInterface, QSharedPointer<ComInterface>
    targetInterface, SWInstance &ourInstance, SWInstance &targetInstance)
{
    QString ourScalarSize = ourInterface->getPropertyValues().value("scalar_size");
    QString theirScalarSize = targetInterface->getPropertyValues().value("scalar_size");

    if ( ourScalarSize != theirScalarSize )
    {
        utility_->printError("Scalar sizes of connected endpoints did not match! "
            "First instance: " + ourInstance.getInstanceName() + " first interface: "
            + ourInterface->getName() + " Second instance: " + targetInstance.getInstanceName() + 
            " second interface " + targetInterface->getName());
    }
}
