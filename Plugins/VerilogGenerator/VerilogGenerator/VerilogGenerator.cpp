//-----------------------------------------------------------------------------
// File: VerilogGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// Verilog file generator.
//-----------------------------------------------------------------------------

#include "VerilogGenerator.h"

#include <library/LibraryManager/libraryinterface.h>

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/Model.h>

#include <Plugins/VerilogGenerator/common/WriterGroup.h>
#include <Plugins/VerilogGenerator/CommentWriter/CommentWriter.h>
#include <Plugins/VerilogGenerator/ComponentVerilogWriter/ComponentVerilogWriter.h>
#include <Plugins/VerilogGenerator/ComponentInstanceVerilogWriter/ComponentInstanceVerilogWriter.h>
#include <Plugins/VerilogGenerator/PortSorter/InterfaceDirectionNameSorter.h>
#include <Plugins/VerilogGenerator/VerilogHeaderWriter/VerilogHeaderWriter.h>
#include <Plugins/VerilogGenerator/VerilogWireWriter/VerilogWireWriter.h>

#include <QDateTime>
#include "QFileInfo"

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::VerilogGenerator()
//-----------------------------------------------------------------------------
VerilogGenerator::VerilogGenerator(LibraryInterface* library): library_(library), headerWriter_(0), topWriter_(0), 
topComponent_(), design_(), wireWriters_(), instanceWriters_(), sorter_(new InterfaceDirectionNameSorter())
{

}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::~VerilogGenerator()
//-----------------------------------------------------------------------------
VerilogGenerator::~VerilogGenerator()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::parse()
//-----------------------------------------------------------------------------
void VerilogGenerator::parse(QSharedPointer<Component> component, QString const& topComponentView, 
    QSharedPointer<Design> design /*= QSharedPointer<Design>()*/)
{
    topComponent_ = component;
    design_ = design;

    initializeWriters(topComponentView);

    if (design_)
    {
        createWritersForComponentInstances();

        connectAndWireAdHocConnections();

        connectHierarchicalConnectionsToInstances();
        createWiresForInterconnections();        
    }

    addWritersToTopInDesiredOrder();
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::generate()
//-----------------------------------------------------------------------------
void VerilogGenerator::generate(QString const& outputPath) const
{
    if (nothingToWrite())
    {
        return;
    }

    QFile outputFile(outputPath); 
    if (!outputFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QTextStream outputStream(&outputFile);

    QString fileName = QFileInfo(outputPath).fileName();

    headerWriter_->write(outputStream, fileName, topComponent_->getDescription(), QDateTime::currentDateTime());
    topWriter_->write(outputStream);

    outputFile.close();
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::initializeWriters()
//-----------------------------------------------------------------------------
void VerilogGenerator::initializeWriters(QString const& topComponentView)
{
    QSettings settings;
    QString currentUser = settings.value("General/Username").toString();
    QString componentXmlPath = library_->getPath(topComponent_->getVlnv());

    headerWriter_ = QSharedPointer<VerilogHeaderWriter>(new VerilogHeaderWriter(topComponent_->getVlnv(), 
        componentXmlPath, currentUser));

    topWriter_ = QSharedPointer<ComponentVerilogWriter>(new ComponentVerilogWriter(topComponent_, topComponentView,
        sorter_, createFormatterForComponent(topComponent_)));

    instanceWriters_.clear();

    wireWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::createFormatterForComponent()
//-----------------------------------------------------------------------------
QSharedPointer<ExpressionFormatter> VerilogGenerator::createFormatterForComponent(QSharedPointer<Component> targetComponent)
{
    QSharedPointer<ParameterFinder> parameterFinder(new ComponentParameterFinder(targetComponent));
    return QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(parameterFinder));
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::nothingToWrite()
//-----------------------------------------------------------------------------
bool VerilogGenerator::nothingToWrite() const
{
    return topWriter_ == 0;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::createWiresForInterconnections()
//-----------------------------------------------------------------------------
void VerilogGenerator::createWiresForInterconnections()
{
    foreach(QSharedPointer<ActiveInterface> primaryInterface, getPrimaryInterfacesForInterconnections())
    {
        createWiresForInterface(primaryInterface);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::getPrimaryInterfacesForInterconnections()
//-----------------------------------------------------------------------------
QList<QSharedPointer<ActiveInterface> > VerilogGenerator::getPrimaryInterfacesForInterconnections()
{
    QList<QSharedPointer<ActiveInterface> > connectedStartInterfaces;

    foreach(QSharedPointer<Interconnection> connection, *design_->getInterconnections())
    {       
        if(!connection->getActiveInterfaces()->isEmpty())
        {
            QSharedPointer<ActiveInterface> startInterface = connection->getStartInterface();

            if (interfaceModeForInterface(connection->getActiveInterfaces()->first()) == General::MASTER ||
                interfaceModeForInterface(connection->getActiveInterfaces()->first()) == General::MIRROREDSLAVE)
            {
                startInterface = connection->getActiveInterfaces()->first();
            }               

            if (!connectedStartInterfaces.contains(startInterface))
            {
                connectedStartInterfaces.append(startInterface);
            }
        }

    }

    return connectedStartInterfaces;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::createWiresForLogicalSignalsInInterface()
//-----------------------------------------------------------------------------
void VerilogGenerator::createWiresForInterface(QSharedPointer<ActiveInterface> interface)
{
    QSharedPointer<BusInterface> busInterface = getBusinterfaceForInterface(interface);
    QStringList logicalSignalsInInterface = busInterface->getLogicalPortNames();
    logicalSignalsInInterface.removeDuplicates();

    foreach(QString const& logicalSignal, logicalSignalsInInterface)
    {
        createWireForLogicalSignalInInterface(logicalSignal, interface);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::createWireForLogicalSignalInInterface()
//-----------------------------------------------------------------------------
void VerilogGenerator::createWireForLogicalSignalInInterface(QString const& logicalSignal, 
    QSharedPointer<ActiveInterface> interface)
{
    QString wireName = generateWireName(interface, logicalSignal);
    int wireSize = findWireSize(interface, logicalSignal); 

    addWriterForWire(wireName, wireSize);

    connectPortsInInterfaceToWire(interface, logicalSignal, wireName, wireSize);
    connectPortsInConnectedInstancesToWire(interface, logicalSignal, wireName, wireSize);
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::generateWireName()
//-----------------------------------------------------------------------------
QString VerilogGenerator::generateWireName(QSharedPointer<ActiveInterface> primaryInterface, QString const& logicalPort)
{
    QString wireName = "";

   /* if (design_->getConnectedInterfaces(primaryInterface).count() == 1)
    {
        wireName = findConnectionNameForInterface(primaryInterface) + "_" + logicalPort;
    }	
    else
    {*/
        wireName = primaryInterface->getComponentReference() + "_" + 
            primaryInterface->getBusReference() + "_" + logicalPort;
   // }

    return wireName;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::findConnectionNameForInterface()
//-----------------------------------------------------------------------------
QString VerilogGenerator::findConnectionNameForInterface(QSharedPointer<ActiveInterface> interface)
{    
    foreach(QSharedPointer<Interconnection> connection, *design_->getInterconnections())
    {
        if (connection->getStartInterface() == interface || connection->getActiveInterfaces()->first() == interface)
        {
            return connection->name();
        }
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::findWireSize()
//-----------------------------------------------------------------------------
int VerilogGenerator::findWireSize(QSharedPointer<ActiveInterface> primaryInterface, QString const& logicalName)
{
    int highestBound =  findHighestMappedLogicalIndexInInterface(logicalName, primaryInterface);

   /* foreach(QSharedPointer<ActiveInterface> connectedInterface, design_->getConnectedInterfaces(primaryInterface))
    {
        int highestBoundInInterface = findHighestMappedLogicalIndexInInterface(logicalName, connectedInterface);
        highestBound = qMax(highestBound, highestBoundInInterface);
    }*/

    return highestBound + 1;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::findHighestMappedLogicalIndexInInterface()
//-----------------------------------------------------------------------------
int VerilogGenerator::findHighestMappedLogicalIndexInInterface(QString const& logicalName, 
    QSharedPointer<ActiveInterface> interface)
{
    int highestBound = 0;
    foreach(QSharedPointer<PortMap> startPortMap, findPortMapsForLogicalPortInInterface(logicalName, interface))
    {
        General::PortBounds logicalBounds = logicalBoundsInInstance(interface->getComponentReference(), startPortMap);
        highestBound = qMax(highestBound, qMax(logicalBounds.left_, logicalBounds.right_));
    }	
    
    return highestBound;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::logicalBoundsInInstance()
//-----------------------------------------------------------------------------
General::PortBounds VerilogGenerator::logicalBoundsInInstance(QString const& instanceName, 
    QSharedPointer<PortMap> portMap) const
{
    General::PortBounds bounds;
    
    QSharedPointer<Component> instanceComponent = getComponentForInstance(instanceName);
    if (instanceComponent)
    {// TODO: form bounds from expressions
        /*QSharedPointer<Port> port = instanceComponent->getPort(portMap->physicalPort());   
        if (port)
        {
            bounds = portMap->getLogicalRange(port);
        }*/
    }

    return bounds;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::getComponentForInstance()
//-----------------------------------------------------------------------------
QSharedPointer<Component> VerilogGenerator::getComponentForInstance(QString const& instanceName) const
{
    VLNV instanceVLNV = design_->getHWComponentVLNV(instanceName);
    return library_->getModel(instanceVLNV).dynamicCast<Component>();
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::findPortMapsForLogicalPortInInterface()
//-----------------------------------------------------------------------------
QList<QSharedPointer<PortMap> > VerilogGenerator::findPortMapsForLogicalPortInInterface(QString const& logicalPort, 
    QSharedPointer<ActiveInterface> interface) const
{
    QList<QSharedPointer<PortMap> > portMaps;
    foreach(QSharedPointer<PortMap> slavePortMap, *getBusinterfaceForInterface(interface)->getPortMaps())
    {
        if (slavePortMap->getLogicalPort()->name_ == logicalPort)
        {
            portMaps.append(slavePortMap);
        }
    }

    return portMaps;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::mapConnectedInstances()
//-----------------------------------------------------------------------------
void VerilogGenerator::connectPortsInConnectedInstancesToWire(QSharedPointer<ActiveInterface> startInterface, 
    QString const& logicalPort, QString const& wireName, int const& wireSize)
{
    /*foreach(QSharedPointer<ActiveInterface> endInterface, design_->getConnectedInterfaces(startInterface))
    {
        connectPortsInInterfaceToWire(endInterface, logicalPort, wireName, wireSize);
    }*/
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::connectPortsInInterfaceToWire()
//-----------------------------------------------------------------------------
void VerilogGenerator::connectPortsInInterfaceToWire(QSharedPointer<ActiveInterface> interface, QString const& logicalPort, 
    QString const& wireName, int const& wireSize)
{
    QString instanceName = interface->getComponentReference();
    foreach(QSharedPointer<PortMap> endPortMap, findPortMapsForLogicalPortInInterface(logicalPort, interface))
    {
        connectInstancePortToWire(instanceName, endPortMap, wireName, wireSize);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::connectInstancePortToWire()
//-----------------------------------------------------------------------------
void VerilogGenerator::connectInstancePortToWire(QString const& instanceName, 
    QSharedPointer<PortMap> portMap, QString const& wireName, int const& wireSize)
{
    if (!instanceWriters_.contains(instanceName))
    {
        return;
    }

    QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter = instanceWriters_.value(instanceName);

    QSharedPointer<Component> instanceComponent = getComponentForInstance(instanceName);
	// TODO: get port width
    /*if (wireSize == instanceComponent->getModel()->getPort(portMap->getPhysicalPort()->name_)->)
    {
        instanceWriter->assignPortForFullWidth(portMap->getPhysicalPort()->name_, wireName);
    }
    else*/
    {
		// TODO: fix after expressions available
        //General::PortBounds bounds = portMap->getLogicalRange(instanceComponent->getPort(portMap->physicalPort()));
        //instanceWriter->assignPortForRange(portMap->physicalPort(), wireName, bounds.left_, bounds.right_);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::addWriterForWire()
//-----------------------------------------------------------------------------
void VerilogGenerator::addWriterForWire(QString wireName, int wireSize)
{
    wireWriters_->add(QSharedPointer<VerilogWireWriter>(new VerilogWireWriter(wireName, wireSize)));
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::interfaceModeForInterface()
//-----------------------------------------------------------------------------
General::InterfaceMode VerilogGenerator::interfaceModeForInterface(QSharedPointer<ActiveInterface> interface)
{
    General::InterfaceMode mode = General::INTERFACE_MODE_COUNT;

    QSharedPointer<BusInterface> busInterface = getBusinterfaceForInterface(interface);
    if (busInterface)
    {
        mode = busInterface->getInterfaceMode();
    }

    return mode;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::getBusinterfaceForInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> VerilogGenerator::getBusinterfaceForInterface(QSharedPointer<ActiveInterface> interface) const
{
    QSharedPointer<BusInterface> busInterface;    
    QSharedPointer<Component> component = getComponentForInstance(interface->getComponentReference());

    if (component)
    {
        busInterface = component->getBusInterface(interface->getBusReference());
    }

    return busInterface;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::createWritersForComponentInstances()
//-----------------------------------------------------------------------------
void VerilogGenerator::createWritersForComponentInstances()
{
    foreach(QSharedPointer<ComponentInstance> instance, *design_->getComponentInstances())
    {
        QSharedPointer<Component> component = getComponentForInstance(instance->getInstanceName());

        if (component)
        {
            QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter(
                new ComponentInstanceVerilogWriter(instance, component, sorter_, 
                createFormatterForComponent(component)));
            instanceWriters_.insert(instance->getInstanceName(), instanceWriter);            
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::connectHierarchicalConnectionsToInstances()
//-----------------------------------------------------------------------------
void VerilogGenerator::connectHierarchicalConnectionsToInstances()
{
    foreach(QSharedPointer<Interconnection> connection, *design_->getInterconnections())
    {
        if (!connection->getHierInterfaces()->isEmpty())
        {
            QSharedPointer<HierInterface> topInterface = connection->getHierInterfaces()->first();

            QSharedPointer<BusInterface> topBusInterface = topComponent_->getBusInterface(topInterface->getBusReference());

            QString connectedInstance = connection->getStartInterface()->getComponentReference();
            QSharedPointer<Component> instanceComponent = getComponentForInstance(connectedInstance);

            if (topBusInterface && instanceComponent)
            {            
                connectTopBusInterfaceToInterfaceInInstance(topBusInterface, connection->getStartInterface());
            }
        }
    }

    /*foreach(HierConnection connection, design_->getHierarchicalConnections())
    {
        QSharedPointer<BusInterface> topBusInterface = topComponent_->getBusInterface(connection.getInterfaceRef());

        QString connectedInstance = connection.getInterface().getComponentRef();           
        QSharedPointer<Component> instanceComponent = getComponentForInstance(connectedInstance);

        if (topBusInterface && instanceComponent)
        {            
            connectTopBusInterfaceToInterfaceInInstance(topBusInterface, connection.getInterface());
        }
    }*/
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::connectTopBusInterfaceToInterfaceInInstance()
//-----------------------------------------------------------------------------
void VerilogGenerator::connectTopBusInterfaceToInterfaceInInstance(QSharedPointer<BusInterface> topIf, 
    QSharedPointer<ActiveInterface> instanceInterface)
{
    QString instanceName = instanceInterface->getComponentReference();    
    if (!instanceWriters_.contains(instanceName))
    {
        return;
    }

    QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter = instanceWriters_.value(instanceName);

    QSharedPointer<Component> instanceComponent = getComponentForInstance(instanceInterface->getComponentReference());

    foreach(QSharedPointer<PortMap> topMap, *topIf->getPortMaps())
    {
        foreach(QSharedPointer<PortMap> instancePortMap, 
            findPortMapsForLogicalPortInInterface(topMap->getLogicalPort()->name_, instanceInterface))
        {
            QSharedPointer<PortMap::PhysicalPort> instancePort = instancePortMap->getPhysicalPort();
            QSharedPointer<PortMap::PhysicalPort> topPort = topMap->getPhysicalPort();

			/* TODO: Calculate alignment with expressions.
            int instancePortWidth = instanceComponent->getPortWidth(instancePort);
            int topPortWidth = topComponent_->getPortWidth(topPort);

            General::PortAlignment alignment = General::calculatePortAlignment(instancePortMap.data(), 
                instanceComponent->getPortLeftBound(instancePort), 
                instanceComponent->getPortRightBound(instancePort),
                topMap.data(), 
                topComponent_->getPortLeftBound(topPort),
                topComponent_->getPortRightBound(instancePort));

            if (canConnectForFullWidth(instancePortWidth, topPortWidth, alignment))
            {
                instanceWriter->assignPortForFullWidth(instancePort, topPort);
            }
            else
            {
                instanceWriter->assignPortForRange(instancePort, topPort, alignment.port2Left_, alignment.port2Right_);    
            } */ 
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::canConnectForFullWidth()
//-----------------------------------------------------------------------------
bool VerilogGenerator::canConnectForFullWidth(int instancePortWidth, int topPortWidth,
    General::PortAlignment const& alignment) const
{
    return instancePortWidth == topPortWidth &&
        alignment.port1Left_ == alignment.port2Left_ && 
        alignment.port1Right_ == alignment.port2Right_;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::connectAndWireAdHocConnections()
//-----------------------------------------------------------------------------
void VerilogGenerator::connectAndWireAdHocConnections()
{
    foreach(QSharedPointer<AdHocConnection> adHocConnection, *design_->getAdHocConnections())
    {
        if (isHierarchicalAdHocConnection(adHocConnection))
        {
            connectInstancePortsTopPort(adHocConnection);            
        }
        else if (shouldCreateWireForAdHocConnection(adHocConnection))
        {
            createWireForAdHocConnection(adHocConnection);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::isHierarchicalAdHocConnection()
//-----------------------------------------------------------------------------
bool VerilogGenerator::isHierarchicalAdHocConnection(QSharedPointer<AdHocConnection> adHocConnection)
{
    return !adHocConnection->getExternalPortReferences()->isEmpty();
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::connectInstancePortsTopPorts()
//-----------------------------------------------------------------------------
void VerilogGenerator::connectInstancePortsTopPort(QSharedPointer<AdHocConnection> adHocConnection)
{
    QSharedPointer<PortReference> topReference = adHocConnection->getExternalPortReferences()->first();

    foreach(QSharedPointer<PortReference> instanceReference, *adHocConnection->getInternalPortReferences())
    {
        if (instanceWriters_.contains(instanceReference->getComponentRef()))
        {
            QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter = 
                instanceWriters_.value(instanceReference->getComponentRef());
            instanceWriter->assignPortForFullWidth(instanceReference->getPortRef(), topReference->getPortRef());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::shouldCreateWireForAdHocConnection()
//-----------------------------------------------------------------------------
bool VerilogGenerator::shouldCreateWireForAdHocConnection(QSharedPointer<AdHocConnection> adHocConnection)
{
    foreach(QSharedPointer<PortReference> instanceReference, *adHocConnection->getInternalPortReferences())
    {
        QSharedPointer<Component> component = getComponentForInstance(instanceReference->getComponentRef());
        if (component)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::createWireForAdHocConnection()
//-----------------------------------------------------------------------------
void VerilogGenerator::createWireForAdHocConnection(QSharedPointer<AdHocConnection> adHocConnection)
{
    QString wireName = adHocConnection->name();
    int wireSize = findWireSizeForAdHocConnection(adHocConnection);
    
    addWriterForWire(wireName, wireSize);

    connectPortsInAdHocConnectionToWire(adHocConnection, wireName);    
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::findWireSizeForAdHocConnection()
//-----------------------------------------------------------------------------
int VerilogGenerator::findWireSizeForAdHocConnection(QSharedPointer<AdHocConnection> adHocConnection) const
{
    int wireSize = 1;
    foreach(QSharedPointer<PortReference> internalRef, *adHocConnection->getInternalPortReferences())
    {
        int portSize = 1;

        QSharedPointer<PartSelect> part = internalRef->getPartSelect();
        if (part.isNull())
        {
            QSharedPointer<Component> component = getComponentForInstance(internalRef->getComponentRef());
            if (component)
            {
				//TODO: fix after expressions exist
                //portSize = component->getPortWidth(internalRef->getPortRef());
            }
        }
        else
        {
            portSize = abs(part->getLeftRange().toInt() - part->getRightRange().toInt()) + 1;
        }
        wireSize = qMax(wireSize, portSize);
    }	
    return wireSize;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::connectPortsInAdHocConnectionToWire()
//-----------------------------------------------------------------------------
void VerilogGenerator::connectPortsInAdHocConnectionToWire(QSharedPointer<AdHocConnection> adHocConnection, 
    QString const& wireName)
{
    foreach(QSharedPointer<PortReference> internalRef, *adHocConnection->getInternalPortReferences())
    {
        if (instanceWriters_.contains(internalRef->getComponentRef()))
        {
            QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter = instanceWriters_.value(internalRef->getComponentRef());
            instanceWriter->assignPortForFullWidth(internalRef->getPortRef(), wireName);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::addWritersToTopInDesiredOrder()
//-----------------------------------------------------------------------------
void VerilogGenerator::addWritersToTopInDesiredOrder() const
{
    topWriter_->add(wireWriters_);    

    foreach(QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter, instanceWriters_)
    {
        QString instanceName = instanceWriters_.key(instanceWriter);

        QSharedPointer<WriterGroup> instanceGroup(new WriterGroup);
        instanceGroup->add(createHeaderWriterForInstance(instanceName));
        instanceGroup->add(instanceWriter);

        topWriter_->add(instanceGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::createHeaderWriterForInstance()
//-----------------------------------------------------------------------------
QSharedPointer<Writer> VerilogGenerator::createHeaderWriterForInstance(QString const& instanceName) const
{
    QString header = design_->getHWInstanceDescription(instanceName);
    if (!header.isEmpty())
    {
        header.append("\n");
    }

    QString instanceVLNV = design_->getHWComponentVLNV(instanceName).toString();
    header.append("IP-XACT VLNV: " + instanceVLNV);

    QSharedPointer<CommentWriter> headerWriter(new CommentWriter(header));
    headerWriter->setIndent(4);

    return headerWriter;
}
