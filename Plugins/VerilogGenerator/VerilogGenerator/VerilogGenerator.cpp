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

    QString fileName = outputPath.mid(outputPath.lastIndexOf("/") + 1);

    headerWriter_->write(outputStream, fileName, topComponent_->getDescription(), QDateTime::currentDateTime());
    topWriter_->write(outputStream);

    outputFile.close();
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::parse()
//-----------------------------------------------------------------------------
void VerilogGenerator::parse(QSharedPointer<Component> component, QSharedPointer<Design> design)
{
    topComponent_ = component;
    design_ = design;

    QSettings settings;

    QString currentUser = settings.value("General/Username").toString();
    QString componentXmlPath = library_->getPath(*topComponent_->getVlnv());

    headerWriter_ = QSharedPointer<VerilogHeaderWriter>(new VerilogHeaderWriter(*topComponent_->getVlnv(), 
        componentXmlPath, currentUser));

    topWriter_ = QSharedPointer<ComponentVerilogWriter>(new ComponentVerilogWriter(topComponent_, sorter_));

    instanceWriters_.clear();

    wireWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    if (design_)
    {
        parseComponentInstances();

        parseAdHocConnections();

        parseHierarchicalConnections();
        parseInterconnections();        
    }

    addWritersToTopInDesiredOrder();
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::nothingToWrite()
//-----------------------------------------------------------------------------
bool VerilogGenerator::nothingToWrite() const
{
    return topWriter_ == 0;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::parseInterconnections()
//-----------------------------------------------------------------------------
void VerilogGenerator::parseInterconnections()
{
    QStringList wireNames;
    foreach(Interface startInterface, getStartInterfacesForInterconnections())
    {
        QSharedPointer<BusInterface> startBusInterface = getBusinterfaceForInterface(startInterface);
        QStringList logicalPorts = startBusInterface->getLogicalPortNames();
        logicalPorts.removeDuplicates();

        foreach(QString logical, logicalPorts)
        {
            QString wireName = generateWireName(startInterface, logical);
            int wireSize = findWireSize(startInterface, logical); 

            mapPortsInInterface(startInterface, logical, wireName, wireSize);

            mapConnectedInstances(startInterface, logical, wireName, wireSize);
             
            addWireWriter(wireName, wireSize);
            wireNames.append(wireName);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::getStartInterfacesForInterconnections()
//-----------------------------------------------------------------------------
QList<Interface> VerilogGenerator::getStartInterfacesForInterconnections()
{
    QList<Interface> connectedStartInterfaces;

    foreach(Interconnection connection, design_->getInterconnections())
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
// Function: VerilogGenerator::generateWireName()
//-----------------------------------------------------------------------------
QString VerilogGenerator::generateWireName(Interface const& startInterface, QString const& logicalPort)
{
    QString wireName;
    if (design_->getConnectedInterfaces(startInterface).size() == 1)
    {
        wireName = findConnectionNameForInterface(startInterface) + "_" + logicalPort;
    }	
    else
    {
        wireName = startInterface.getComponentRef() + "_" + startInterface.getBusRef() + "_" + logicalPort;
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
        if (connection.getInterfaces().first == interface ||
            connection.getInterfaces().second == interface)
        {
            return connection.name();
        }
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::findWireSize()
//-----------------------------------------------------------------------------
int VerilogGenerator::findWireSize(Interface const& startInterface, QString const& logicalName)
{
    int higherBound =  0;

    foreach(QSharedPointer<PortMap> startPortMap, findPortMaps(logicalName, startInterface))
    {
        General::PortBounds bounds = logicalBoundsInInstance(startInterface.getComponentRef(), startPortMap);
        higherBound = qMax(higherBound, qMax(bounds.left_, bounds.right_));
    }

    foreach(Interface endInterface, design_->getConnectedInterfaces(startInterface))
    {
        foreach(QSharedPointer<PortMap> endPortMap, findPortMaps(logicalName, endInterface))
        {
            General::PortBounds bounds = logicalBoundsInInstance(endInterface.getComponentRef(), endPortMap);
            higherBound = qMax(higherBound, qMax(bounds.left_, bounds.right_));
        }
    }

    return higherBound + 1;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::logicalBoundsInInstance()
//-----------------------------------------------------------------------------
General::PortBounds VerilogGenerator::logicalBoundsInInstance(QString const& instanceName, 
    QSharedPointer<PortMap> portMap) const
{
    General::PortBounds bounds;
    
    QSharedPointer<Component> instanceComponent = getComponentForInstanceName(instanceName);
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
// Function: VerilogGenerator::getComponentForInstanceName()
//-----------------------------------------------------------------------------
QSharedPointer<Component> VerilogGenerator::getComponentForInstanceName(QString const& instanceName) const
{
    VLNV instanceVLNV = design_->getHWComponentVLNV(instanceName);
    return  library_->getModel(instanceVLNV).dynamicCast<Component>();
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::getPortMapsForLogicalSignalInBusInteface()
//-----------------------------------------------------------------------------
QList<QSharedPointer<PortMap> > VerilogGenerator::findPortMaps(QString const& logicalPort, 
    Interface const& interface) const
{
    QList<QSharedPointer<PortMap> > portMaps;

    QSharedPointer<BusInterface> slaveBusInterface = getBusinterfaceForInterface(interface);

    foreach(QSharedPointer<PortMap> slavePortMap, slaveBusInterface->getPortMaps())
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
void VerilogGenerator::mapConnectedInstances(Interface const& startInterface, QString const& logicalPort, 
    QString const& wireName, int const& wireSize)
{
    foreach(Interface endInterface, design_->getConnectedInterfaces(startInterface))
    {
        mapPortsInInterface(endInterface, logicalPort, wireName, wireSize);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::mapPortsInInterface()
//-----------------------------------------------------------------------------
void VerilogGenerator::mapPortsInInterface(Interface const& interface, QString const& logicalPort, 
    QString const& wireName, int const& wireSize)
{
    QString instanceName = interface.getComponentRef();
    foreach(QSharedPointer<PortMap> endPortMap, findPortMaps(logicalPort, interface))
    {
        mapInstancePortToWire(instanceName, endPortMap, wireName, wireSize);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::mapInstancePortToWire()
//-----------------------------------------------------------------------------
void VerilogGenerator::mapInstancePortToWire(QString const& instanceName, 
    QSharedPointer<PortMap> portMap, QString const& wireName, int const& wireSize)
{
    if (instanceWriters_.contains(instanceName))
    {
        QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter = instanceWriters_.value(instanceName);
        QSharedPointer<Component> instanceComponent = getComponentForInstanceName(instanceName);

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
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::addWireWriter()
//-----------------------------------------------------------------------------
void VerilogGenerator::addWireWriter(QString wireName, int wireSize)
{
    wireWriters_->add(QSharedPointer<VerilogWireWriter>(new VerilogWireWriter(wireName, wireSize)));
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::getInterfaceModeForInterface()
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
    QSharedPointer<Component> component = getComponentForInstanceName(interface.getComponentRef());

    if (component)
    {
        busInterface = component->getBusInterface(interface.getBusRef());
    }

    return busInterface;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::parseComponentInstances()
//-----------------------------------------------------------------------------
void VerilogGenerator::parseComponentInstances()
{
    foreach(ComponentInstance instance, design_->getComponentInstances())
    {
        QSharedPointer<Component> component = getComponentForInstanceName(instance.getInstanceName());
        if (component)
        {
            QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter = 
                QSharedPointer<ComponentInstanceVerilogWriter>(new ComponentInstanceVerilogWriter(instance, 
                component, sorter_));
            instanceWriters_.insert(instance.getInstanceName(), instanceWriter);            
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::parseHierarchicalConnections()
//-----------------------------------------------------------------------------
void VerilogGenerator::parseHierarchicalConnections()
{
    foreach(HierConnection connection, design_->getHierarchicalConnections())
    {
        QSharedPointer<BusInterface> topIf = topComponent_->getBusInterface(connection.getInterfaceRef());

        QString instanceName = connection.getInterface().getComponentRef();           
        QSharedPointer<Component> component = getComponentForInstanceName(instanceName);
        if (topIf && component)
        {
            mapTopBusInterfaceToInterfaceInInstance(topIf, connection.getInterface(), component);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::mapTopBusInterfaceToInterfaceInInstance()
//-----------------------------------------------------------------------------
void VerilogGenerator::mapTopBusInterfaceToInterfaceInInstance(QSharedPointer<BusInterface> topIf, 
    Interface const& instanceInterface, QSharedPointer<Component> instanceComponent)
{
    QString instanceName = instanceInterface.getComponentRef();    
    if (instanceWriters_.contains(instanceName))
    {
        QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter = instanceWriters_.value(instanceName);

        foreach(QSharedPointer<PortMap> topMap, topIf->getPortMaps())
        {
            foreach(QSharedPointer<PortMap> instanceMap, 
                findPortMaps(topMap->logicalPort(), instanceInterface))
            {
                QString port = instanceMap->physicalPort();
                QString signal = topMap->physicalPort();

                General::PortAlignment alignment = General::calculatePortAlignment(instanceMap.data(), 
                    instanceComponent->getPortLeftBound(port),instanceComponent->getPortRightBound(port),
                    topMap.data(), topComponent_->getPortLeftBound(signal), topComponent_->getPortRightBound(port));                

                if (instanceComponent->getPortWidth(port) == topComponent_->getPortWidth(signal) &&
                    alignment.port1Left_ == alignment.port2Left_ && 
                    alignment.port1Right_ == alignment.port2Right_)
                {
                    instanceWriter->assignPortForFullWidth(port, signal);
                }
                else
                {
                    instanceWriter->assignPortForRange(port, signal, alignment.port2Left_, alignment.port2Right_);    
                }  
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::parseAdHocConnections()
//-----------------------------------------------------------------------------
void VerilogGenerator::parseAdHocConnections()
{
    foreach(AdHocConnection const& adHocConnection, design_->getAdHocConnections())
    {
        if (isHierarchicalAdHocConnection(adHocConnection))
        {
            createPortMapsForHierarchicalAdHocConnection(adHocConnection);            
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
// Function: VerilogGenerator::createPortMapsForHierarchicalAdHocConnection()
//-----------------------------------------------------------------------------
void VerilogGenerator::createPortMapsForHierarchicalAdHocConnection(AdHocConnection const& adHocConnection)
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
        QSharedPointer<Component> component = getComponentForInstanceName(instanceReference.getComponentRef());
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
    int wireSize = wireSizeForAdHocConnection(adHocConnection);

    mapPortsInAdHocConnectionToWire(adHocConnection, wireName);

    addWireWriter(wireName, wireSize);
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::wireSizeForAdHocConnection()
//-----------------------------------------------------------------------------
int VerilogGenerator::wireSizeForAdHocConnection(AdHocConnection const &adHocConnection) const
{
    int wireSize = 1;
    foreach(PortRef internalRef, adHocConnection.internalPortReferences())
    {
        int portSize = 1;
        if (internalRef.getLeft() == -1 || internalRef.getRight() == -1)
        {
            QSharedPointer<Component> component = getComponentForInstanceName(internalRef.getComponentRef());
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
// Function: VerilogGenerator::mapAdHocConnectionInstancePortsToWire()
//-----------------------------------------------------------------------------
void VerilogGenerator::mapPortsInAdHocConnectionToWire(AdHocConnection const &adHocConnection, QString wireName)
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
    if (wireWriters_->hasWriters())
    {
        topWriter_->add(wireWriters_);    
    }

    foreach(QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter, instanceWriters_)
    {
        QString instanceName = instanceWriters_.key(instanceWriter);

        QSharedPointer<WriterGroup> instanceGroup = QSharedPointer<WriterGroup>(new WriterGroup);
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

    QString vlnv = design_->getHWComponentVLNV(instanceName).toString();
    header.append("IP-XACT VLNV: " + vlnv);

    QSharedPointer<CommentWriter> headerWriter(new CommentWriter(header));
    headerWriter->setIndent(4);
    return headerWriter;
}
