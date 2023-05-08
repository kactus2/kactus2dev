//-----------------------------------------------------------------------------
// File: RenodeUtilities.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.03.2023
//
// Description:
// Utility functions for Renode generator.
//-----------------------------------------------------------------------------

#include "RenodeUtilities.h"

#include <Plugins/common/ConnectivityGraphUtilities.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeStructs.h>

#include <editors/MemoryDesigner/MemoryItem.h>
#include <editors/MemoryDesigner/MemoryDesignerConstants.h>
#include <editors/MemoryDesigner/MemoryConnectionAddressCalculator.h>

#include <QJsonArray>

namespace
{
    //-----------------------------------------------------------------------------
    // Function: valueToHexa()
    //-----------------------------------------------------------------------------
    QString valueToHexa(quint64 const& value)
    {
        QString valueInHexa = QString::number(value, 16).toUpper();

        int valueNumbers =
            MemoryDesignerConstants::getAmountOfNumbersInRange(valueInHexa, valueInHexa);

        valueInHexa = "0x" +
            MemoryDesignerConstants::getValueWithZerosAdded(valueInHexa, valueNumbers);

        return valueInHexa;
    }

    //-----------------------------------------------------------------------------
    // Function: SVDGenerator::getBlockMemoryItem()
    //-----------------------------------------------------------------------------
    QVector<QSharedPointer<MemoryItem>> getSubMemoryItems(QSharedPointer<MemoryItem> memoryItem, QString const& subItemType)
    {
        QVector<QSharedPointer<MemoryItem>> subItems;

        for (auto subItem : memoryItem->getChildItems())
        {
            if (subItem->getType().compare(subItemType, Qt::CaseInsensitive) == 0)
            {
                subItems.append(subItem);
            }
        }

        return subItems;
    }

    //-----------------------------------------------------------------------------
    // Function: SVDGenerator::getAddressBlockItems()
    //-----------------------------------------------------------------------------
    QMultiMap<quint64, QSharedPointer<MemoryItem> > getAddressBlockItems(QSharedPointer<MemoryItem> mapItem)
    {
        QMultiMap<quint64, QSharedPointer<MemoryItem>> blockItems;

        for (auto subItem : getSubMemoryItems(mapItem, MemoryDesignerConstants::ADDRESSBLOCK_TYPE))
        {
            blockItems.insertMulti(subItem->getOffset().toULongLong(), mapItem);
        }

        return blockItems;
    }

    //-----------------------------------------------------------------------------
    // Function: RenodeUtilities::getOrderedChildItems()
    //-----------------------------------------------------------------------------
    QMultiMap<quint64, QSharedPointer<MemoryItem> > getOrderedChildItems(QSharedPointer<MemoryItem> mapItem)
    {
        QMultiMap<quint64, QSharedPointer<MemoryItem>> blockItems;
        for (auto subItem : mapItem->getChildItems())
        {
            blockItems.insertMulti(subItem->getAddress().toULongLong(), subItem);
        }

        return blockItems;
    }

    //-----------------------------------------------------------------------------
    // Function: CommonInterface::nameIsUnique()
    //-----------------------------------------------------------------------------
    bool nameIsUnique(QString const& mapName, QStringList const& peripheralNames)
    {
        for (auto containedName : peripheralNames)
        {
            if (containedName == mapName)
            {
                return false;
            }
        }

        return true;
    }

    //-----------------------------------------------------------------------------
    // Function: RenodeUtilities::getUniqueName()
    //-----------------------------------------------------------------------------
    QString getUniqueName(QString const& referenceName, QStringList const& peripheralNames)
    {
        QString newName(referenceName);

        QString format(QLatin1String("$itemName$_$itemNumber$"));
        int runningNumber = 0;
        while (!nameIsUnique(newName, peripheralNames))
        {
            newName = format;
            newName.replace("$itemName$", referenceName);
            newName.replace("$itemNumber$", QString::number(runningNumber));

            runningNumber++;
        }

        return newName;
    }

