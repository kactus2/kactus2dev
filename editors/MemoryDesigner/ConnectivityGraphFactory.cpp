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

#include <KactusAPI/include/LibraryInterface.h>

#include "ConnectivityConnection.h"
#include "ConnectivityInterface.h"
#include "ConnectivityComponent.h"
#include "MemoryItem.h"
#include "ConnectivityGraph.h"

#include <KactusAPI/include/IPXactSystemVerilogParser.h>
#include <KactusAPI/include/ParameterCache.h>
#include <KactusAPI/include/ListParameterFinder.h>
#include <KactusAPI/include/MultipleParameterFinder.h>
#include <KactusAPI/include/ComponentInstanceParameterFinder.h>
#include <KactusAPI/include/DesignParameterFinder.h>

#include <editors/MemoryDesigner/MemoryDesignerConstants.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Channel.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/FieldReset.h>
#include <IPXACTmodels/Component/EnumeratedValue.h>
#include <IPXACTmodels/Component/MasterInterface.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryRemap.h>
#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/MirroredSlaveInterface.h>
#include <IPXACTmodels/Component/RegisterBase.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/SlaveInterface.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::ConnectivityGraphFactory()
//-----------------------------------------------------------------------------
ConnectivityGraphFactory::ConnectivityGraphFactory(LibraryInterface* library):
library_(library),
parameterFinder_(new MultipleParameterFinder()), 
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
    QSharedPointer<const Component> topComponent, QString const& activeView)
{
    QSharedPointer<ConnectivityGraph> graph(new ConnectivityGraph());

    if (topComponent)
    {
        parameterFinder_->addFinder(QSharedPointer<ParameterFinder>(new ParameterCache(topComponent)));

        QSharedPointer<ConnectivityComponent> instanceNode =
            createInstanceData(QSharedPointer<ComponentInstance>(0), topComponent, activeView, graph);

        QVector<QSharedPointer<ConnectivityInterface> > instanceInterfaces =
            createInterfacesForInstance(topComponent, instanceNode, graph);

        createConnectionsForDesign(topComponent, activeView, instanceInterfaces, graph);
    }

    return graph;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::analyzeDesign()
//-----------------------------------------------------------------------------
void ConnectivityGraphFactory::analyzeDesign(QSharedPointer<DesignInstantiation> designInstantiation,
    QSharedPointer<const Design> design, QSharedPointer<const DesignConfiguration> designConfiguration,
    QVector<QSharedPointer<ConnectivityInterface>> const& topInterfaces, QSharedPointer<ConnectivityGraph> graph)
    const
{
    QSharedPointer<DesignParameterFinder> designParameterFinder(new DesignParameterFinder(designInstantiation));
    designParameterFinder->setParameterList(design->getParameters());

    parameterFinder_->addFinder(designParameterFinder);

    QVector<QSharedPointer<ConnectivityInterface> > interfacesInDesign;

    for (QSharedPointer<ComponentInstance> componentInstance : *design->getComponentInstances())
    {
        QSharedPointer<Component const> instancedComponent = 
            library_->getModelReadOnly(*componentInstance->getComponentRef()).dynamicCast<Component const>();

        if (instancedComponent)
        {
            QSharedPointer<ComponentInstanceParameterFinder> componentFinder(
                new ComponentInstanceParameterFinder(componentInstance, instancedComponent));

            parameterFinder_->addFinder(componentFinder);

            QString activeView;
            if (designConfiguration)
            {
                activeView = QString::fromStdString(designConfiguration->getActiveView(componentInstance->getInstanceName().toStdString()));
            }

            QSharedPointer<ConnectivityComponent> instanceNode = createInstanceData(componentInstance, 
                instancedComponent, activeView, graph);

            QVector<QSharedPointer<ConnectivityInterface> > instanceInterfaces =
                createInterfacesForInstance(instancedComponent, instanceNode, graph);

            createInteralConnectionsAndDesigns(instancedComponent, instanceNode,
                componentInstance->getInstanceName(), activeView, instanceInterfaces, graph);

            interfacesInDesign += instanceInterfaces;

            parameterFinder_->removeFinder(componentFinder);
        }
    }

    for (QSharedPointer<Interconnection> interconnection : *design->getInterconnections())
    {
        createConnectionsForInterconnection(interconnection, interfacesInDesign, topInterfaces, graph);
    }

    parameterFinder_->removeFinder(designParameterFinder);
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::getInstanceData()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectivityComponent> ConnectivityGraphFactory::createInstanceData(
    QSharedPointer<ComponentInstance> instance, QSharedPointer<const Component> component,
    QString const& activeView, QSharedPointer<ConnectivityGraph> graph) const
{
    QSharedPointer<ConnectivityComponent> newInstance;
    if (instance)
    {
        newInstance =
            QSharedPointer<ConnectivityComponent>(new ConnectivityComponent(instance->getInstanceName()));
        newInstance->setInstanceUuid(instance->getUuid());
        newInstance->setVlnv(instance->getComponentRef()->toString());
    }
    else
    {
        newInstance = QSharedPointer<ConnectivityComponent>(new ConnectivityComponent("top"));
        newInstance->setInstanceUuid("top");
        newInstance->setVlnv(component->getVlnv().toString());
    }

    newInstance->setActiveView(activeView);

    addAddressSpaceMemories(newInstance, component);

    addMemoryMapMemories(newInstance, component);

    graph->getInstances().append(newInstance);
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

    for (QSharedPointer<AddressSpace> space : *component->getAddressSpaces())
    {
        if (space->getIsPresent().isEmpty() ||
            expressionParser_->parseExpression(space->getIsPresent()).toInt() == 1)
        {
            QString spaceAUB = space->getAddressUnitBits();
            QString spaceIdentifier = instanceIdentifier + "." + space->name();

            QSharedPointer<MemoryItem> spaceItem(new MemoryItem(space->name(), "addressSpace"));
            spaceItem->setIdentifier(spaceIdentifier);
            spaceItem->setDisplayName(space->displayName());
            spaceItem->setAUB(spaceAUB);
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

            for (QSharedPointer<Segment> segment : *space->getSegments())
            {
                if (segment->getIsPresent().isEmpty() ||
                    expressionParser_->parseExpression(segment->getIsPresent()).toInt() == 1)
                {
                    QSharedPointer<MemoryItem> segmentItem(new MemoryItem(segment->name(), "segment"));
                    segmentItem->setIdentifier(spaceIdentifier + '.' + segment->name());
                    segmentItem->setDisplayName(segment->displayName());
                    segmentItem->setRange(expressionParser_->parseExpression(segment->getRange()));
                    segmentItem->setOffset(expressionParser_->parseExpression(segment->getAddressOffset()));

                    spaceItem->addChild(segmentItem);
                }
            }

            if (space->hasLocalMemoryMap())
            {
                QSharedPointer<MemoryMapBase> localMap = space->getLocalMemoryMap();
                int aubInInt = expressionParser_->parseExpression(spaceAUB).toInt();

                QSharedPointer<MemoryItem> localMemoryItem = createMemoryMapData(localMap, aubInInt, newInstance);
                spaceItem->addChild(localMemoryItem);
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
    for (QSharedPointer<const MemoryMap> map : *component->getMemoryMaps())
    {
        if (map->getIsPresent().isEmpty() ||
            expressionParser_->parseExpression(map->getIsPresent()).toInt() == 1)
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
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::createMemoryMapData()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryItem> ConnectivityGraphFactory::createMemoryMapData(QSharedPointer<const MemoryMapBase> map,
    int addressableUnitBits, QSharedPointer<ConnectivityComponent> containingInstance) const
{
    QString mapIdentifier = containingInstance->getVlnv().replace(':', '.') + "." + 
        containingInstance->getInstanceUuid() + "." + containingInstance->getName() + "." + map->name();

    QSharedPointer<MemoryItem> mapItem(new MemoryItem(map->name(), "memoryMap"));
    mapItem->setIdentifier(mapIdentifier);
    mapItem->setDisplayName(map->displayName());
    mapItem->setAUB(QString::number(addressableUnitBits));

    General::Usage mapUsage = General::USAGE_COUNT;
    for (QSharedPointer<MemoryBlockBase> block : *map->getMemoryBlocks())
    {
        if (block->getIsPresent().isEmpty() ||
            expressionParser_->parseExpression(block->getIsPresent()).toInt() == 1)
        {
            QSharedPointer<AddressBlock> addressBlock = block.dynamicCast<AddressBlock>();
            if (addressBlock)
            {
                QSharedPointer<MemoryItem> blockItem = createMemoryBlock(addressBlock, mapIdentifier, addressableUnitBits);
                blockItem->setUsage(addressBlock->getUsage());
                mapUsage = addressBlock->getUsage();

                mapItem->addChild(blockItem);
            }
        }
    }

    if (map->getMemoryBlocks()->size() == 1 && mapUsage == General::MEMORY)
    {
        mapItem->setUsage(General::MEMORY);
    }

    return mapItem;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::createMemoryBlock()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryItem> ConnectivityGraphFactory::createMemoryBlock(
    QSharedPointer<const AddressBlock> addressBlock, QString const& mapIdentifier, int addressableUnitBits) const
{
    QString blockIdentifier = mapIdentifier + "." + addressBlock->name();
    int baseAddress = expressionParser_->parseExpression(addressBlock->getBaseAddress()).toInt();

    QSharedPointer<MemoryItem> blockItem(new MemoryItem(addressBlock->name(), "addressBlock"));
    blockItem->setIdentifier(blockIdentifier);
    blockItem->setDisplayName(addressBlock->displayName());
    blockItem->setAUB(QString::number(addressableUnitBits));
    blockItem->setAddress(QString::number(baseAddress));
    blockItem->setRange(expressionParser_->parseExpression(addressBlock->getRange()));
    blockItem->setWidth(expressionParser_->parseExpression(addressBlock->getWidth()));
    blockItem->setUsage(addressBlock->getUsage());

    QString blockPresence = addressBlock->getIsPresent();
    if (blockPresence.isEmpty())
    {
        blockPresence = "1";
    }
    else
    {
        blockPresence = expressionParser_->parseExpression(addressBlock->getIsPresent());
    }

    blockItem->setIsPresent(blockPresence);

    for (QSharedPointer<RegisterBase> registerBase : *addressBlock->getRegisterData())
    {
        if (registerBase->getIsPresent().isEmpty() || 
            expressionParser_->parseExpression(registerBase->getIsPresent()).toInt() == 1)
        {
            QSharedPointer<Register> reg = registerBase.dynamicCast<Register>();
            if (reg)
            {
                addRegisterData(reg, baseAddress, addressableUnitBits, blockIdentifier, blockItem);
            }

            //! What about register files?
        }
    }

    return blockItem;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::addRegisterData()
//-----------------------------------------------------------------------------
void ConnectivityGraphFactory::addRegisterData(QSharedPointer<const Register> reg, int baseAddress, 
    int addressableUnitBits, QString const& blockIdentifier, QSharedPointer<MemoryItem> blockItem) const
{
    quint64 registerOffset = expressionParser_->parseExpression(reg->getAddressOffset()).toInt();
    quint64 registerAddress = baseAddress + registerOffset;
    int registerSize =  expressionParser_->parseExpression(reg->getSize()).toInt();
    int registerDimension = expressionParser_->parseExpression(reg->getDimension()).toInt();

    for (int i = 0; i < registerDimension || (i == 0 && registerDimension == 0); i++)
    {
        QString registerIdentifier = blockIdentifier + "." + reg->name();

        QSharedPointer<MemoryItem> regItem(new MemoryItem(reg->name(), "register"));

        if (!reg->getDimension().isEmpty())
        {
            regItem->setDimension(QString::number(registerDimension));
            registerIdentifier.append(QString("[%1]").arg(i));
        }

        regItem->setIdentifier(registerIdentifier);
        regItem->setDisplayName(reg->displayName());
        regItem->setAUB(QString::number(addressableUnitBits));
        regItem->setAddress(QString::number(registerAddress));
        regItem->setOffset(QString::number(registerOffset));
        regItem->setSize(expressionParser_->parseExpression(reg->getSize()));

        QVector<QSharedPointer<MemoryItem> > fieldItems;

        for (QSharedPointer<Field> field : *reg->getFields())
        {
            if (field->getIsPresent().isEmpty() ||
                expressionParser_->parseExpression(field->getIsPresent()).toInt() == 1)
            {
                fieldItems.append(createField(field, registerIdentifier, registerAddress, addressableUnitBits));
            }
        }

        QMap<quint64, QSharedPointer<MemoryItem> > orderedFieldItems = getOrderedFieldItems(regItem, fieldItems);

        QMapIterator<quint64, QSharedPointer<MemoryItem> > fieldIterator(orderedFieldItems);
        while (fieldIterator.hasNext())
        {
            fieldIterator.next();
            regItem->addChild(fieldIterator.value());
        }

        regItem->addChild(
            createRegisterResetItem(orderedFieldItems, regItem, registerIdentifier, addressableUnitBits));

        registerAddress += registerSize / addressableUnitBits;

        blockItem->addChild(regItem);
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::setOrderedFieldItemsForRegister()
//-----------------------------------------------------------------------------
QMap<quint64, QSharedPointer<MemoryItem> > ConnectivityGraphFactory::getOrderedFieldItems(
    QSharedPointer<MemoryItem> registerItem, QVector<QSharedPointer<MemoryItem> > fieldItems) const
{
    quint64 registerOffset = registerItem->getAddress().toULongLong();
    QMap<quint64, QSharedPointer<MemoryItem> > orderedFieldItems;

    for (auto fieldItem : fieldItems)
    {
        quint64 fieldAddress = fieldItem->getAddress().toInt();
        quint64 fieldStart = fieldItem->getOffset().toInt();

        int aub = fieldItem->getAUB().toInt();

        fieldStart = (fieldAddress - registerOffset) * aub + fieldStart;

        orderedFieldItems.insert(fieldStart, fieldItem);
    }

    return orderedFieldItems;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::createField()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryItem> ConnectivityGraphFactory::createField(QSharedPointer<const Field> field, 
    QString const& registerIdentifier, int regAddress, int addressableUnitBits) const
{
    QString fieldIdentifier = registerIdentifier + "." + field->name();
    int bitOffset = expressionParser_->parseExpression(field->getBitOffset()).toInt();

    QSharedPointer<MemoryItem> fieldItem(new MemoryItem(field->name(), "field"));
    fieldItem->setIdentifier(fieldIdentifier);
    fieldItem->setDisplayName(field->displayName());
    fieldItem->setAUB(QString::number(addressableUnitBits));
    fieldItem->setWidth(expressionParser_->parseExpression(field->getBitWidth()));
    fieldItem->setAddress(QString::number(regAddress + bitOffset/addressableUnitBits));
    fieldItem->setOffset(QString::number(bitOffset));

    for (auto enumeratedValue : *field->getEnumeratedValues())
    {
        fieldItem->addChild(createEnumeratedValueItem(enumeratedValue, fieldIdentifier, addressableUnitBits));
    }

    for (auto reset : *field->getResets())
    {
        fieldItem->addChild(createFieldResetItem(reset, fieldIdentifier, addressableUnitBits, fieldItem->getWidth()));
    }

    return fieldItem;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::createEnumeratedValueItem()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryItem> ConnectivityGraphFactory::createEnumeratedValueItem(
    QSharedPointer<const EnumeratedValue> enumeratedValue, QString const& fieldIdentifier,
    int const& addressUnitBits) const
{
    QString enumName = enumeratedValue->name();
    QString enumeratedValueIdentifier = fieldIdentifier + enumName;

    QSharedPointer<MemoryItem> enumItem(new MemoryItem(enumName, MemoryDesignerConstants::ENUMERATED_VALUE_TYPE));
    enumItem->setIdentifier(enumeratedValueIdentifier);
    enumItem->setDisplayName(enumeratedValue->displayName());
    enumItem->setAUB(QString::number(addressUnitBits));
    enumItem->setValue(expressionParser_->parseExpression(enumeratedValue->getValue()));

    return enumItem;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::createFieldResetItem()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryItem> ConnectivityGraphFactory::createFieldResetItem(QSharedPointer<FieldReset> fieldReset,
    QString const& fieldIdentifier, int const& addressUnitBits, QString const& fieldWidth) const
{
    QString resetType = fieldReset->getResetTypeReference();
    if (resetType.isEmpty())
    {
        resetType = MemoryDesignerConstants::HARD_RESET_TYPE;
    }

    QString resetIdentifier = fieldIdentifier + resetType;

    QSharedPointer<MemoryItem> resetItem(new MemoryItem(resetType, MemoryDesignerConstants::RESET_TYPE));
    resetItem->setIdentifier(resetIdentifier);
    resetItem->setAUB(QString::number(addressUnitBits));

    QString resetValue = expressionParser_->parseExpression(fieldReset->getResetValue());
    qulonglong decimalResetValue = resetValue.toULongLong();
    resetValue.setNum(decimalResetValue, 2);

    QString resetMask;
    if (fieldReset->getResetMask().isEmpty())
    {
        resetMask = QString("1").repeated(fieldWidth.toInt());
    } 
    else
    {
        resetMask = expressionParser_->parseExpression(fieldReset->getResetMask());
        qulonglong decimalResetMask = resetMask.toULongLong();
        resetMask.setNum(decimalResetMask, 2);
    } 

    resetItem->setResetValue(resetValue);
    resetItem->setResetMask(resetMask);

    return resetItem;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::createRegisterResetItem()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryItem> ConnectivityGraphFactory::createRegisterResetItem(
    QMap<quint64, QSharedPointer<MemoryItem>> fieldItems, QSharedPointer<MemoryItem> registerItem,
    QString const& registerIdentifier, int const& addressUnitBits) const
{
    QString resetIdentifier = registerIdentifier + QStringLiteral(".") + MemoryDesignerConstants::HARD_RESET_TYPE;

    QString resetValue = "";
    QString resetMask = "";

    QSharedPointer<MemoryItem> resetItem(
        new MemoryItem(MemoryDesignerConstants::HARD_RESET_TYPE, MemoryDesignerConstants::RESET_TYPE));
    resetItem->setIdentifier(resetIdentifier);
    resetItem->setAUB(QString::number(addressUnitBits));

    QMapIterator<quint64, QSharedPointer<MemoryItem> > fieldIterator(fieldItems);
    quint64 currentEnd = 0;
    while (fieldIterator.hasNext())
    {
        fieldIterator.next();
        quint64 fieldStart = fieldIterator.key();
        quint64 fieldEnd = 0;

        QSharedPointer<MemoryItem> fieldItem = fieldIterator.value();
        quint64 fieldWidth = fieldItem->getWidth().toInt();

        if (fieldWidth > 0)
        {
            fieldEnd = fieldStart + fieldWidth - 1;
        }

        if (currentEnd < fieldStart)
        {
            for (int i = currentEnd; i < fieldStart; ++i )
            {
                resetValue.prepend("0");
                resetMask.prepend("0");
            }
        }

        QSharedPointer<MemoryItem> fieldResetItem = getHardResetItem(fieldItem);
        if (fieldResetItem)
        {
            QString fieldResetValue = fieldResetItem->getResetValue();
            QString fieldResetMask = fieldResetItem->getResetMask();

            fieldResetValue = fieldResetValue.rightJustified(fieldWidth, '0', true);
            fieldResetMask = fieldResetMask.rightJustified(fieldWidth, '0', true);

            resetValue.prepend(fieldResetValue);
            resetMask.prepend(fieldResetMask);
        }
        else
        {
            for (int i = 0; i < fieldWidth; ++i)
            {
                resetValue.prepend("0");
                resetMask.prepend("0");
            }
        }
        currentEnd = fieldEnd + 1;
    }

    quint64 registerSize = registerItem->getSize().toULongLong();
    resetValue = resetValue.rightJustified(registerSize, '0', true);
    resetMask = resetMask.rightJustified(registerSize, '0', true);

    resetItem->setResetValue(resetValue);
    resetItem->setResetMask(resetMask);

    return resetItem;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::getHardResetItem()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryItem> ConnectivityGraphFactory::getHardResetItem(QSharedPointer<MemoryItem> containingItem)
    const
{
    for (auto resetItem : containingItem->getChildItems())
    {
        if (resetItem->getType() == MemoryDesignerConstants::RESET_TYPE &&
            resetItem->getName() == MemoryDesignerConstants::HARD_RESET_TYPE)
        {
            return resetItem;
        }
    }

    return QSharedPointer<MemoryItem>();
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::addMemoryRemapData()
//-----------------------------------------------------------------------------
void ConnectivityGraphFactory::addMemoryRemapData(QSharedPointer<const MemoryMap> map,
    QSharedPointer<MemoryItem> mapItem, int addressableUnitBits,
    QSharedPointer<ConnectivityComponent> containingInstance) const
{
    QString remapPrefix =  containingInstance->getVlnv().replace(':', '.') + "." +
        containingInstance->getInstanceUuid() + "." + containingInstance->getName() + ".";

    for (QSharedPointer<MemoryRemap> remap : *map->getMemoryRemaps())
    {
        QString remapIdentifier = remapPrefix + remap->name();

        QSharedPointer<MemoryItem> remapItem(new MemoryItem(remap->name(), "memoryRemap"));
        remapItem->setDisplayName(remap->displayName());
        remapItem->setIdentifier(remapIdentifier);
        remapItem->setAUB(QString::number(addressableUnitBits));

        mapItem->addChild(remapItem);

        for (QSharedPointer<MemoryBlockBase> block : *remap->getMemoryBlocks())
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
// Function: ConnectivityGraphFactory::createInterfacesForInstance()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<ConnectivityInterface> > ConnectivityGraphFactory::createInterfacesForInstance(
    QSharedPointer<Component const> instancedComponent, QSharedPointer<ConnectivityComponent> instanceNode,
    QSharedPointer<ConnectivityGraph> graph) const
{
    QVector<QSharedPointer<ConnectivityInterface> > instanceInterfaces;

    for (QSharedPointer<const BusInterface> busInterface : *instancedComponent->getBusInterfaces())
    {
        QSharedPointer<ConnectivityInterface> node = createInterfaceData(busInterface, instanceNode, graph);
        instanceInterfaces.append(node);
    }

    return instanceInterfaces;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::createInterfaceData()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectivityInterface> ConnectivityGraphFactory::createInterfaceData(
    QSharedPointer<const BusInterface> busInterface, QSharedPointer<ConnectivityComponent> instanceNode,
    QSharedPointer<ConnectivityGraph> graph) const
{
    QSharedPointer<ConnectivityInterface> interfaceNode(new ConnectivityInterface(busInterface->name()));
    interfaceNode->setMode(General::interfaceMode2Str(busInterface->getInterfaceMode()));                   
    interfaceNode->setInstance(instanceNode);

    QString memoryReference;

    if (busInterface->getInterfaceMode() == General::MASTER)
    {
        interfaceNode->setBaseAddress(
            expressionParser_->parseExpression(busInterface->getMaster()->getBaseAddress()));
        memoryReference = busInterface->getAddressSpaceRef();
    }

    if (busInterface->getInterfaceMode() == General::SLAVE)
    {
        memoryReference = busInterface->getMemoryMapRef();
    }

    if (busInterface->getInterfaceMode() == General::MIRROREDSLAVE && 
        !busInterface->getMirroredSlave()->getRemapAddresses()->isEmpty())
    {
        interfaceNode->setRemapAddress(expressionParser_->parseExpression(
            busInterface->getMirroredSlave()->getRemapAddresses()->first()->remapAddress_));

        interfaceNode->setRemapRange(
            expressionParser_->parseExpression(busInterface->getMirroredSlave()->getRange()));
    }

    if (!memoryReference.isEmpty())
    {
        QVector<QSharedPointer<MemoryItem> > instanceMemories = instanceNode->getMemories();
        for (int i = 0; i < instanceMemories.size(); ++i)
        {
            QSharedPointer<MemoryItem> memory = instanceMemories.at(i);
            if (memory->getName().compare(memoryReference) == 0)
            {
                interfaceNode->setConnectedMemory(memory);
            }
        }
    }

    graph->getInterfaces().append(interfaceNode);
    return interfaceNode;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::createInteralConnectionsAndDesigns()
//-----------------------------------------------------------------------------
void ConnectivityGraphFactory::createInteralConnectionsAndDesigns(
    QSharedPointer<const Component> instancedComponent, QSharedPointer<ConnectivityComponent> instanceNode,
    QString const& instanceName, QString const& activeView,
    QVector<QSharedPointer<ConnectivityInterface> > instanceInterfaces, QSharedPointer<ConnectivityGraph> graph)
    const
{
    if (!instancedComponent->getChannels()->isEmpty())
    {
        instanceNode->setChanneled();
    }

    for (QSharedPointer<Channel> channel : *instancedComponent->getChannels())
    {
        createInternalConnectionsForChannel(channel, instanceName, instanceInterfaces, graph);
    }

    for (QSharedPointer<BusInterface> busInterface : *instancedComponent->getBusInterfaces())
    {
        if (busInterface->hasBridge())
        {
            createInternalConnectionsForBridge(busInterface, instanceName, instanceInterfaces, graph);
        }
    }
    
    for (QSharedPointer<AddressSpace> space : *instancedComponent->getAddressSpaces())
    {
        if (space->hasLocalMemoryMap())
        {
            createInternalSpaceMapConnection(
                instancedComponent, instanceNode, instanceName, space, instanceInterfaces, graph);
        }
    }

    createConnectionsForDesign(instancedComponent, activeView, instanceInterfaces, graph);
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::createInternalSpaceMapConnection()
//-----------------------------------------------------------------------------
void ConnectivityGraphFactory::createInternalSpaceMapConnection(QSharedPointer<const Component> instancedComponent,
    QSharedPointer<ConnectivityComponent> instanceNode, QString const& instanceName,
    QSharedPointer<AddressSpace> addressSpace, QVector<QSharedPointer<ConnectivityInterface> > instanceInterfaces,
    QSharedPointer<ConnectivityGraph> graph) const
{
    QSharedPointer<BusInterface> containingBus =
        getBusInterfaceReferencingAddressSpace(instancedComponent, addressSpace->name());
    QSharedPointer<ConnectivityInterface> localConnectionInterface;

    if (containingBus)
    {
        localConnectionInterface = getInterface(containingBus->name(), instanceName, instanceInterfaces);
    }
    else
    {
        QSharedPointer<MemoryItem> interfacedMemory = getMemoryItemNode(addressSpace->name(), instanceNode);
        if (interfacedMemory)
        {
            localConnectionInterface =
                createLocalInterfaceData(addressSpace, interfacedMemory, instanceNode, graph);
        }
    }

    QString connectionName =
        addressSpace->name() + "_to_local_memory_map_" + addressSpace->getLocalMemoryMap()->name();

    createConnectionData(connectionName, localConnectionInterface, localConnectionInterface, graph);
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::createLocalInterfaceData()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectivityInterface> ConnectivityGraphFactory::createLocalInterfaceData(
    QSharedPointer<AddressSpace> addressSpace, QSharedPointer<MemoryItem> interfacedMemory,
    QSharedPointer<ConnectivityComponent> instanceNode, QSharedPointer<ConnectivityGraph> graph) const
{
    QSharedPointer<ConnectivityInterface> localConnectionInterface(
        new ConnectivityInterface(addressSpace->name() + QStringLiteral(" interface")));

    localConnectionInterface->setConnectedMemory(interfacedMemory);
    localConnectionInterface->setInstance(instanceNode);

    graph->getInterfaces().append(localConnectionInterface);

    return localConnectionInterface;
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::getBusInterfaceReferencingAddressSpace()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> ConnectivityGraphFactory::getBusInterfaceReferencingAddressSpace(
    QSharedPointer<const Component> instancedComponent, QString const& spaceName) const
{
    for (QSharedPointer<BusInterface> busInterface : *instancedComponent->getBusInterfaces())
    {
        if (busInterface->getAddressSpaceRef().compare(spaceName) == 0)
        {
            return busInterface;
        }
    }

    return QSharedPointer<BusInterface>();
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::getMemoryItemNode()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryItem> ConnectivityGraphFactory::getMemoryItemNode(QString const& memoryReference,
    QSharedPointer<ConnectivityComponent> instanceNode) const
{
    QVector<QSharedPointer<MemoryItem> > instanceMemories = instanceNode->getMemories();
    for (int i = 0; i < instanceMemories.size(); ++i)
    {
        QSharedPointer<MemoryItem> memory = instanceMemories.at(i);
        if (memory->getName().compare(memoryReference) == 0)
        {
            return memory;
        }
    }

    return QSharedPointer<MemoryItem>();
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::createConnectionsForDesign()
//-----------------------------------------------------------------------------
void ConnectivityGraphFactory::createConnectionsForDesign(QSharedPointer<const Component> instancedComponent,
    QString const& activeView, QVector<QSharedPointer<ConnectivityInterface> > instanceInterfaces,
    QSharedPointer<ConnectivityGraph> graph) const
{
    QSharedPointer<View> activeComponentView = findView(instancedComponent, activeView);

    if (activeComponentView && activeComponentView->isHierarchical())
    {
        QSharedPointer<const DesignConfiguration> hierarchicalConfiguration =
            getHierarchicalDesignConfiguration(instancedComponent, activeComponentView);

        QSharedPointer<const Design> hierarchicalDesign = getHierarchicalDesign(instancedComponent,
            activeComponentView, hierarchicalConfiguration);

        if (hierarchicalDesign)
        {              
            for (QSharedPointer<ConnectivityInterface> topInterface : instanceInterfaces)
            {
                topInterface->setHierarchical();
            }

            QSharedPointer<DesignInstantiation> designInstantiation =
                getDesignInstantiation(instancedComponent, activeComponentView);
            analyzeDesign(designInstantiation, hierarchicalDesign, hierarchicalConfiguration, instanceInterfaces,
                graph);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::getDesignInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<DesignInstantiation> ConnectivityGraphFactory::getDesignInstantiation(
    QSharedPointer<const Component> instancedComponent, QSharedPointer<View> activeView) const
{
    QString designInstantiationReference = activeView->getDesignInstantiationRef();
    for (auto instantiation : *instancedComponent->getDesignInstantiations())
    {
        if (instantiation->name() == designInstantiationReference)
        {
            return instantiation;
        }
    }

    return QSharedPointer<DesignInstantiation>();
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::createConnectionsForInterconnection()
//-----------------------------------------------------------------------------
void ConnectivityGraphFactory::createConnectionsForInterconnection(
    QSharedPointer<const Interconnection> interconnection,
    QVector<QSharedPointer<ConnectivityInterface> > designInterfaces,
    QVector<QSharedPointer<ConnectivityInterface> > const& topInterfaces,
    QSharedPointer<ConnectivityGraph> graph) const
{
    QSharedPointer<ActiveInterface> start = interconnection->getStartInterface();

    QSharedPointer<ConnectivityInterface> startInterface =
        getInterface(start->getBusReference(), start->getComponentReference(), designInterfaces);

    for (QSharedPointer<HierInterface> hierInterface : *interconnection->getHierInterfaces())
    {
        QSharedPointer<ConnectivityInterface> target = getTopInterface(hierInterface->getBusReference(), topInterfaces);

        createConnectionData(interconnection->name(), startInterface, target, graph);
    }

    for (QSharedPointer<ActiveInterface> activeInterface : *interconnection->getActiveInterfaces())
    {                
        QSharedPointer<ConnectivityInterface> target = getInterface(activeInterface->getBusReference(),
            activeInterface->getComponentReference(), designInterfaces);

        createConnectionData(interconnection->name(), startInterface, target, graph);
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::getInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectivityInterface> ConnectivityGraphFactory::getInterface(QString const& interfaceName,
    QString const& instanceName, QVector<QSharedPointer<ConnectivityInterface> > const& instanceInterfaces) const
{
    for (int i = 0; i < instanceInterfaces.size(); ++i)
    {
        QSharedPointer<ConnectivityInterface> currentInterface = instanceInterfaces[i];
        if (currentInterface->getName().compare(interfaceName) == 0 &&
            currentInterface->getInstance()->getName().compare(instanceName) == 0)
        {
            return currentInterface;
        }
    }

    return QSharedPointer<ConnectivityInterface>();
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::getTopInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectivityInterface> ConnectivityGraphFactory::getTopInterface(QString const& interfaceName,
    QVector<QSharedPointer<ConnectivityInterface> > const& topInterfaces) const
{
    for (int i = 0; i < topInterfaces.size(); ++i)
    {
        QSharedPointer<ConnectivityInterface> interface = topInterfaces[i];
        if (interface->getName().compare(interfaceName) == 0)
        {
            return interface;
        }
    }

    return QSharedPointer<ConnectivityInterface>();
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::findView()
//-----------------------------------------------------------------------------
QSharedPointer<View> ConnectivityGraphFactory::findView(QSharedPointer<const Component> instancedComponent,
    QString const& viewName) const
{
    if (viewName.isEmpty() && instancedComponent->getViews()->count() == 1)
    {
        return instancedComponent->getViews()->first();
    }

    for (QSharedPointer<View> componentView : *instancedComponent->getViews())
    {
        if (componentView->name().compare(viewName) == 0)
        {
            return componentView;
        }
    }

    return QSharedPointer<View>();
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::createInternalConnectionsForChannel()
//-----------------------------------------------------------------------------
void ConnectivityGraphFactory::createInternalConnectionsForChannel(QSharedPointer<const Channel> channel, 
    QString const& instanceName, QVector<QSharedPointer<ConnectivityInterface> > instanceInterfaces,
    QSharedPointer<ConnectivityGraph> graph) const
{
    QStringList channelInterfaces = channel->getInterfaces();

    while (!channelInterfaces.isEmpty())
    {
        QString startInterfaceName = channelInterfaces.takeFirst();
        QSharedPointer<ConnectivityInterface> startInterface =
            getInterface(startInterfaceName, instanceName, instanceInterfaces);

        if (startInterface)
        {
            for (QString const& targetName : channelInterfaces)
            {
                QSharedPointer<ConnectivityInterface> target =
                    getInterface(targetName, instanceName, instanceInterfaces);

                if (target)
                {
                    createConnectionData(channel->name(), startInterface, target, graph);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::createInternalConnectionsForBridge()
//-----------------------------------------------------------------------------
void ConnectivityGraphFactory::createInternalConnectionsForBridge(QSharedPointer<const BusInterface> busInterface,
    QString const& instanceName, QVector<QSharedPointer<ConnectivityInterface> > const& instanceInterfaces,
    QSharedPointer<ConnectivityGraph> graph) const
{
    QSharedPointer<ConnectivityInterface> startInterface =
        getInterface(busInterface->name(), instanceName, instanceInterfaces);
    startInterface->setBridged();

    for (QSharedPointer<TransparentBridge> bridge : *busInterface->getSlave()->getBridges())
    {
        QSharedPointer<ConnectivityInterface> endInterface =
            getInterface(bridge->getMasterRef(), instanceName, instanceInterfaces);

        if (endInterface)
        {
            endInterface->setBridged();

            QString bridgeName = startInterface->getName() + "_bridge_to_" + endInterface->getName();
            createConnectionData(bridgeName, startInterface, endInterface, graph);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::getHierarchicalDesignConfiguration()
//-----------------------------------------------------------------------------
QSharedPointer<const DesignConfiguration> ConnectivityGraphFactory::getHierarchicalDesignConfiguration(
    QSharedPointer<const Component> component, QSharedPointer<const View> hierarchicalView) const
{
    QString configurationReference = hierarchicalView->getDesignConfigurationInstantiationRef();
    if (!configurationReference.isEmpty())
    {
        for (QSharedPointer<DesignConfigurationInstantiation> instantiation :
            *component->getDesignConfigurationInstantiations())
        {
            if (instantiation->name().compare(configurationReference) == 0)
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
    QSharedPointer<const View> hierarchicalView, QSharedPointer<const DesignConfiguration> designConfiguration) const
{
    VLNV designVLNV = getHierarchicalDesignVLNV(component, hierarchicalView);

    if (!designVLNV.isValid() && !designConfiguration.isNull())
    {
        designVLNV = designConfiguration->getDesignRef();        
    }

    if (designVLNV.isValid())
    {
        return library_->getModelReadOnly(designVLNV).dynamicCast<const Design>();
    }

    return QSharedPointer<const Design>();
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::getHierarchicalDesignVLNV()
//-----------------------------------------------------------------------------
VLNV ConnectivityGraphFactory::getHierarchicalDesignVLNV(QSharedPointer<const Component> component,
    QSharedPointer<const View> hierarchicalView) const
{
    QString referencedInstantiation = hierarchicalView->getDesignInstantiationRef();

    if (!referencedInstantiation.isEmpty())
    {
        for (QSharedPointer<DesignInstantiation> instantiation : *component->getDesignInstantiations())
        {
            if (instantiation->name().compare(referencedInstantiation) == 0)
            {
                return *instantiation->getDesignReference();
            }
        }
    }

    return VLNV();
}

//-----------------------------------------------------------------------------
// Function: ConnectivityGraphFactory::createConnectionData()
//-----------------------------------------------------------------------------
void ConnectivityGraphFactory::createConnectionData(QString const& connectionName,
    QSharedPointer<ConnectivityInterface> startPoint, QSharedPointer<ConnectivityInterface> endPoint,
    QSharedPointer<ConnectivityGraph> graph) const
{
    if (startPoint != nullptr && endPoint != nullptr)
    {
        QSharedPointer<ConnectivityConnection> connection(
            new ConnectivityConnection(connectionName, startPoint, endPoint));

        graph->getConnections().append(connection);
    }
}
