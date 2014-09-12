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

#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/abstractiondefinition.h>
#include <IPXACTmodels/PortRef.h>
#include <IPXACTmodels/PortMap.h>

#include <Plugins/VerilogGenerator/common/WriterGroup.h>
#include <Plugins/VerilogGenerator/CommentWriter/CommentWriter.h>
#include <Plugins/VerilogGenerator/ComponentVerilogWriter/ComponentVerilogWriter.h>
#include <Plugins/VerilogGenerator/ComponentInstanceVerilogWriter/ComponentInstanceVerilogWriter.h>
#include <Plugins/VerilogGenerator/PortSorter/InterfaceDirectionNameSorter.h>
#include <Plugins/VerilogGenerator/VerilogHeaderWriter/VerilogHeaderWriter.h>
#include <Plugins/VerilogGenerator/VerilogWireWriter/VerilogWireWriter.h>

#include <QDateTime>

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
void VerilogGenerator::parse(QSharedPointer<Component> component, QSharedPointer<Design> design)
{
    topComponent_ = component;
    design_ = design;

    initializeWriters();

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
void VerilogGenerator::initializeWriters()
{
    QSettings settings;
    QString currentUser = settings.value("General/Username").toString();
    QString componentXmlPath = library_->getPath(*topComponent_->getVlnv());

    headerWriter_ = QSharedPointer<VerilogHeaderWriter>(new VerilogHeaderWriter(*topComponent_->getVlnv(), 
        componentXmlPath, currentUser));

    topWriter_ = QSharedPointer<ComponentVerilogWriter>(new ComponentVerilogWriter(topComponent_, sorter_));

    instanceWriters_.clear();

    wireWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());
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
    foreach(Interface const&  primaryInterface, getPrimaryInterfacesForInterconnections())
    {
        createWiresForInterface(primaryInterface);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::getPrimaryInterfacesForInterconnections()
//-----------------------------------------------------------------------------
QList<Interface> VerilogGenerator::getPrimaryInterfacesForInterconnections()
{
    QList<Interface> connectedStartInterfaces;

    foreach(Interconnection const& connection, design_->getInterconnections())
    {       
        Interface startInterface = connection.getInterfaces().first;

        if (interfaceModeForInterface(connection.getInterfaces().second) == General::MASTER ||
            interfaceModeForInterface(connection.getInterfaces().second) == General::MIRROREDSLAVE)
        {
            startInterface = connection.getInterfaces().second;
        }               

        if (!connectedStartInterfaces.contains(startInterface))
        {
            connectedStartInterfaces.append(startInterface);
        }
    }

    return connectedStartInterfaces;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::createWiresForLogicalSignalsInInterface()
//-----------------------------------------------------------------------------
void VerilogGenerator::createWiresForInterface(Interface const& interface)
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
    Interface const& interface)
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
QString VerilogGenerator::generateWireName(Interface const& primaryInterface, QString const& logicalPort)
{
    QString wireName = "";

    if (design_->getConnectedInterfaces(primaryInterface).count() == 1)
    {
        wireName = findConnectionNameForInterface(primaryInterface) + "_" + logicalPort;
    }	
    else
    {
        wireName = primaryInterface.getComponentRef() + "_" + primaryInterface.getBusRef() + "_" + logicalPort;
    }

    return wireName;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::findConnectionNameForInterface()
//-----------------------------------------------------------------------------
QString VerilogGenerator::findConnectionNameForInterface(Interface interface)
{    
    foreach(Interconnection connection, design_->getInterconnections())
    {
        if (connection.getInterfaces().first == interface || connection.getInterfaces().second == interface)
        {
            return connection.name();
        }
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::findWireSize()
//-----------------------------------------------------------------------------
int VerilogGenerator::findWireSize(Interface const& primaryInterface, QString const& logicalName)
{
    int highestBound =  findHighestMappedLogicalIndexInInterface(logicalName, primaryInterface);

    foreach(Interface connectedInterface, design_->getConnectedInterfaces(primaryInterface))
    {
        int highestBoundInInterface = findHighestMappedLogicalIndexInInterface(logicalName, connectedInterface);
        highestBound = qMax(highestBound, highestBoundInInterface);
    }

    return highestBound + 1;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::findHighestMappedLogicalIndexInInterface()
//-----------------------------------------------------------------------------
int VerilogGenerator::findHighestMappedLogicalIndexInInterface(QString const& logicalName, 
    Interface const& interface)
{
    int highestBound = 0;
    foreach(QSharedPointer<PortMap> startPortMap, findPortMapsForLogicalPortInInterface(logicalName, interface))
    {
        General::PortBounds logicalBounds = logicalBoundsInInstance(interface.getComponentRef(), startPortMap);
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
    {
        QSharedPointer<Port> port = instanceComponent->getPort(portMap->physicalPort());   
        if (port)
        {
            bounds = portMap->getLogicalRange(port);
        }
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
    Interface const& interface) const
{
    QList<QSharedPointer<PortMap> > portMaps;
    foreach(QSharedPointer<PortMap> slavePortMap, getBusinterfaceForInterface(interface)->getPortMaps())
    {
        if (slavePortMap->logicalPort() == logicalPort)
        {
            portMaps.append(slavePortMap);
        }
    }

    return portMaps;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::mapConnectedInstances()
//-----------------------------------------------------------------------------
void VerilogGenerator::connectPortsInConnectedInstancesToWire(Interface const& startInterface, 
    QString const& logicalPort, QString const& wireName, int const& wireSize)
{
    foreach(Interface endInterface, design_->getConnectedInterfaces(startInterface))
    {
        connectPortsInInterfaceToWire(endInterface, logicalPort, wireName, wireSize);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::connectPortsInInterfaceToWire()
//-----------------------------------------------------------------------------
void VerilogGenerator::connectPortsInInterfaceToWire(Interface const& interface, QString const& logicalPort, 
    QString const& wireName, int const& wireSize)
{
    QString instanceName = interface.getComponentRef();
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
    if (wireSize == instanceComponent->getPortWidth(portMap->physicalPort()))
    {
        instanceWriter->assignPortForFullWidth(portMap->physicalPort(), wireName);
    }
    else
    {
        General::PortBounds bounds = portMap->getLogicalRange(instanceComponent->getPort(portMap->physicalPort()));
        instanceWriter->assignPortForRange(portMap->physicalPort(), wireName, bounds.left_, bounds.right_);
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
General::InterfaceMode VerilogGenerator::interfaceModeForInterface(Interface const& interface)
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
QSharedPointer<BusInterface> VerilogGenerator::getBusinterfaceForInterface(Interface const& interface) const
{
    QSharedPointer<BusInterface> busInterface;    
    QSharedPointer<Component> component = getComponentForInstance(interface.getComponentRef());

    if (component)
    {
        busInterface = component->getBusInterface(interface.getBusRef());
    }

    return busInterface;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::createWritersForComponentInstances()
//-----------------------------------------------------------------------------
void VerilogGenerator::createWritersForComponentInstances()
{
    foreach(ComponentInstance instance, design_->getComponentInstances())
    {
        QSharedPointer<Component> component = getComponentForInstance(instance.getInstanceName());
        if (component)
        {
            QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter(
                new ComponentInstanceVerilogWriter(instance, component, sorter_));
            instanceWriters_.insert(instance.getInstanceName(), instanceWriter);            
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::connectHierarchicalConnectionsToInstances()
//-----------------------------------------------------------------------------
void VerilogGenerator::connectHierarchicalConnectionsToInstances()
{
    foreach(HierConnection connection, design_->getHierarchicalConnections())
    {
        QSharedPointer<BusInterface> topBusInterface = topComponent_->getBusInterface(connection.getInterfaceRef());

        QString connectedInstance = connection.getInterface().getComponentRef();           
        QSharedPointer<Component> instanceComponent = getComponentForInstance(connectedInstance);

        if (topBusInterface && instanceComponent)
        {            
            connectTopBusInterfaceToInterfaceInInstance(topBusInterface, connection.getInterface());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::connectTopBusInterfaceToInterfaceInInstance()
//-----------------------------------------------------------------------------
void VerilogGenerator::connectTopBusInterfaceToInterfaceInInstance(QSharedPointer<BusInterface> topBusInterface, 
    Interface const& instanceInterface)
{
    QString instanceName = instanceInterface.getComponentRef();    
    if (!instanceWriters_.contains(instanceName))
    {
        return;
    }

    QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter = instanceWriters_.value(instanceName);

    QSharedPointer<Component> instanceComponent = getComponentForInstance(instanceInterface.getComponentRef());

    foreach(QSharedPointer<PortMap> topMap, topBusInterface->getPortMaps())
    {
        foreach(QSharedPointer<PortMap> instancePortMap, 
            findPortMapsForLogicalPortInInterface(topMap->logicalPort(), instanceInterface))
        {
            QString instancePort = instancePortMap->physicalPort();
            QString topPort = topMap->physicalPort();

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
            }  
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
    foreach(AdHocConnection const& adHocConnection, design_->getAdHocConnections())
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
bool VerilogGenerator::isHierarchicalAdHocConnection(AdHocConnection const& adHocConnection)
{
    return !adHocConnection.externalPortReferences().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::connectInstancePortsTopPorts()
//-----------------------------------------------------------------------------
void VerilogGenerator::connectInstancePortsTopPort(AdHocConnection const& adHocConnection)
{
    PortRef topReference = adHocConnection.externalPortReferences().first();

    foreach(PortRef instanceReference, adHocConnection.internalPortReferences())
    {
        if (instanceWriters_.contains(instanceReference.getComponentRef()))
        {
            QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter = instanceWriters_.value(instanceReference.getComponentRef());
            instanceWriter->assignPortForFullWidth(instanceReference.getPortRef(), topReference.getPortRef());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::shouldCreateWireForAdHocConnection()
//-----------------------------------------------------------------------------
bool VerilogGenerator::shouldCreateWireForAdHocConnection(AdHocConnection const& adHocConnection)
{
    foreach(PortRef instanceReference, adHocConnection.internalPortReferences())
    {
        QSharedPointer<Component> component = getComponentForInstance(instanceReference.getComponentRef());
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
void VerilogGenerator::createWireForAdHocConnection(AdHocConnection const& adHocConnection)
{
    QString wireName = adHocConnection.name();
    int wireSize = findWireSizeForAdHocConnection(adHocConnection);
    
    addWriterForWire(wireName, wireSize);

    connectPortsInAdHocConnectionToWire(adHocConnection, wireName);    
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::findWireSizeForAdHocConnection()
//-----------------------------------------------------------------------------
int VerilogGenerator::findWireSizeForAdHocConnection(AdHocConnection const& adHocConnection) const
{
    int wireSize = 1;
    foreach(PortRef internalRef, adHocConnection.internalPortReferences())
    {
        int portSize = 1;
        if (internalRef.getLeft() == -1 || internalRef.getRight() == -1)
        {
            QSharedPointer<Component> component = getComponentForInstance(internalRef.getComponentRef());
            if (component)
            {
                portSize = component->getPortWidth(internalRef.getPortRef());
            }
        }
        else
        {
            portSize = abs(internalRef.getLeft() - internalRef.getRight()) + 1;
        }
        wireSize = qMax(wireSize, portSize);
    }	
    return wireSize;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::connectPortsInAdHocConnectionToWire()
//-----------------------------------------------------------------------------
void VerilogGenerator::connectPortsInAdHocConnectionToWire(AdHocConnection const& adHocConnection, 
    QString const& wireName)
{
    foreach(PortRef internalRef, adHocConnection.internalPortReferences())
    {
        if (instanceWriters_.contains(internalRef.getComponentRef()))
        {
            QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter = instanceWriters_.value(internalRef.getComponentRef());
            instanceWriter->assignPortForFullWidth(internalRef.getPortRef(), wireName);
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