    //-----------------------------------------------------------------------------
    // Function: RenodeUtilities::createPeripheral()
    //-----------------------------------------------------------------------------
    QSharedPointer<RenodeStructs::cpuPeripherals> createPeripheral(QSharedPointer<MemoryItem> mapItem,
        quint64 mapBaseAddress, QVector<QSharedPointer<MemoryItem> > currentMemoryBlock, QStringList& peripheralNames)
    {
        QSharedPointer<MemoryItem> firstBlock = currentMemoryBlock.first();
        QSharedPointer<MemoryItem> lastBlock = currentMemoryBlock.last();

        quint64 offset = mapBaseAddress + firstBlock->getAddress().toULongLong();
        qint64 endAddress = mapBaseAddress + lastBlock->getAddress().toLongLong() + lastBlock->getRange().toLongLong() - 1;

        quint64 size = endAddress - offset + 1;

        QSharedPointer<RenodeStructs::cpuPeripherals> newPeripheral(new RenodeStructs::cpuPeripherals());
        QString newPeripheralName = getUniqueName(mapItem->getName(), peripheralNames);
        newPeripheral->peripheralName_ = newPeripheralName;
        newPeripheral->peripheralID_ = "";
        newPeripheral->baseAddress_ = valueToHexa(offset);
        newPeripheral->size_ = valueToHexa(size);

        peripheralNames.append(newPeripheralName);

        return newPeripheral;
    }

//-----------------------------------------------------------------------------
// Function: RenodeUtilities::createMemory()
//-----------------------------------------------------------------------------
    QSharedPointer<RenodeStructs::cpuMemories> createMemory(QSharedPointer<MemoryItem> mapItem, quint64 mapBaseAddress,
        QVector<QSharedPointer<MemoryItem> > currentMemoryBlock, QStringList& memoryNames)
    {
        QSharedPointer<MemoryItem> firstBlock = currentMemoryBlock.first();
        QSharedPointer<MemoryItem> lastBlock = currentMemoryBlock.last();

        quint64 offset = mapBaseAddress + firstBlock->getAddress().toULongLong();
        qint64 endAddress = mapBaseAddress + lastBlock->getAddress().toLongLong() + lastBlock->getRange().toLongLong() - 1;

        quint64 size = endAddress - offset + 1;

        QSharedPointer<RenodeStructs::cpuMemories> newMemory(new RenodeStructs::cpuMemories());
        QString newMemoryName = getUniqueName(mapItem->getName(), memoryNames);

        newMemory->memoryName_ = newMemoryName;
        newMemory->memoryID_ = "";
        newMemory->baseAddress_ = valueToHexa(offset);
        newMemory->size_ = valueToHexa(size);

        memoryNames.append(newMemoryName);

        return newMemory;
    }

    //-----------------------------------------------------------------------------
    // Function: RenodeUtilities::createAndAddPeripheralOrMemory()
    //-----------------------------------------------------------------------------
    void createItemBlock(General::Usage blockUsage, QVector<QSharedPointer<MemoryItem> >& currentMemoryBlock,
        QVector<QSharedPointer<RenodeStructs::cpuPeripherals> >& peripherals, QVector<QSharedPointer<RenodeStructs::cpuMemories> >& memories,
        QSharedPointer<MemoryItem> interfaceMemory, quint64 const& memoryBaseAddress, QStringList& peripheralNames, QStringList& memoryNames)
    {
        if (!currentMemoryBlock.isEmpty())
        {
            if (blockUsage == General::MEMORY)
            {
                memories.append(createMemory(interfaceMemory, memoryBaseAddress, currentMemoryBlock, memoryNames));
            }
            else
            {
                peripherals.append(createPeripheral(interfaceMemory, memoryBaseAddress, currentMemoryBlock, peripheralNames));
            }
        }
    }

    //-----------------------------------------------------------------------------
    // Function: RenodeUtilities::usageIsMatching()
    //-----------------------------------------------------------------------------
    bool usageIsMatching(General::Usage currentUsage, General::Usage previousUsage)
    {
        return (currentUsage == General::MEMORY && currentUsage == previousUsage) || (currentUsage != General::MEMORY && previousUsage != General::MEMORY);
    }

    //-----------------------------------------------------------------------------
    // Function: RenodeUtilities::getFirstBlockUsage()
    //-----------------------------------------------------------------------------
    General::Usage getFirstBlockUsage(QMultiMap<quint64, QSharedPointer<MemoryItem> > orderedChildItems)
    {
        QMultiMapIterator<quint64, QSharedPointer<MemoryItem> > blockIterator(orderedChildItems);
        while (blockIterator.hasNext())
        {
            blockIterator.next();
            QSharedPointer<MemoryItem> blockItem = blockIterator.value();

            if (blockItem->getType().compare(MemoryDesignerConstants::ADDRESSBLOCK_TYPE, Qt::CaseInsensitive) == 0)
            {
                return blockItem->getUsage();
            }
        }

        return General::USAGE_COUNT;
    }

