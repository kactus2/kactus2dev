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
#include <editors/ComponentEditor/common/MultipleParameterFinder.h>
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
#include <Plugins/VerilogGenerator/VerilogTiedValueWriter/VerilogTiedValueWriter.h>

#include <Plugins/VerilogImport/VerilogSyntax.h>

#include <QDateTime>
#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::VerilogGenerator()
//-----------------------------------------------------------------------------
VerilogGenerator::VerilogGenerator(LibraryInterface* library): QObject(0), 
library_(library),
headerWriter_(0),
topWriter_(0), 
topComponent_(),
design_(),
wireWriters_(),
tiedValueWriter_(),
instanceWriters_(),
sorter_(new InterfaceDirectionNameSorter())
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
void VerilogGenerator::parse(QSharedPointer<Component> component, QString topComponentView, 
    QString const& outputPath /*=""*/, QSharedPointer<Design> design /*= QSharedPointer<Design>()*/ )
{
    topComponent_ = component;
    topComponentView_ = topComponentView;
    design_ = design;

    initializeWriters();

    if (design_)
    {
		// If we are generating based on a design, it should contain stuff like instances and interconnections.
        createWritersForComponentInstances();

        connectAndWireAdHocConnections();

        connectHierarchicalConnectionsToInstances();
		createWiresForInterconnections(); 

		addWritersToTopInDesiredOrder();       
    }
	else
	{
		// If we are not generating based on a design, we must parse the existing implementation.
		QString implementation;
		QString postModule;

		if (!selectImplementation(outputPath, implementation, postModule))
		{
			// If parser says no-go, we dare not do nothing.
			return;
		}

		// Must add a warning before the existing implementation.
		QSharedPointer<CommentWriter> tagWriter(new CommentWriter(VerilogSyntax::TAG_OVERRIDE));
		topWriter_->add(tagWriter);

		// Next comes the implementation.
		QSharedPointer<TextBodyWriter> implementationWriter(new TextBodyWriter(implementation));
		topWriter_->add(implementationWriter);

		// Also write any stuff that comes after the actual module.
		QSharedPointer<TextBodyWriter> postModuleWriter(new TextBodyWriter(postModule));
		topWriter_->setPostModule( postModuleWriter );
	}
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::generate()
//-----------------------------------------------------------------------------
void VerilogGenerator::generate(QString const& outputPath) const
{
    if (nothingToWrite())
	{
		emit reportError(tr("Nothing to write"));
        return;
    }

	QFile outputFile(outputPath); 
    if (!outputFile.open(QIODevice::WriteOnly))
	{
		emit reportError(tr("Could not open output file for writing: %1").arg(outputPath));
        return;
    }

    QTextStream outputStream(&outputFile);

    QString fileName = QFileInfo(outputPath).fileName();

    headerWriter_->write(outputStream, fileName, topComponent_->getDescription(), QDateTime::currentDateTime());
    topWriter_->write(outputStream);

    outputFile.close();
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::selectImplementation()
//-----------------------------------------------------------------------------
bool VerilogGenerator::selectImplementation(QString const& outputPath, QString& implementation,
	QString& postModule) const
{
	// Check if the output file already exists.
	QFile outputFile(outputPath); 

	// If it does not, there is nothing do here.
	if (!outputFile.exists())
	{
		return true;
	}

	// Must be able to open it for reading.
	if (!outputFile.open(QIODevice::ReadOnly))
	{
		emit reportError(tr("Could not open output file for reading: %1").arg(outputPath));
		return false;
	}

	// Read the content.
	QTextStream outputStream(&outputFile);
	QString fileContent = outputStream.readAll();

	// We do not support multiple modules in the same file.
	if (fileContent.count(VerilogSyntax::MODULE_KEY_WORD) > 1)
	{
		emit reportError(tr("There was more than one module headers in the output file!"));
		return false;
	}

	// Find the beginning of the module header.
	int moduleDeclarationBeginIndex = fileContent.indexOf(VerilogSyntax::MODULE_KEY_WORD);

	// Must have it to proceed.
	if (moduleDeclarationBeginIndex == -1)
	{
		emit reportError(tr("Could not find module header start from the output file!"));
		return false;
	}

	// Find the ending of the module header.
	int moduleDeclarationEndIndex = fileContent.indexOf(");",moduleDeclarationBeginIndex);

	// Must have it to proceed.
	if (moduleDeclarationEndIndex == -1)
	{
		emit reportError(tr("Could not find module header end from the output file!"));
		return false;
	}

	// End of the header is the beginning of the implementation.
	int implementationStart = moduleDeclarationEndIndex + 2;
	int implementationEnd = fileContent.indexOf(VerilogSyntax::MODULE_END);

	// The module must end some where.
	if (implementationEnd == -1)
	{
		emit reportError(tr("Could not find module end from the output file!"));
		return false;
	}

	// Rip the implementation once detected.
	int implementationLength = implementationEnd - implementationStart;
	implementation = fileContent.mid(implementationStart,implementationLength);

	// Remove the tag, if it exists.
	implementation.remove("// " + VerilogSyntax::TAG_OVERRIDE);

	// Also trim away extra white space.
	implementation = implementation.trimmed();

	// Then take all the text that comes after the module, just in case.
	int postStart = implementationEnd + 9;
	postModule = fileContent.mid(postStart);

	// Also trim away extra white space.
	postModule = postModule.trimmed();

	// The destructor shall close the file. All done here.
	return true;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::initializeWriters()
//-----------------------------------------------------------------------------
void VerilogGenerator::initializeWriters()
{
    QSettings settings;
    QString currentUser = settings.value("General/Username").toString();
    QString componentXmlPath = library_->getPath(topComponent_->getVlnv());

    headerWriter_ = QSharedPointer<VerilogHeaderWriter>(new VerilogHeaderWriter(topComponent_->getVlnv(), 
        componentXmlPath, currentUser));

    QSharedPointer<ExpressionFormatter> topFormatter = createFormatterForComponent(topComponent_);

    topWriter_ = QSharedPointer<ComponentVerilogWriter>(new ComponentVerilogWriter(topComponent_,
		topComponentView_, sorter_, topFormatter));

    instanceWriters_.clear();

    wireWriters_ = QSharedPointer<WriterGroup>(new WriterGroup());

    tiedValueWriter_ = QSharedPointer<VerilogTiedValueWriter>(new VerilogTiedValueWriter(topFormatter));
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::createFormatterForComponent()
//-----------------------------------------------------------------------------
QSharedPointer<ExpressionFormatter> VerilogGenerator::createFormatterForComponent(QSharedPointer<Component> targetComponent)
{    
    if (targetComponent != topComponent_)
    {
        QSharedPointer<MultipleParameterFinder> finder(new MultipleParameterFinder());

        QSharedPointer<TopComponentParameterFinder> topFinder(new TopComponentParameterFinder(topComponent_));
        topFinder->setActiveView(topComponentView_);
        finder->addFinder(topFinder);
        finder->addFinder(QSharedPointer<ParameterFinder>(new ComponentParameterFinder(targetComponent)));
        return QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(finder));
    }
    else
    {
        QSharedPointer<ParameterFinder> parameterFinder(new ComponentParameterFinder(targetComponent));
        return QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(parameterFinder));
    }    
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
// Function: VerilogGenerator::primaryInterfaceIsUnique()
//-----------------------------------------------------------------------------
bool VerilogGenerator::primaryInterfaceIsUnique(QList<QSharedPointer<ActiveInterface> > const& connectedInterfaces,
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
// Function: VerilogGenerator::createWiresForInterface()
//-----------------------------------------------------------------------------
void VerilogGenerator::createWiresForInterface(QSharedPointer<ActiveInterface> interface)
{
    QSharedPointer<BusInterface> busInterface = getBusinterfaceForInterface(interface);

    if (busInterface)
    {
        QStringList logicalSignalsInInterface = busInterface->getLogicalPortNames();
        logicalSignalsInInterface.removeDuplicates();

        foreach(QString const& logicalSignal, logicalSignalsInInterface)
        {
            createWireForLogicalSignalInInterface(logicalSignal, interface);
        }
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
    bool alreadyConnected = false;

    foreach(QSharedPointer<Interconnection> wireConnection, *design_->getInterconnections())
    {
        QList<QSharedPointer<ActiveInterface> > interfaces = *wireConnection->getActiveInterfaces();
        interfaces.append(wireConnection->getStartInterface());

        foreach (QSharedPointer<ActiveInterface> connectionInterface, interfaces)
        {
            if (interfacesShareReferences(connectionInterface, interface))
            {
                wireName = wireConnection->name();

                if (alreadyConnected)
                {
                    return interface->getComponentReference() + "_" + interface->getBusReference();
                }

                alreadyConnected = true;
            }
        }
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
            connectedInterfaces.append(*interconnection->getActiveInterfaces());
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
        QSharedPointer<ComponentParameterFinder> instanceFinder(new ComponentParameterFinder(instanceComponent));
        IPXactSystemVerilogParser instanceParser(instanceFinder);

        if (portMap->getLogicalPort() && portMap->getLogicalPort()->range_)
        {
            bounds.first = instanceParser.parseExpression(portMap->getLogicalPort()->range_->getLeft()).toInt();
            bounds.second = instanceParser.parseExpression(portMap->getLogicalPort()->range_->getRight()).toInt();
        }
        else if (portMap->getPhysicalPort() && instanceComponent->hasPort(portMap->getPhysicalPort()->name_))
        {
            QSharedPointer<Port> physicalPort = instanceComponent->getPort(portMap->getPhysicalPort()->name_);
            bounds.first = instanceParser.parseExpression(physicalPort->getLeftBound()).toInt();
            bounds.second = instanceParser.parseExpression(physicalPort->getRightBound()).toInt();
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
void VerilogGenerator::connectInstancePortToWire(QString const& instanceName, QSharedPointer<PortMap> portMap,
    QString const& wireName, int const& wireSize)
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

        int physicalSize = abs(portLeft - portRight) + 1;

        int logicalLeft = wireSize - 1;
        int logicalRight = 0;

        if (portMap->getLogicalPort()->range_)
        {
            logicalLeft = instanceParser.parseExpression(portMap->getLogicalPort()->range_->getLeft()).toInt();
            logicalRight = instanceParser.parseExpression(portMap->getLogicalPort()->range_->getRight()).toInt();
        }

        int logicalSize = abs(logicalLeft - logicalRight) + 1;

        if ((wireSize == physicalSize && !portMap->getLogicalPort()->range_) ||
            (portMap->getLogicalPort()->range_ && wireSize == logicalSize))
        {
            instanceWriter->assignPortForFullWidth(portMap->getPhysicalPort()->name_, wireName);
        }
        else
        {
            if (logicalSize > physicalSize)
            {
                logicalLeft = logicalRight + physicalSize - 1;
            }

            instanceWriter->assignPortForRange(portMap->getPhysicalPort()->name_, wireName, logicalLeft,
                logicalRight);
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
    QSharedPointer<BusInterface> busInterface = getBusinterfaceForInterface(interface);
    if (busInterface)
    {
        return busInterface->getInterfaceMode();
    }

    return General::INTERFACE_MODE_COUNT;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::getBusinterfaceForInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> VerilogGenerator::getBusinterfaceForInterface(
    QSharedPointer<ActiveInterface> interface) const
{
    QSharedPointer<Component> component = getComponentForInstance(interface->getComponentReference());
    if (component)
    {
        return component->getBusInterface(interface->getBusReference());
    }

    return QSharedPointer<BusInterface>();
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
            QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter(new ComponentInstanceVerilogWriter(
                instance, component, sorter_, createFormatterForComponent(component)));
            instanceWriters_.insert(instance->getInstanceName(), instanceWriter);            
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::connectHierarchicalConnectionsToInstances()
//-----------------------------------------------------------------------------
void VerilogGenerator::connectHierarchicalConnectionsToInstances()
{
    if (!design_->getInterconnections()->isEmpty())
    {
        QSharedPointer<TopComponentParameterFinder> topFinder(new TopComponentParameterFinder(topComponent_));
        topFinder->setActiveView(topComponentView_);

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

    QSharedPointer<Component> instanceComponent = getComponentForInstance(instanceName);
    QSharedPointer<BusInterface> instanceBus = instanceComponent->getBusInterface(instanceInterface->getBusReference());

    QSharedPointer<ComponentParameterFinder> instanceFinder(new ComponentParameterFinder(instanceComponent));
    QSharedPointer<IPXactSystemVerilogParser> instanceParser(new IPXactSystemVerilogParser(instanceFinder));

    QSharedPointer<AbstractionDefinition> referencedAbstraction;
    if (topIf->getAbstractionTypes()->first()->getAbstractionRef())
    {
        VLNV abstractionVLNV = *topIf->getAbstractionTypes()->first()->getAbstractionRef();
        referencedAbstraction = library_->getModel(abstractionVLNV).dynamicCast<AbstractionDefinition>();
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

            QSharedPointer<Port> topComponentPort = topComponent_->getPort(topPhysicalPort->name_);
            if (topComponentPort)
            {
                int topPortLeft = topParser->parseExpression(topComponentPort->getLeftBound()).toInt();
                int topPortRight = topParser->parseExpression(topComponentPort->getRightBound()).toInt();
                int topPortWidth = abs(topPortLeft - topPortRight) + 1;

                QString topLogicalWidth("x");
                QString instanceLogicalWidth("x");
                if (referencedAbstraction)
                {
                    QSharedPointer<PortAbstraction> logicalSignal = referencedAbstraction->getPort(topLogicalPort->name_);
                    topLogicalWidth = abstractionParser.parseExpression(
                        logicalSignal->getWire()->getWidth(topIf->getInterfaceMode()));
                    instanceLogicalWidth = abstractionParser.parseExpression(
                        logicalSignal->getWire()->getWidth(instanceBus->getInterfaceMode()));
                }

                PortAlignment topPortAlignment = calculateMappedPortBounds(topMap, topPortLeft, topPortRight, 
                    topParser, topLogicalWidth);

                foreach(QSharedPointer<PortMap> instancePortMap, matchingInstancePortMaps)
                {
                    QSharedPointer<PortMap::PhysicalPort> instancePhysicalPort = instancePortMap->getPhysicalPort();

                    int instancePortLeft = instanceParser->parseExpression(
                        instanceComponent->getPort(instancePhysicalPort->name_)->getLeftBound()).toInt();
                    int instancePortRight = instanceParser->parseExpression(
                        instanceComponent->getPort(instancePhysicalPort->name_)->getRightBound()).toInt();
                    int instancePortWidth = abs(instancePortLeft - instancePortRight) + 1;

                    PortAlignment instancePortAlignment = calculateMappedPortBounds(
                        instancePortMap, instancePortLeft, instancePortRight, instanceParser, instanceLogicalWidth);

                    if (topPortAlignment.isValid() && instancePortAlignment.isValid())
                    {
                        if (fullWidthConnectionIsValid(topPortAlignment, instancePortAlignment, topPortWidth,
                            instancePortWidth))
                        {
                            instanceWriter->assignPortForFullWidth(instancePhysicalPort->name_, topPhysicalPort->name_);
                        }
                        else
                        {
                            int alignmentMax = topPortAlignment.getRightAlignment() + abs(
                                instancePortAlignment.getLeftAlignment() - instancePortAlignment.getRightAlignment());

                            alignmentMax = qMin(alignmentMax, topPortAlignment.getLeftAlignment());

                            instanceWriter->assignPortForRange(instancePhysicalPort->name_, topPhysicalPort->name_,
                                alignmentMax, topPortAlignment.getRightAlignment());
                        }
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::fullWidthConnectionIsValid()
//-----------------------------------------------------------------------------
bool VerilogGenerator::fullWidthConnectionIsValid(PortAlignment const& topAlignment,
    PortAlignment const&instanceAlignment, int topWidth, int instanceWidth) const
{
    return topWidth == instanceWidth &&
        topAlignment.getLeftAlignment() == instanceAlignment.getLeftAlignment() &&
        topAlignment.getRightAlignment() == instanceAlignment.getRightAlignment();
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::calculateMappedPortBounds()
//-----------------------------------------------------------------------------
PortAlignment VerilogGenerator::calculateMappedPortBounds(QSharedPointer<PortMap> containingMap,
    int portLeft, int portRight, QSharedPointer<ExpressionParser> parser, QString logicalPortWidth) const
{
    int maxAlignment = qMax(portLeft, portRight);
    int minAlignment = qMin(portLeft, portRight);
    PortAlignment mappedPort(maxAlignment, minAlignment);

    QSharedPointer<PortMap::PhysicalPort> physicalPort = containingMap->getPhysicalPort();
    QSharedPointer<PortMap::LogicalPort> logicalPort = containingMap->getLogicalPort();

    if (physicalPort && containingMap->getPhysicalPort()->partSelect_)
    {
        int partLeft = parser->parseExpression(physicalPort->partSelect_->getLeftRange()).toInt();
        int partRight = parser->parseExpression(physicalPort->partSelect_->getRightRange()).toInt();

        int partMax = qMax(partLeft, partRight);
        int partMin = qMin(partLeft, partRight);

        mappedPort.setLeftAlignment(qMin(partMax, maxAlignment));
        mappedPort.setRightAlignment(qMax(partMin, minAlignment));
    }
    
    if (logicalPort && logicalPort->range_ && !logicalPortWidth.contains("x"))
    {
        int logicalWidth = logicalPortWidth.toInt();
        int logicalLeft = parser->parseExpression(logicalPort->range_->getLeft()).toInt();
        int logicalRight = parser->parseExpression(logicalPort->range_->getRight()).toInt();

        if (logicalLeft > logicalWidth && logicalRight > logicalWidth)
        {
            mappedPort.setAlignmentValidity(false);
        }
        else
        {
            int logicalMax = qMin(logicalWidth, qMax(logicalLeft, logicalRight));
            int logicalMin = qMin(logicalLeft, logicalRight);

            mappedPort.setLeftAlignment(qMin(logicalMax, mappedPort.getLeftAlignment()));
            mappedPort.setRightAlignment(qMax(logicalMin, mappedPort.getRightAlignment()));
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
        if (!adHocConnection->getTiedValue().isEmpty())
        {
            connectTieOffConnection(adHocConnection);
        }
        else if (isHierarchicalAdHocConnection(adHocConnection))
        {
            connectInstancePortsToTopPort(adHocConnection);   
        }
    }

    foreach(QSharedPointer<PortReference> primaryPort, findPrimaryPortsInAdHocConnections())
    {
        wireConnectedPorts(primaryPort);
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::connectTieOffConnection()
//-----------------------------------------------------------------------------
void VerilogGenerator::connectTieOffConnection(QSharedPointer<AdHocConnection> adHocConnection)
{
    if (!adHocConnection->getInternalPortReferences()->isEmpty())
    {
        QSharedPointer<PortReference> portReference = adHocConnection->getInternalPortReferences()->first();

        QSharedPointer<Component> instanceComponent = getComponentForInstance(portReference->getComponentRef());

        QString tieOffValue =
            getConnectionTieOffValue(adHocConnection, instanceComponent, portReference, DirectionTypes::IN);

        if (instanceWriters_.contains(portReference->getComponentRef()))
        {
            QSharedPointer<ComponentInstanceVerilogWriter> containingInstanceWriter =
                instanceWriters_.value(portReference->getComponentRef());

            containingInstanceWriter->assignPortTieOff(portReference->getPortRef(), tieOffValue);
        }
    }
    else if (!adHocConnection->getExternalPortReferences()->isEmpty())
    {
        QSharedPointer<PortReference> portReference = adHocConnection->getExternalPortReferences()->first();

        QString tieOffValue =
            getConnectionTieOffValue(adHocConnection, topComponent_, portReference, DirectionTypes::OUT);

        if (!tieOffValue.isEmpty() && tiedValueWriter_)
        {
            tiedValueWriter_->addPortTiedValue(portReference->getPortRef(), tieOffValue);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::getConnectionTieOffValue()
//-----------------------------------------------------------------------------
QString VerilogGenerator::getConnectionTieOffValue(QSharedPointer<AdHocConnection> adHocConnection,
    QSharedPointer<Component> containingComponent, QSharedPointer<PortReference> portReference,
    DirectionTypes::Direction requiredDirection) const
{
    QString tieOffValue = "";

    if (containingComponent)
    {
        QSharedPointer<Port> referencedPort = containingComponent->getPort(portReference->getPortRef());
        if (referencedPort && referencedPort->getDirection() == requiredDirection ||
            referencedPort->getDirection() == DirectionTypes::INOUT)
        {
            tieOffValue = adHocConnection->getTiedValue();
            if (QString::compare(tieOffValue, "default", Qt::CaseInsensitive) == 0)
            {
                tieOffValue = referencedPort->getDefaultValue();
            }
            else if (QString::compare(tieOffValue, "open", Qt::CaseInsensitive) == 0)
            {
                tieOffValue = "";
            }
        }
    }

    return tieOffValue;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::isHierarchicalAdHocConnection()
//-----------------------------------------------------------------------------
bool VerilogGenerator::isHierarchicalAdHocConnection(QSharedPointer<AdHocConnection> adHocConnection) const
{
    return !adHocConnection->getExternalPortReferences()->isEmpty();
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::connectInstancePortsToTopPort()
//-----------------------------------------------------------------------------
void VerilogGenerator::connectInstancePortsToTopPort(QSharedPointer<AdHocConnection> adHocConnection)
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
// Function: VerilogGenerator::findPrimaryPortsInAdHocConnections()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<PortReference> > VerilogGenerator::findPrimaryPortsInAdHocConnections() const
{
    QVector<QSharedPointer<PortReference> > primaryPorts;

    foreach(QSharedPointer<AdHocConnection> adHocConnection, *design_->getAdHocConnections())
    {
        if (adHocConnection->getTiedValue().isEmpty() && !isHierarchicalAdHocConnection(adHocConnection) &&
            shouldCreateWireForAdHocConnection(adHocConnection))
        {
            foreach(QSharedPointer<PortReference> internalPort, *adHocConnection->getInternalPortReferences())
            {
                QSharedPointer<Port> instancePort = findPhysicalPort(internalPort);
                if (instancePort && instancePort->getDirection() == DirectionTypes::OUT)
                {
                    if (isUniquePortReference(internalPort, primaryPorts))
                    {
                        primaryPorts.append(internalPort);
                    }                        
                }
            }
        }
    }

    return primaryPorts;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::findPhysicalPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> VerilogGenerator::findPhysicalPort(QSharedPointer<PortReference> portReference) const
{
    QSharedPointer<Component> instanceComponent = getComponentForInstance(portReference->getComponentRef());
    if (instanceComponent)
    {
        return instanceComponent->getPort(portReference->getPortRef());
    }

    return QSharedPointer<Port>(0);
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::isUniquePortReference()
//-----------------------------------------------------------------------------
bool VerilogGenerator::isUniquePortReference(QSharedPointer<PortReference> reference, 
    QVector<QSharedPointer<PortReference> > const& otherReferences) const
{
    foreach(QSharedPointer<PortReference> listedReference, otherReferences)
    {
        if (referencesTheSamePort(reference, listedReference))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::referencesTheSamePort()
//-----------------------------------------------------------------------------
bool VerilogGenerator::referencesTheSamePort(QSharedPointer<PortReference> firstPort,
    QSharedPointer<PortReference> secondPort) const
{
    return firstPort->getComponentRef() == secondPort->getComponentRef() &&
        firstPort->getPortRef() == secondPort->getPortRef();
}
  
//-----------------------------------------------------------------------------
// Function: VerilogGenerator::wireConnectedPorts()
//-----------------------------------------------------------------------------
void VerilogGenerator::wireConnectedPorts(QSharedPointer<PortReference> primaryPort)
{
    QSharedPointer<AdHocConnection> primaryConnection = findAdHocConnectionForPort(primaryPort);
    QVector<QSharedPointer<PortReference> > connectedPorts = findConnectedPorts(primaryPort);

    QString wireName = primaryConnection->name();
    if (connectedPorts.count() > 1)
    {
        wireName = primaryPort->getPortRef() + "_from_" + primaryPort->getComponentRef();
    }

    int wireSize = findWireSizeForAdHocConnection(primaryConnection);

    connectPortToWire(primaryPort, wireName, wireSize);

    foreach(QSharedPointer<PortReference> port, connectedPorts)
    {
        connectPortToWire(port, wireName, wireSize);
    }

    addWriterForWire(wireName, wireSize);
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::findAdHocConnectionForPort()
//-----------------------------------------------------------------------------
QSharedPointer<AdHocConnection> VerilogGenerator::findAdHocConnectionForPort(
    QSharedPointer<PortReference> primaryPort) const
{
    foreach(QSharedPointer<AdHocConnection> adHocConnection, *design_->getAdHocConnections())
    {
        if (adHocConnection->getInternalPortReferences()->contains(primaryPort))
        {
            return adHocConnection;
        }
    }       
    
    return QSharedPointer<AdHocConnection>();
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::findConnectedPorts()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<PortReference> > VerilogGenerator::findConnectedPorts(QSharedPointer<PortReference> primaryPort) const
{
    QList<QSharedPointer<PortReference> > connectedPorts;

    foreach(QSharedPointer<AdHocConnection> adHocConnection, *design_->getAdHocConnections())
    {
        foreach(QSharedPointer<PortReference> internalPort, *adHocConnection->getInternalPortReferences())
        {
            if (referencesTheSamePort(primaryPort, internalPort))
            {             
                connectedPorts.append(*adHocConnection->getInternalPortReferences());
                connectedPorts.removeOne(internalPort);
            }
        }
    }

    return connectedPorts.toVector();
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::shouldCreateWireForAdHocConnection()
//-----------------------------------------------------------------------------
bool VerilogGenerator::shouldCreateWireForAdHocConnection(QSharedPointer<AdHocConnection> adHocConnection) const
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
        else if (part)
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
// Function: VerilogGenerator::connectPortToWire()
//-----------------------------------------------------------------------------
void VerilogGenerator::connectPortToWire(QSharedPointer<PortReference> port, QString const& wireName, int wireSize)
{
    if (instanceWriters_.contains(port->getComponentRef()))
    {
        QSharedPointer<ComponentInstanceVerilogWriter> instanceWriter = instanceWriters_.value(
            port->getComponentRef());

        QSharedPointer<Component> referencedComponent = getComponentForInstance(port->getComponentRef());
        QSharedPointer<Port> referencedPort = referencedComponent->getPort(port->getPortRef());

        QSharedPointer<ComponentParameterFinder> finder(new ComponentParameterFinder(referencedComponent));
        IPXactSystemVerilogParser parser(finder);

        int portLeft = parser.parseExpression(referencedPort->getLeftBound()).toInt();
        int portRight = parser.parseExpression(referencedPort->getRightBound()).toInt();

        if (port->getPartSelect())
        {
            portLeft = parser.parseExpression(port->getPartSelect()->getLeftRange()).toInt();
            portRight = parser.parseExpression(port->getPartSelect()->getRightRange()).toInt();
        }

        int portWidth = abs(portLeft - portRight) + 1;

        if (portWidth == wireSize)
        {
            instanceWriter->assignPortForFullWidth(port->getPortRef(), wireName);
        }
        else
        {
            int connectionLeft = qMin(portWidth, wireSize) - 1;
            instanceWriter->assignPortForRange(port->getPortRef(), wireName, connectionLeft, 0);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGenerator::addWritersToTopInDesiredOrder()
//-----------------------------------------------------------------------------
void VerilogGenerator::addWritersToTopInDesiredOrder() const
{
    topWriter_->add(wireWriters_);

    topWriter_->add(tiedValueWriter_);

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

