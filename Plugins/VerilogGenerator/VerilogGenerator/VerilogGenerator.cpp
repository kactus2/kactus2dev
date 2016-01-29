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

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <designEditors/common/TopComponentParameterFinder.h>

#include <IPXACTmodels/common/PortAlignment.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>

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

        connectHierarchicalConnectionsToInstances(topComponentView);
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

            if (primaryInterfaceIsUnique(connectedStartInterfaces, startInterface))
            {
                connectedStartInterfaces.append(startInterface);
            }
        }

    }

    return connectedStartInterfaces;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::isDuplicatePrimaryInterface()
//-----------------------------------------------------------------------------
bool VerilogGenerator::primaryInterfaceIsUnique(QList<QSharedPointer<ActiveInterface> > connectedInterfaces,
    QSharedPointer<ActiveInterface> primaryInterface) const
{
    foreach (QSharedPointer<ActiveInterface> comparisonInterface, connectedInterfaces)
    {
        if (interfacesShareReferences(comparisonInterface, primaryInterface))
        {
            return false;
        }
    }

    return true;
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
QString VerilogGenerator::generateWireName(QSharedPointer<ActiveInterface> primaryInterface,
    QString const& logicalPort)
{
    QString wireName = findConnectionNameForInterface(primaryInterface);

    return wireName + "_" + logicalPort;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::findConnectionNameForInterface()
//-----------------------------------------------------------------------------
QString VerilogGenerator::findConnectionNameForInterface(QSharedPointer<ActiveInterface> interface)
{
    QString wireName ("");
    int wireConnectionsCount = 0;

    foreach(QSharedPointer<Interconnection> wireConnection, *design_->getInterconnections())
    {
        if (interfacesShareReferences(wireConnection->getStartInterface(), interface))
        {
            wireName = wireConnection->name();

            wireConnectionsCount = wireConnectionsCount + wireConnection->getActiveInterfaces()->count();
        }
        else
        {
            foreach (QSharedPointer<ActiveInterface> connectionInterface, *wireConnection->getActiveInterfaces())
            {
                if (interfacesShareReferences(connectionInterface, interface))
                {
                    wireName = wireConnection->name();
                    wireConnectionsCount = wireConnectionsCount + wireConnection->getActiveInterfaces()->count();
                }
            }
        }
    }

    if (wireConnectionsCount != 1)
    {
        wireName = interface->getComponentReference() + "_" + interface->getBusReference();
    }

    return wireName;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::findWireSize()
//-----------------------------------------------------------------------------
int VerilogGenerator::findWireSize(QSharedPointer<ActiveInterface> primaryInterface, QString const& logicalName)
{
    int highestBound =  findHighestMappedLogicalIndexInInterface(logicalName, primaryInterface);

    QList<QSharedPointer<ActiveInterface> > connectedInterfaces = getConnectedInterfaces(primaryInterface);
    foreach (QSharedPointer<ActiveInterface> wireInterface, connectedInterfaces)
    {
        int interfaceHighBound = findHighestMappedLogicalIndexInInterface(logicalName, wireInterface);
        highestBound = qMax(highestBound, interfaceHighBound);
    }

    return highestBound + 1;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::getConnectedInterfaces()
//-----------------------------------------------------------------------------
QList<QSharedPointer<ActiveInterface> > VerilogGenerator::getConnectedInterfaces(
    QSharedPointer<ActiveInterface> primaryInterface) const
{
    QList<QSharedPointer<ActiveInterface> > connectedInterfaces;

    foreach (QSharedPointer<Interconnection> interconnection, *design_->getInterconnections())
    {
        QSharedPointer<ActiveInterface> startInterface = interconnection->getStartInterface();
        if (interfacesShareReferences(startInterface, primaryInterface))
        {
            foreach (QSharedPointer<ActiveInterface> connectionInterface, *interconnection->getActiveInterfaces())
            {
                connectedInterfaces.append(connectionInterface);
            }
        }
        else
        {
            foreach (QSharedPointer<ActiveInterface> connectionInterface, *interconnection->getActiveInterfaces())
            {
                if (interfacesShareReferences(connectionInterface, primaryInterface))
                {
                    connectedInterfaces.append(startInterface);
                    break;
                }
            }
        }
    }

    return connectedInterfaces;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::interfacesShareReferences()
//-----------------------------------------------------------------------------
bool VerilogGenerator::interfacesShareReferences(QSharedPointer<ActiveInterface> comparisonInterface,
    QSharedPointer<ActiveInterface> comparedInterface) const
{
    return comparisonInterface->getComponentReference() == comparedInterface->getComponentReference() &&
        comparisonInterface->getBusReference() == comparedInterface->getBusReference();
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
        QPair<int, int> logicalBounds = logicalBoundsInInstance(interface->getComponentReference(), startPortMap);
        highestBound = qMax(highestBound, qMax(logicalBounds.first, logicalBounds.second));
    }
    
    return highestBound;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::logicalBoundsInInstance()
//-----------------------------------------------------------------------------
QPair<int, int> VerilogGenerator::logicalBoundsInInstance(QString const& instanceName,
    QSharedPointer<PortMap> portMap) const
{
    QPair<int, int> bounds(0, 0);

    QSharedPointer<Component> instanceComponent = getComponentForInstance(instanceName);
    if (instanceComponent)
    {
        if (portMap->getLogicalPort() && portMap->getLogicalPort()->range_)
        {
            QSharedPointer<ComponentParameterFinder> instanceFinder (
                new ComponentParameterFinder(instanceComponent));
            IPXactSystemVerilogParser instanceParser(instanceFinder);

            bounds.first = instanceParser.parseExpression(portMap->getLogicalPort()->range_->getLeft()).toInt();
            bounds.second = instanceParser.parseExpression(portMap->getLogicalPort()->range_->getRight()).toInt();
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
    QList<QSharedPointer<ActiveInterface> > connectedInterfaces = getConnectedInterfaces(startInterface);
    foreach (QSharedPointer<ActiveInterface> wireInterface, connectedInterfaces)
    {
        connectPortsInInterfaceToWire(wireInterface, logicalPort, wireName, wireSize);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::connectPortsInInterfaceToWire()
//-----------------------------------------------------------------------------
void VerilogGenerator::connectPortsInInterfaceToWire(QSharedPointer<ActiveInterface> interface,
    QString const& logicalPort, QString const& wireName, int const& wireSize)
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
    QSharedPointer<PortMap::PhysicalPort> mappedPhysical = portMap->getPhysicalPort();
    if (instanceComponent && mappedPhysical)
    {
        QSharedPointer<ComponentParameterFinder> instanceFinder (new ComponentParameterFinder(instanceComponent));
        IPXactSystemVerilogParser instanceParser(instanceFinder);

        QSharedPointer<Port> physicalPort = instanceComponent->getPort(mappedPhysical->name_);
        int portLeft = instanceParser.parseExpression(physicalPort->getLeftBound()).toInt();
        int portRight = instanceParser.parseExpression(physicalPort->getRightBound()).toInt();

        int portWidth = abs(portLeft - portRight) + 1;
        if (wireSize == portWidth)
        {
            instanceWriter->assignPortForFullWidth(portMap->getPhysicalPort()->name_, wireName);
        }
        else
        {
            int logicalLeft = wireSize - 1;
            int logicalRight = 0;
            if (portMap->getLogicalPort()->range_)
            {
                logicalLeft = instanceParser.parseExpression(portMap->getLogicalPort()->range_->getLeft()).toInt();
                logicalRight =
                    instanceParser.parseExpression(portMap->getLogicalPort()->range_->getRight()).toInt();
            }

            instanceWriter->assignPortForRange(
                portMap->getPhysicalPort()->name_, wireName, logicalLeft, logicalRight);
        }
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
QSharedPointer<BusInterface> VerilogGenerator::getBusinterfaceForInterface(
    QSharedPointer<ActiveInterface> interface) const
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
void VerilogGenerator::connectHierarchicalConnectionsToInstances(QString const& topComponentView)
{
    if (!design_->getInterconnections()->isEmpty())
    {
        QSharedPointer<TopComponentParameterFinder> topFinder (new TopComponentParameterFinder(topComponent_));
        topFinder->setActiveView(topComponentView);

        QSharedPointer<IPXactSystemVerilogParser> topParser(new IPXactSystemVerilogParser(topFinder));

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
                    connectTopBusInterfaceToInterfaceInInstance(topBusInterface, topParser,
                        connection->getStartInterface());
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::connectTopBusInterfaceToInterfaceInInstance()
//-----------------------------------------------------------------------------
void VerilogGenerator::connectTopBusInterfaceToInterfaceInInstance(QSharedPointer<BusInterface> topIf,
    QSharedPointer<ExpressionParser> topParser, QSharedPointer<ActiveInterface> instanceInterface)
{
    QString instanceName = instanceInterface->getComponentReference();    
    if (!instanceWriters_.contains(instanceName))
    {
        return;
    }

    QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter = instanceWriters_.value(instanceName);

    QSharedPointer<Component> instanceComponent = getComponentForInstance(instanceInterface->getComponentReference());
    QSharedPointer<BusInterface> instanceBus =
        instanceComponent->getBusInterface(instanceInterface->getBusReference());

    QSharedPointer<ComponentParameterFinder> instanceFinder (new ComponentParameterFinder(instanceComponent));
    QSharedPointer<IPXactSystemVerilogParser> instanceParser (new IPXactSystemVerilogParser(instanceFinder));

    QSharedPointer<AbstractionDefinition> referencedAbstraction;
    if (topIf->getAbstractionTypes()->first()->getAbstractionRef())
    {
        referencedAbstraction = library_->getModel(
            *topIf->getAbstractionTypes()->first()->getAbstractionRef().data())
            .dynamicCast<AbstractionDefinition>();
    }
    SystemVerilogExpressionParser abstractionParser;

    foreach(QSharedPointer<PortMap> topMap, *topIf->getPortMaps())
    {
        QList<QSharedPointer<PortMap> > matchingInstancePortMaps =
            findPortMapsForLogicalPortInInterface(topMap->getLogicalPort()->name_, instanceInterface);

        if (!matchingInstancePortMaps.isEmpty())
        {
            QSharedPointer<PortMap::PhysicalPort> topPhysicalPort = topMap->getPhysicalPort();
            QSharedPointer<PortMap::LogicalPort> topLogicalPort = topMap->getLogicalPort();

            int topPortLeft = topParser->parseExpression(
                topComponent_->getPort(topPhysicalPort->name_)->getLeftBound()).toInt();
            int topPortRight = topParser->parseExpression(
                topComponent_->getPort(topPhysicalPort->name_)->getRightBound()).toInt();
            int topPortWidth = abs(topPortLeft - topPortRight) + 1;

            QString topLogicalWidth("x");
            QString instanceLogicalWidth("x");
            if (referencedAbstraction)
            {
                QSharedPointer<PortAbstraction> referencedLogicalPort =
                    referencedAbstraction->getPort(topLogicalPort->name_);
                topLogicalWidth = abstractionParser.parseExpression(
                    referencedLogicalPort->getWire()->getWidth(topIf->getInterfaceMode()));
                instanceLogicalWidth = abstractionParser.parseExpression(
                    referencedLogicalPort->getWire()->getWidth(instanceBus->getInterfaceMode()));
            }

            QSharedPointer<PortAlignment> topPortAlignment = calculateMappedPortBounds(
                topMap, topPortLeft, topPortRight, topParser, topLogicalWidth);

            foreach(QSharedPointer<PortMap> instancePortMap, 
                findPortMapsForLogicalPortInInterface(topMap->getLogicalPort()->name_, instanceInterface))
            {
                QSharedPointer<PortMap::PhysicalPort> instancePhysicalPort = instancePortMap->getPhysicalPort();

                int instancePortLeft = instanceParser->parseExpression(
                    instanceComponent->getPort(instancePhysicalPort->name_)->getLeftBound()).toInt();
                int instancePortRight = instanceParser->parseExpression(
                    instanceComponent->getPort(instancePhysicalPort->name_)->getRightBound()).toInt();
                int instancePortWidth = abs(instancePortLeft - instancePortRight) + 1;

                QSharedPointer<PortAlignment> instancePortAlignment = calculateMappedPortBounds(
                    instancePortMap, instancePortLeft, instancePortRight, instanceParser, instanceLogicalWidth);

                if (topPortAlignment->isValid() && instancePortAlignment->isValid())
                {
                    if (fullWidthConnectionIsValid(topPortAlignment, instancePortAlignment, topPortWidth,
                        instancePortWidth))
                    {
                        instanceWriter->assignPortForFullWidth(instancePhysicalPort->name_, topPhysicalPort->name_);
                    }
                    else
                    {
                        int alignmentMax = topPortAlignment->getRightAlignment() + abs(
                            instancePortAlignment->getLeftAlignment() - instancePortAlignment->getRightAlignment());

                        if (alignmentMax > topPortAlignment->getLeftAlignment())
                        {
                            alignmentMax = topPortAlignment->getLeftAlignment();
                        }

                        instanceWriter->assignPortForRange(instancePhysicalPort->name_, topPhysicalPort->name_,
                            alignmentMax, topPortAlignment->getRightAlignment());
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::fullWidthConnectionIsValid()
//-----------------------------------------------------------------------------
bool VerilogGenerator::fullWidthConnectionIsValid(QSharedPointer<PortAlignment> topAlignment,
    QSharedPointer<PortAlignment> instanceAlignment, int topWidth, int instanceWidth) const
{
    return topWidth == instanceWidth &&
        topAlignment->getLeftAlignment() == instanceAlignment->getLeftAlignment() &&
        topAlignment->getRightAlignment() == instanceAlignment->getRightAlignment();
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::calculateMappedPortBounds()
//-----------------------------------------------------------------------------
QSharedPointer<PortAlignment> VerilogGenerator::calculateMappedPortBounds(QSharedPointer<PortMap> containingMap,
    int portLeft, int portRight, QSharedPointer<ExpressionParser> parser, QString logicalPortWidth) const
{
    int maxAlignment = qMax(portLeft, portRight);
    int minAlignment = qMin(portLeft, portRight);
    QSharedPointer<PortAlignment> mappedPort (new PortAlignment(maxAlignment, minAlignment));

    if (containingMap->getPhysicalPort() && containingMap->getPhysicalPort()->partSelect_)
    {
        int partLeft =
            parser->parseExpression(containingMap->getPhysicalPort()->partSelect_->getLeftRange()).toInt();
        int partRight =
            parser->parseExpression(containingMap->getPhysicalPort()->partSelect_->getRightRange()).toInt();

        int partMax = qMax(partLeft, partRight);
        int partMin = qMin(partLeft, partRight);

        if (partMax < maxAlignment)
        {
            mappedPort->setLeftAlignment(partMax);
        }
        if (partMin > minAlignment)
        {
            mappedPort->setRightAlignment(partMin);
        }
    }

    int logicalWidthInt = logicalPortWidth.toInt();
    if (containingMap->getLogicalPort() && containingMap->getLogicalPort()->range_ &&
        !logicalPortWidth.contains("x"))
    {
        int logicalLeft = parser->parseExpression(containingMap->getLogicalPort()->range_->getLeft()).toInt();
        int logicalRight = parser->parseExpression(containingMap->getLogicalPort()->range_->getRight()).toInt();

        if (logicalLeft > logicalWidthInt && logicalRight > logicalWidthInt)
        {
            mappedPort->setAlignmentValidity(false);
        }
        else
        {
            int logicalMax = qMax(logicalLeft, logicalRight);
            int logicalMin = qMin(logicalLeft, logicalRight);
            if (logicalMax > logicalWidthInt)
            {
                logicalMax = logicalWidthInt;
            }

            if (logicalMax < mappedPort->getLeftAlignment())
            {
                mappedPort->setLeftAlignment(logicalMax);
            }
            if (logicalMin > mappedPort->getRightAlignment())
            {
                mappedPort->setRightAlignment(logicalMin);
            }
        }
    }

    return mappedPort;
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
        QSharedPointer<Component> component = getComponentForInstance(internalRef->getComponentRef());

        QSharedPointer<ComponentParameterFinder> instanceFinder (new ComponentParameterFinder(component));
        IPXactSystemVerilogParser instanceParser(instanceFinder);

        QSharedPointer<PartSelect> part = internalRef->getPartSelect();
        if (part.isNull() && component)
        {
            QSharedPointer<Port> referencedPort = component->getPort(internalRef->getPortRef());
            int portLeft = instanceParser.parseExpression(referencedPort->getLeftBound()).toInt();
            int portRight = instanceParser.parseExpression(referencedPort->getRightBound()).toInt();

            portSize = abs(portLeft - portRight) + 1;
        }
        else
        {
            int partLeft = instanceParser.parseExpression(part->getLeftRange()).toInt();
            int partRight = instanceParser.parseExpression(part->getRightRange()).toInt();

            portSize = abs(partLeft - partRight) + 1;
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