    //-----------------------------------------------------------------------------
    // Function: RenodeUtilities::findPeripheral()
    //-----------------------------------------------------------------------------
    QSharedPointer<RenodeStructs::cpuPeripherals> findPeripheral(QString const& peripheralName, QVector<QSharedPointer<RenodeStructs::cpuPeripherals> > const& peripherals)
    {
        for (auto currentPeripheral : peripherals)
        {
            if (currentPeripheral->peripheralName_ == peripheralName)
            {
                return currentPeripheral;
            }
        }

        return QSharedPointer<RenodeStructs::cpuPeripherals>();
    }

    //-----------------------------------------------------------------------------
    // Function: RenodeUtilities::setupPeripheralConfiguration()
    //-----------------------------------------------------------------------------
    void setupPeripheralConfiguration(QJsonObject const& configurationObject, QVector<QSharedPointer<RenodeStructs::cpuPeripherals> >& peripherals)
    {
        QJsonValue peripheralsValue = configurationObject.value(RenodeConstants::PERIPHERALS);
        if (peripheralsValue.isArray())
        {
            QJsonArray peripheralsArray = peripheralsValue.toArray();

            for (int arrayIndex = 0; arrayIndex < peripheralsArray.count(); arrayIndex++)
            {
                QJsonValue singlePeripheralValue = peripheralsArray.at(arrayIndex);
                if (singlePeripheralValue.isObject())
                {
                    QJsonObject peripheralObject = singlePeripheralValue.toObject();
                    QString peripheralName = peripheralObject.value(RenodeConstants::PERIPHERALNAME).toString("");

                    QSharedPointer<RenodeStructs::cpuPeripherals> matchingPeripheral = findPeripheral(peripheralName, peripherals);
                    if (matchingPeripheral)
                    {
                        matchingPeripheral->className_ = peripheralObject.value(RenodeConstants::PERIPHERALCLASS).toString(matchingPeripheral->className_);
                        matchingPeripheral->filePath_ = peripheralObject.value(RenodeConstants::PERIPHERALPATH).toString(matchingPeripheral->filePath_);
                        matchingPeripheral->initable_ = peripheralObject.value(RenodeConstants::PERIPHERALINITABLE).toBool(matchingPeripheral->initable_);
                    }
                }
            }
        }
    }
    
    //-----------------------------------------------------------------------------
    // Function: RenodeUtilities::findMemory()
    //-----------------------------------------------------------------------------
    QSharedPointer<RenodeStructs::cpuMemories> findMemory(QString const& memoryName, QVector<QSharedPointer<RenodeStructs::cpuMemories> > const& memories)
    {
        for (auto currentMemory : memories)
        {
            if (currentMemory->memoryName_ == memoryName)
            {
                return currentMemory;
            }
        }

        return QSharedPointer<RenodeStructs::cpuMemories>();
    }

    //-----------------------------------------------------------------------------
    // Function: RenodeUtilities::setupMemoryConfiguration()
    //-----------------------------------------------------------------------------
    void setupMemoryConfiguration(QJsonObject const& configurationObject, QVector<QSharedPointer<RenodeStructs::cpuMemories> >& memories)
    {
        QJsonValue memoriesValue = configurationObject.value(RenodeConstants::MEMORY);
        if (memoriesValue.isArray())
        {
            QJsonArray memoryArray = memoriesValue.toArray();

            for (int arrayIndex = 0; arrayIndex < memoryArray.count(); arrayIndex++)
            {
                QJsonValue singleMemoryValue = memoryArray.at(arrayIndex);
                if (singleMemoryValue.isObject())
                {
                    QJsonObject memoryObject = singleMemoryValue.toObject();
                    QString memoryName = memoryObject.value(RenodeConstants::MEMORYNAME).toString("");

                    QSharedPointer<RenodeStructs::cpuMemories> matchingMemory = findMemory(memoryName, memories);
                    if (matchingMemory)
                    {
                        matchingMemory->className_ = memoryObject.value(RenodeConstants::MEMORYCLASS).toString(matchingMemory->className_);
                    }
                }
            }
        }
    }
};

