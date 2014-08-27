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
    wireWriters_.clear();

    if (design_)
    {
        parseComponentInstances();

        parseHierarchicalConnections();
        parseInterconnections();

        parseAdHocConnections();
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
   foreach(Interface startInterface, getStartInterfacesForInterconnections())
   {
        QSharedPointer<BusInterface> startBusInterface = getBusinterfaceForInterface(startInterface);

        foreach(QSharedPointer<General::PortMap> startPortMap, startBusInterface->getPortMaps())
        { 
            QString wireName = generateWireName(startInterface, startPortMap->logicalPort_);
            int wireSize = findWireSize(startInterface, startPortMap); 

            mapInstancePortToWire(startInterface.getComponentRef(), startPortMap, wireName, wireSize);

            mapToConnectedInstances(startInterface, startPortMap->logicalPort_, wireName, wireSize);

            addWireWriter(wireName, wireSize);
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
int VerilogGenerator::findWireSize(Interface const& startInterface, QSharedPointer<General::PortMap> startPortMap)
{
    General::PortBounds masterBounds = logicalBoundsInInstance(startInterface.getComponentRef(), startPortMap);

    int wireSize =  abs(masterBounds.left_ - masterBounds.right_) + 1;

    foreach(Interface endInterface, design_->getConnectedInterfaces(startInterface))
    {
        foreach(QSharedPointer<General::PortMap> slavePortMap, 
            findPortMaps(startPortMap->logicalPort_, endInterface))
        {
            General::PortBounds bounds = logicalBoundsInInstance(endInterface.getComponentRef(), slavePortMap);
            int size = abs(bounds.left_ - bounds.right_) + 1;
            wireSize = qMax(wireSize, size);
        }
    }

    return wireSize;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::logicalBoundsInInstance()
//-----------------------------------------------------------------------------
General::PortBounds VerilogGenerator::logicalBoundsInInstance(QString const& instanceName, 
    QSharedPointer<General::PortMap> portMap) const
{
    General::PortBounds bounds;
    
    QSharedPointer<Component> instanceComponent = getComponentForInstanceName(instanceName);
    if (instanceComponent)
    {
        QSharedPointer<Port> port = instanceComponent->getPort(portMap->physicalPort_);   
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
QList<QSharedPointer<General::PortMap> > VerilogGenerator::findPortMaps(QString const& logicalPort, 
    Interface const& interface) const
{
    QList<QSharedPointer<General::PortMap> > portMaps;

    QSharedPointer<BusInterface> slaveBusInterface = getBusinterfaceForInterface(interface);

    foreach(QSharedPointer<General::PortMap> slavePortMap, slaveBusInterface->getPortMaps())
    {
        if (slavePortMap->logicalPort_ == logicalPort)
        {
            portMaps.append(slavePortMap);
        }
    }

    return portMaps;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::mapInstancePortToWire()
//-----------------------------------------------------------------------------
void VerilogGenerator::mapInstancePortToWire(QString const& instanceName, 
    QSharedPointer<General::PortMap> portMap, QString const& wireName, int const& wireSize)
{
    if (instanceWriters_.contains(instanceName))
    {
        QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter = instanceWriters_.value(instanceName);
        QSharedPointer<Component> instanceComponent = getComponentForInstanceName(instanceName);

        if (wireSize == instanceComponent->getPortWidth(portMap->physicalPort_))
        {
            instanceWriter->assignPortForFullWidth(portMap->physicalPort_, wireName);
        }
        else
        {
            General::PortBounds bounds = portMap->getLogicalRange(instanceComponent->getPort(portMap->physicalPort_));
            instanceWriter->assignPortForRange(portMap->physicalPort_, wireName, bounds.left_, bounds.right_);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::mapToConnectedInstances()
//-----------------------------------------------------------------------------
void VerilogGenerator::mapToConnectedInstances(Interface const& startInterface, QString const& logicalPort, 
    QString const& wireName, int const& wireSize)
{
    foreach(Interface endInterface, design_->getConnectedInterfaces(startInterface))
    {
        QString endInstanceName = endInterface.getComponentRef();
        foreach(QSharedPointer<General::PortMap> endPortMap, findPortMaps(logicalPort, endInterface))
        {
            mapInstancePortToWire(endInstanceName, endPortMap, wireName, wireSize);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::addWireWriter()
//-----------------------------------------------------------------------------
void VerilogGenerator::addWireWriter(QString wireName, int wireSize)
{
    wireWriters_.append(QSharedPointer<VerilogWireWriter>(new VerilogWireWriter(wireName, wireSize)));
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

        foreach(QSharedPointer<General::PortMap> topMap, topIf->getPortMaps())
        {
            foreach(QSharedPointer<General::PortMap> instanceMap, 
                findPortMaps(topMap->logicalPort_, instanceInterface))
            {
                QString port = instanceMap->physicalPort_;
                QString signal = topMap->physicalPort_;

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
    foreach(QSharedPointer<VerilogWireWriter> wireWriter, wireWriters_)
    {
        topWriter_->add(wireWriter);
    }

    foreach(QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter, instanceWriters_)
    {
        topWriter_->add(instanceWriter);
    }
}
