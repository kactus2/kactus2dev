//-----------------------------------------------------------------------------
// File: ConnectivityGraphFactory.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.07.2016
//
// Description:
// Creates a connectivity graph from a given design and design configuration.
//-----------------------------------------------------------------------------

#include "ConnectivityGraphFactory.h"

#include <library/LibraryManager/libraryinterface.h>

#include "ConnectivityConnection.h"
#include "ConnectivityInterface.h"
#include "ConnectivityComponent.h"
#include "MemoryItem.h"
#include "ConnectivityGraph.h"

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ParameterCache.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Channel.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/MasterInterface.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryRemap.h>
#include <IPXACTmodels/Component/MirroredSlaveInterface.h>
#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/SlaveInterface.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::ConnectivityGraphFactory()
//-----------------------------------------------------------------------------
ConnectivityGraphFactory::ConnectivityGraphFactory(LibraryInterface* library):
library_(library), parameterFinder_(new ParameterCache(QSharedPointer<Component>())), 
    expressionParser_(new IPXactSystemVerilogParser(parameterFinder_))
{

}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::~ConnectivityGraphFactory()
//-----------------------------------------------------------------------------
ConnectivityGraphFactory::~ConnectivityGraphFactory()
{

}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::createConnectivityGraph()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectivityGraph> ConnectivityGraphFactory::createConnectivityGraph(
    QSharedPointer<const Design> design, QSharedPointer<const DesignConfiguration> designConfiguration)
{
    QSharedPointer<ConnectivityGraph> graph(new ConnectivityGraph());

    if (design)
    {
        analyzeDesign(design, designConfiguration, "", graph);
    }

    return graph;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::analyzeDesign()
//-----------------------------------------------------------------------------
void ConnectivityGraphFactory::analyzeDesign(QSharedPointer<const Design> design,
    QSharedPointer<const DesignConfiguration> designConfiguration, QString const& topInstance, 
    QSharedPointer<ConnectivityGraph> graph)
{
    addInstancesAndInterfaces(design, designConfiguration, graph);

    addConnections(design, topInstance, graph);
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::addInstancesAndInterfaces()
//-----------------------------------------------------------------------------
void ConnectivityGraphFactory::addInstancesAndInterfaces(QSharedPointer<const Design> design, 
    QSharedPointer<const DesignConfiguration> designConfiguration, QSharedPointer<ConnectivityGraph> graph)
{
    foreach (QSharedPointer<ComponentInstance> componentInstance, *design->getComponentInstances())
    {
        QSharedPointer<Document> instanceDocument = library_->getModel(*componentInstance->getComponentRef());
        QSharedPointer<Component> instancedComponent = instanceDocument.dynamicCast<Component>();

        if (instancedComponent)
        {
            parameterFinder_->setComponent(instancedComponent);

            QString activeView;
            if (designConfiguration)
            {
                activeView = designConfiguration->getActiveView(componentInstance->getInstanceName());
            }

            QSharedPointer<ConnectivityComponent> instanceNode = createInstanceData(componentInstance, 
                instancedComponent, activeView);

            foreach (QSharedPointer<BusInterface> busInterface, *instancedComponent->getBusInterfaces())
            {
                graph->addInterface(createInterfaceData(busInterface, instanceNode));
            }

            foreach (QSharedPointer<Channel> channel, *instancedComponent->getChannels())
            {
                QStringList channelInterfaces = channel->getInterfaces();

                QString startInterfaceName = channelInterfaces.takeFirst();
                QSharedPointer<ConnectivityInterface> startInterface = 
                    graph->getInterface(componentInstance->getInstanceName(), startInterfaceName);

                foreach (QString const& endInterface, channelInterfaces)
                {
                    graph->addConnection(channel->name(), startInterface, 
                        graph->getInterface(componentInstance->getInstanceName(), endInterface));
                }
            }

            foreach (QSharedPointer<BusInterface> busInterface, *instancedComponent->getBusInterfaces())
            {
                if (busInterface->hasBridge())
                {
                    QSharedPointer<ConnectivityInterface> startInterface = 
                        graph->getInterface(componentInstance->getInstanceName(), busInterface->name());
                    startInterface->setBridged();

                    foreach (QSharedPointer<SlaveInterface::Bridge> bridge, *busInterface->getSlave()->getBridges())
                    {
                        QSharedPointer<ConnectivityInterface> endInterface = graph->getInterface(
                            componentInstance->getInstanceName(), bridge->masterRef_);
                        if (endInterface)
                        {
                            endInterface->setBridged();

                            graph->addConnection(busInterface->name() + QObject::tr("_bridge"), startInterface, 
                                endInterface);
                        }
                    }

                }
            }

            graph->addInstance(instanceNode);

            addSubInstances(instanceNode, instancedComponent, designConfiguration, graph);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::getInstanceData()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectivityComponent> ConnectivityGraphFactory::createInstanceData(QSharedPointer<ComponentInstance> instance,
    QSharedPointer<const Component> component, QString const& activeView) const
{
    QSharedPointer<ConnectivityComponent> newInstance(new ConnectivityComponent(instance->getInstanceName()));
    newInstance->setInstanceUuid(instance->getUuid());
    newInstance->setVlnv(instance->getComponentRef()->toString());
    newInstance->setActiveView(activeView);

    addAddressSpaceMemories(newInstance, component);

    addMemoryMapMemories(newInstance, component);

    return newInstance;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::addAddressSpaceMemories()
//-----------------------------------------------------------------------------
void ConnectivityGraphFactory::addAddressSpaceMemories(QSharedPointer<ConnectivityComponent> newInstance, 
    QSharedPointer<const Component> component) const
{
    QString instanceIdentifier = newInstance->getVlnv().replace(':', '.') + "." + newInstance->getInstanceUuid() +
        "." + newInstance->getName();

    foreach (QSharedPointer<AddressSpace> space, *component->getAddressSpaces())
    {
        QString spaceIdentifier = instanceIdentifier + "." + space->name();
        QSharedPointer<MemoryItem> spaceItem(new MemoryItem(space->name(), "addressSpace"));
        spaceItem->setIdentifier(spaceIdentifier);
        spaceItem->setAUB(space->getAddressUnitBits());
        spaceItem->setAddress("0");
        
        if (!space->getRange().isEmpty())
        {
            spaceItem->setRange(expressionParser_->parseExpression(space->getRange()));
        }
        
        if (!space->getWidth().isEmpty())
        {
            spaceItem->setWidth(expressionParser_->parseExpression(space->getWidth()));
        }        

        newInstance->addMemory(spaceItem);

        foreach(QSharedPointer<Segment> segment, *space->getSegments())
        {
            if (segment->getIsPresent().isEmpty() ||
                expressionParser_->parseExpression(segment->getIsPresent()).toInt() == 1)
            {
                QSharedPointer<MemoryItem> segmentItem(new MemoryItem(segment->name(), "segment"));
                segmentItem->setIdentifier(spaceIdentifier + '.' + segment->name());
                segmentItem->setRange(expressionParser_->parseExpression(segment->getRange()));
                segmentItem->setOffset(expressionParser_->parseExpression(segment->getAddressOffset()));

                spaceItem->addChild(segmentItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::addMemoryMapMemories()
//-----------------------------------------------------------------------------
void ConnectivityGraphFactory::addMemoryMapMemories(QSharedPointer<ConnectivityComponent> instanceData,
    QSharedPointer<const Component> component) const
{
    foreach (QSharedPointer<MemoryMap> map, *component->getMemoryMaps())
    {
        int addressableUnitBits = expressionParser_->parseExpression(map->getAddressUnitBits()).toInt();
        if (addressableUnitBits == 0)
        {
            addressableUnitBits = 8;
        }

        QSharedPointer<MemoryItem> mapItem = createMemoryMapData(map, addressableUnitBits, instanceData);
        instanceData->addMemory(mapItem);

        addMemoryRemapData(map, mapItem, addressableUnitBits, instanceData);
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::createMemoryMapData()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryItem> ConnectivityGraphFactory::createMemoryMapData(QSharedPointer<MemoryMap> map,
    int addressableUnitBits, QSharedPointer<ConnectivityComponent> containingInstance) const
{
    QString mapIdentifier = containingInstance->getVlnv().replace(':', '.') + "." + 
        containingInstance->getInstanceUuid() + "." + containingInstance->getName() + "." + map->name();

    QSharedPointer<MemoryItem> mapItem(new MemoryItem(map->name(), "memoryMap"));
    mapItem->setIdentifier(mapIdentifier);
    mapItem->setAUB(QString::number(addressableUnitBits));

    foreach (QSharedPointer<MemoryBlockBase> block, *map->getMemoryBlocks())
    {
        if (block->getIsPresent().isEmpty() ||
            expressionParser_->parseExpression(block->getIsPresent()).toInt() == 1)
        {
            QSharedPointer<AddressBlock> addressBlock = block.dynamicCast<AddressBlock>();
            QSharedPointer<MemoryItem> blockItem = createMemoryBlock(addressBlock, mapIdentifier, addressableUnitBits);

            mapItem->addChild(blockItem);
        }
    }

    return mapItem;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::createMemoryBlock()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryItem> ConnectivityGraphFactory::createMemoryBlock(QSharedPointer<AddressBlock> addressBlock,
    QString const& mapIdentifier, int addressableUnitBits) const
{
    QString blockIdentifier = mapIdentifier + "." + addressBlock->name();
    int baseAddress = expressionParser_->parseExpression(addressBlock->getBaseAddress()).toInt();

    QSharedPointer<MemoryItem> blockItem(new MemoryItem(addressBlock->name(), "addressBlock"));
    blockItem->setIdentifier(blockIdentifier);
    blockItem->setAUB(QString::number(addressableUnitBits));
    blockItem->setAddress(QString::number(baseAddress));
    blockItem->setRange(expressionParser_->parseExpression(addressBlock->getRange()));
    blockItem->setWidth(expressionParser_->parseExpression(addressBlock->getWidth()));

    foreach (QSharedPointer<RegisterBase> registerBase, *addressBlock->getRegisterData())
    {
        if (registerBase->getIsPresent().isEmpty() || 
            expressionParser_->parseExpression(registerBase->getIsPresent()).toInt() == 1)
        {
            QSharedPointer<Register> reg = registerBase.dynamicCast<Register>();

            addRegisterData(reg, baseAddress, addressableUnitBits, blockIdentifier, blockItem);
        }
    }

    return blockItem;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::addRegisterData()
//-----------------------------------------------------------------------------
void ConnectivityGraphFactory::addRegisterData(QSharedPointer<Register> reg, int baseAddress, 
    int addressableUnitBits, QString const& blockIdentifier, QSharedPointer<MemoryItem> blockItem) const
{
    int registerAddress = baseAddress + expressionParser_->parseExpression(reg->getAddressOffset()).toInt();    
    int registerSize =  expressionParser_->parseExpression(reg->getSize()).toInt();
    int registerDimension = expressionParser_->parseExpression(reg->getDimension()).toInt();

    for (int i = 0; i < registerDimension || (i == 0 && registerDimension == 0); i++)
    {
        QString registerIdentifier = blockIdentifier + "." + reg->name();
        if (!reg->getDimension().isEmpty())
        {
            registerIdentifier.append(QString("[%1]").arg(i));
        }

        QSharedPointer<MemoryItem> regItem(new MemoryItem(reg->name(), "register"));
        regItem->setIdentifier(registerIdentifier);
        regItem->setAUB(QString::number(addressableUnitBits));
        regItem->setAddress(QString::number(registerAddress));
        regItem->setSize(expressionParser_->parseExpression(reg->getSize()));

        foreach (QSharedPointer<Field> field, *reg->getFields())
        {
            if (field->getIsPresent().isEmpty() || 
                expressionParser_->parseExpression(field->getIsPresent()).toInt() == 1)
            {
                regItem->addChild(createField(field, registerIdentifier, registerAddress, addressableUnitBits));
            }
        }

        registerAddress += registerSize / addressableUnitBits;      

        blockItem->addChild(regItem);
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::createField()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryItem> ConnectivityGraphFactory::createField(QSharedPointer<Field> field, 
    QString const& registerIdentifier, int regAddress, int addressableUnitBits) const
{
    QString fieldIdentifier = registerIdentifier + "." + field->name();
    int bitOffset = expressionParser_->parseExpression(field->getBitOffset()).toInt();
   
    QSharedPointer<MemoryItem> fieldItem(new MemoryItem(field->name(), "field"));
    fieldItem->setIdentifier(fieldIdentifier);
    fieldItem->setAUB(QString::number(addressableUnitBits));
    fieldItem->setWidth(expressionParser_->parseExpression(field->getBitWidth()));
    fieldItem->setAddress(QString::number(regAddress + bitOffset/addressableUnitBits));
    fieldItem->setOffset(QString::number(bitOffset % addressableUnitBits));

    return fieldItem;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::addMemoryRemapData()
//-----------------------------------------------------------------------------
void ConnectivityGraphFactory::addMemoryRemapData(QSharedPointer<MemoryMap> map, 
    QSharedPointer<MemoryItem> mapItem, int addressableUnitBits, 
    QSharedPointer<ConnectivityComponent> containingInstance) const
{
    foreach (QSharedPointer<MemoryRemap> remap, *map->getMemoryRemaps())
    {
        QString remapIdentifier =  containingInstance->getVlnv().replace(':', '.') + "." + 
            containingInstance->getInstanceUuid() + "." + containingInstance->getName() + "." + remap->name();

        QSharedPointer<MemoryItem> remapItem(new MemoryItem(remap->name(), "memoryRemap"));
        remapItem->setIdentifier(remapIdentifier);
        remapItem->setAUB(QString::number(addressableUnitBits));

        mapItem->addChild(remapItem);

        foreach (QSharedPointer<MemoryBlockBase> block, *remap->getMemoryBlocks())
        {
            if (block->getIsPresent().isEmpty() ||
                expressionParser_->parseExpression(block->getIsPresent()).toInt() == 1)
            {
                QSharedPointer<AddressBlock> addressBlock = block.dynamicCast<AddressBlock>();

                remapItem->addChild(createMemoryBlock(addressBlock, remapIdentifier, addressableUnitBits));
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::addSubInstances()
//-----------------------------------------------------------------------------
void ConnectivityGraphFactory::addSubInstances(QSharedPointer<ConnectivityComponent> topInstance, 
    QSharedPointer<const Component> topComponent,
    QSharedPointer<const DesignConfiguration> designConfiguration, QSharedPointer<ConnectivityGraph> graph)
{
    QString viewName = "";
    if (designConfiguration)
    {
        viewName = designConfiguration->getActiveView(topInstance->getName());
    }

    if (viewName.isEmpty() && topComponent->getViews()->count() == 1)
    {
        viewName = topComponent->getViews()->first()->name();
    }

    if (!viewName.isEmpty())
    {
        foreach (QSharedPointer<View> componentView, *topComponent->getViews())
        {
            if (componentView->name() == viewName && componentView->isHierarchical())
            {
                QSharedPointer<const DesignConfiguration> hierarchicalConfiguration =
                    getHierarchicalDesignConfiguration(topComponent, componentView);
                QSharedPointer<const Design> hierarchicalDesign = getHierarchicalDesign(topComponent,
                    componentView, hierarchicalConfiguration);

                if (hierarchicalDesign)
                {
                    analyzeDesign(hierarchicalDesign, hierarchicalConfiguration, topInstance->getName(), graph);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::getHierarchicalDesignConfiguration()
//-----------------------------------------------------------------------------
QSharedPointer<const DesignConfiguration> ConnectivityGraphFactory::getHierarchicalDesignConfiguration(
    QSharedPointer<const Component> component, QSharedPointer<View> hierarchicalView) const
{
    QString configurationReference = hierarchicalView->getDesignConfigurationInstantiationRef();
    if (!configurationReference.isEmpty())
    {
        foreach (QSharedPointer<DesignConfigurationInstantiation> instantiation,
            *component->getDesignConfigurationInstantiations())
        {
            if (instantiation->name() == configurationReference)
            {
                QSharedPointer<ConfigurableVLNVReference> configurationVLNV =
                    instantiation->getDesignConfigurationReference();

                QSharedPointer<const Document> configurationDocument =
                    library_->getModelReadOnly(*configurationVLNV);

                return configurationDocument.dynamicCast<const DesignConfiguration>();
            }
        }
    }

    return QSharedPointer<const DesignConfiguration>();
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::getHierarchicalDesign()
//-----------------------------------------------------------------------------
QSharedPointer<const Design> ConnectivityGraphFactory::getHierarchicalDesign(QSharedPointer<const Component> component,
    QSharedPointer<View> hierarchicalView, QSharedPointer<const DesignConfiguration> designConfiguration) const
{
    VLNV designVLNV = getHierarchicalDesignVLNV(component, hierarchicalView);

    if (!designVLNV.isValid())
    {
        designVLNV = designConfiguration->getDesignRef();        
    }

    if (designVLNV.isValid())
    {
        return library_->getModelReadOnly(designVLNV).dynamicCast<const Design>();
    }

    return QSharedPointer<Design>();
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::getHierarchicalDesignVLNV()
//-----------------------------------------------------------------------------
VLNV ConnectivityGraphFactory::getHierarchicalDesignVLNV(QSharedPointer<const Component> component,
    QSharedPointer<View> hierarchicalView) const
{
    QString referencedInstantiation = hierarchicalView->getDesignInstantiationRef();

    if (!referencedInstantiation.isEmpty())
    {
        foreach (QSharedPointer<DesignInstantiation> instantiation, *component->getDesignInstantiations())
        {
            if (instantiation->name() == referencedInstantiation)
            {
                return *instantiation->getDesignReference();
            }
        }
    }

    return VLNV();
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::createInterfaceData()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectivityInterface> ConnectivityGraphFactory::createInterfaceData(QSharedPointer<BusInterface> busInterface,
    QSharedPointer<ConnectivityComponent> instanceNode)
{
    QSharedPointer<ConnectivityInterface> interfaceNode(new ConnectivityInterface(busInterface->name()));
    interfaceNode->setMode(General::interfaceMode2Str(busInterface->getInterfaceMode()));                   
    interfaceNode->setInstance(instanceNode);

    if (busInterface->getInterfaceMode() == General::MASTER)
    {
        interfaceNode->setBaseAddress(expressionParser_->parseExpression(
            busInterface->getMaster()->getBaseAddress()));
    }

    if (busInterface->getInterfaceMode() == General::MIRROREDSLAVE && 
         !busInterface->getMirroredSlave()->getRemapAddresses()->isEmpty())
    {
        interfaceNode->setRemapAddress(expressionParser_->parseExpression(
            busInterface->getMirroredSlave()->getRemapAddresses()->first()->remapAddress_));

        interfaceNode->setRemapRange(expressionParser_->parseExpression(
            busInterface->getMirroredSlave()->getRange()));
    }

    QString memoryReference = busInterface->getAddressSpaceRef();
    if (memoryReference.isEmpty())
    {
        memoryReference = busInterface->getMemoryMapRef();
    }

    if (!memoryReference.isEmpty())
    {
        foreach (QSharedPointer<MemoryItem> memory, instanceNode->getMemories())
        {
            if (memory->getName() == memoryReference)
            {
                interfaceNode->setConnectedMemory(memory);
            }
        }
    }

    return interfaceNode;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::addConnections()
//-----------------------------------------------------------------------------
void ConnectivityGraphFactory::addConnections(QSharedPointer<const Design> design,
    QString const& topInstanceName, QSharedPointer<ConnectivityGraph> graph)
{
    foreach (QSharedPointer<Interconnection> interconnection, *design->getInterconnections())
    {
        QSharedPointer<ActiveInterface> start = interconnection->getStartInterface();

        QSharedPointer<ConnectivityInterface> startInterface = graph->getInterface(start->getComponentReference(),
            start->getBusReference());

        if (!startInterface.isNull())
        {
            foreach (QSharedPointer<HierInterface> hierInterface, *interconnection->getHierInterfaces())
            {
                QSharedPointer<ConnectivityInterface> target = graph->getInterface(topInstanceName,  
                    hierInterface->getBusReference());
                
                if (target)
                {
                    target->setHierarchical();
                    graph->addConnection(interconnection->name(), startInterface, target);
                }
            }

            foreach (QSharedPointer<ActiveInterface> activeInterface, *interconnection->getActiveInterfaces())
            {                
                QSharedPointer<ConnectivityInterface> target = graph->getInterface(activeInterface->getComponentReference(), 
                    activeInterface->getBusReference());

                if (target)
                {
                    graph->addConnection(interconnection->name(), startInterface, target);
                }                 
            }
        }
    }
}