//-----------------------------------------------------------------------------
// Function: RenodeUtilities::getRenodeCpuRoutes()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<RenodeCPUDetailRoutes> > RenodeUtilities::getRenodeCpuRoutes(QJsonObject const& configurationObject, LibraryInterface* library,
    QSharedPointer<Component> component, QString const& viewName)
{
    QVector<QSharedPointer<RenodeCPUDetailRoutes> > cpuDetails;
    for (auto defaultCPU : ConnectivityGraphUtilities::getDefaultCPUs(library, component, viewName))
    {
        QVector<QSharedPointer<RenodeStructs::cpuPeripherals> > peripherals;
        QVector<QSharedPointer<RenodeStructs::cpuMemories> > memories;
        QStringList peripheralNames;
        QStringList memoryNames;

        for (auto cpuInterfaceContainer : defaultCPU->getRoutes())
        {
            QSharedPointer<const ConnectivityInterface> cpuInterface = cpuInterfaceContainer->cpuInterface_;
            for (auto masterSlaveRoute : cpuInterfaceContainer->routes_)
            {
                for (auto routeInterface : masterSlaveRoute)
                {
                    QSharedPointer<const ConnectivityComponent> interfacedComponent = routeInterface->getInstance();
                    QSharedPointer<MemoryItem> interfaceMemory = routeInterface->getConnectedMemory();

                    if (interfaceMemory && interfaceMemory->getType().compare(MemoryDesignerConstants::MEMORYMAP_TYPE, Qt::CaseInsensitive) == 0)
                    {
                        QSharedPointer<const Component> component = ConnectivityGraphUtilities::getInterfacedComponent(library, interfacedComponent);

                        MemoryConnectionAddressCalculator::ConnectionPathVariables pathAddresses =
                            MemoryConnectionAddressCalculator::calculatePathAddresses(cpuInterface, routeInterface, masterSlaveRoute);

                        quint64 memoryBaseAddress = pathAddresses.remappedAddress_;

                        QVector<QPair<General::Usage, QVector<QSharedPointer<MemoryItem> > > > subItemBlocks;

                        QPair<General::Usage, QVector<QSharedPointer<MemoryItem> > > currentItemBlock;

                        QMultiMap<quint64, QSharedPointer<MemoryItem> > orderedChildItems = getOrderedChildItems(interfaceMemory);
                        currentItemBlock.first = getFirstBlockUsage(orderedChildItems);

                        QMultiMapIterator<quint64, QSharedPointer<MemoryItem> > blockIterator(orderedChildItems);
                        while (blockIterator.hasNext())
                        {
                            blockIterator.next();
                            QSharedPointer<MemoryItem> blockItem = blockIterator.value();

                            if (blockItem->getType().compare(MemoryDesignerConstants::ADDRESSBLOCK_TYPE, Qt::CaseInsensitive) == 0)
                            {
                                if (usageIsMatching(currentItemBlock.first, blockItem->getUsage()))
                                {
                                    currentItemBlock.second.append(blockItem);
                                }
                                else
                                {
                                    createItemBlock(currentItemBlock.first, currentItemBlock.second,
                                        peripherals, memories, interfaceMemory, memoryBaseAddress, peripheralNames, memoryNames);

                                    currentItemBlock.first = blockItem->getUsage();
                                    currentItemBlock.second.clear();
                                    currentItemBlock.second.append(blockItem);
                                }
                            }
                            else
                            {
                                createItemBlock(currentItemBlock.first, currentItemBlock.second,
                                    peripherals, memories, interfaceMemory, memoryBaseAddress, peripheralNames, memoryNames);

                                currentItemBlock.first = General::USAGE_COUNT;
                                currentItemBlock.second.clear();
                            }
                        }

                        createItemBlock(currentItemBlock.first, currentItemBlock.second, peripherals, memories, interfaceMemory,
                            memoryBaseAddress, peripheralNames, memoryNames);
                    }
                }
            }
        }


        QSharedPointer<RenodeCPUDetailRoutes> renodeCPU(new RenodeCPUDetailRoutes(*defaultCPU.data()));

        QJsonValue multiCpuValue = configurationObject.value(RenodeConstants::CPUS);
        if (multiCpuValue.isArray())
        {
            QJsonArray cpuArray = multiCpuValue.toArray();
            if (!cpuArray.isEmpty())
            {
                QJsonValue cpuValue = cpuArray.first();
                if (cpuValue.isObject())
                {
                    QJsonObject cpuObject = cpuValue.toObject();
                    renodeCPU->setClassName(cpuObject.value(RenodeConstants::CPUCLASS).toString(renodeCPU->getClassName()));
                    renodeCPU->setTimeProvider(cpuObject.value(RenodeConstants::CPUTIME).toString(renodeCPU->getTimeProvider()));
                    renodeCPU->setCpuType(cpuObject.value(RenodeConstants::CPUTYPE).toString(renodeCPU->getCpuType()));

                    setupPeripheralConfiguration(cpuObject, peripherals);
                    setupMemoryConfiguration(cpuObject, memories);
                }
            }
        }

        renodeCPU->setPeripherals(peripherals);
        renodeCPU->setMemories(memories);
        cpuDetails.append(renodeCPU);
    }

    return cpuDetails;
}
