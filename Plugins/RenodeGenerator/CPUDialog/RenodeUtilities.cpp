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

        for (auto const& subItem : memoryItem->getChildItems())
        {
            if (subItem->getType().compare(subItemType, Qt::CaseInsensitive) == 0)
            {
                subItems.append(subItem);
            }
        }

        return subItems;
    }

    //-----------------------------------------------------------------------------
    // Function: RenodeUtilities::getOrderedChildItems()
    //-----------------------------------------------------------------------------
    QMultiMap<quint64, QSharedPointer<MemoryItem> > getOrderedChildItems(QSharedPointer<MemoryItem> mapItem)
    {
        QMultiMap<quint64, QSharedPointer<MemoryItem>> blockItems;
        for (auto const& subItem : mapItem->getChildItems())
        {
            blockItems.insert(subItem->getAddress().toULongLong(), subItem);
        }

        return blockItems;
    }

    //-----------------------------------------------------------------------------
    // Function: CommonInterface::nameIsUnique()
    //-----------------------------------------------------------------------------
    bool nameIsUnique(QString const& mapName, QStringList const& peripheralNames)
    {
        return peripheralNames.contains(mapName) == false;
    }

    //-----------------------------------------------------------------------------
    // Function: RenodeUtilities::getUniqueName()
    //-----------------------------------------------------------------------------
    QString getUniqueName(QString const& referenceName, QStringList const& peripheralNames)
    {
        QString newName(referenceName.toLower());

        int runningNumber = 0;
        while (!nameIsUnique(newName, peripheralNames))
        {
            newName = referenceName.toLower() + QString::number(runningNumber);

            runningNumber++;
        }

        return newName;
    }

    //-----------------------------------------------------------------------------
    // Function: RenodeUtilities::createPeripheral()
    //-----------------------------------------------------------------------------
    QSharedPointer<RenodeStructs::cpuPeripheral> createPeripheral(QSharedPointer<MemoryItem> mapItem,
        quint64 mapBaseAddress, QVector<QSharedPointer<MemoryItem> > currentMemoryBlock,
        QStringList& peripheralNames)
    {
        QSharedPointer<MemoryItem> firstBlock = currentMemoryBlock.first();
        QSharedPointer<MemoryItem> lastBlock = currentMemoryBlock.last();

        quint64 offset = mapBaseAddress + firstBlock->getAddress().toULongLong();
        qint64 endAddress = mapBaseAddress + lastBlock->getAddress().toLongLong() + lastBlock->getRange().toLongLong() - 1;

        quint64 size = endAddress - offset + 1;

        QSharedPointer<RenodeStructs::cpuPeripheral> newPeripheral(new RenodeStructs::cpuPeripheral());
        QString newPeripheralName = getUniqueName(mapItem->getName(), peripheralNames);
        newPeripheral->peripheralName_ = newPeripheralName;
        newPeripheral->peripheralID_ = "";
        newPeripheral->baseAddress_ = valueToHexa(offset);
        newPeripheral->size_ = valueToHexa(size);
        newPeripheral->template_ = nullptr;

        peripheralNames.append(newPeripheralName);

        return newPeripheral;
    }

//-----------------------------------------------------------------------------
// Function: RenodeUtilities::createMemory()
//-----------------------------------------------------------------------------
    QSharedPointer<RenodeStructs::cpuMemories> createMemory(QSharedPointer<MemoryItem> mapItem,
        quint64 mapBaseAddress, QVector<QSharedPointer<MemoryItem> > currentMemoryBlock,
        QStringList& memoryNames)
    {
        QSharedPointer<MemoryItem> firstBlock = currentMemoryBlock.first();
        QSharedPointer<MemoryItem> lastBlock = currentMemoryBlock.last();

        quint64 offset = mapBaseAddress + firstBlock->getAddress().toULongLong();
        qint64 endAddress = mapBaseAddress + lastBlock->getAddress().toLongLong() + lastBlock->getRange().toLongLong() - 1;

        quint64 size = endAddress - offset + 1;

        QSharedPointer<RenodeStructs::cpuMemories> newMemory(new RenodeStructs::cpuMemories());
        QString newMemoryName = getUniqueName(mapItem->getName() + "_" + firstBlock->getName(), memoryNames);

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
        QVector<QSharedPointer<RenodeStructs::cpuPeripheral> >& peripherals,
        QVector<QSharedPointer<RenodeStructs::cpuMemories> >& memories,
        QSharedPointer<MemoryItem> interfaceMemory, quint64 const& memoryBaseAddress,
        QStringList& peripheralNames, QStringList& memoryNames)
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
        return (currentUsage == General::MEMORY && currentUsage == previousUsage) || 
            (currentUsage != General::MEMORY && previousUsage != General::MEMORY);
    }

    //-----------------------------------------------------------------------------
    // Function: RenodeUtilities::getFirstBlockUsage()
    //-----------------------------------------------------------------------------
    General::Usage getFirstBlockUsage(QMultiMap<quint64, QSharedPointer<MemoryItem> > orderedChildItems)
    {
        QMultiMapIterator blockIterator(orderedChildItems);
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
    QSharedPointer<RenodeStructs::cpuPeripheral> findPeripheral(QString const& peripheralName,
        QVector<QSharedPointer<RenodeStructs::cpuPeripheral> > const& peripherals)
    {
        for (auto currentPeripheral : peripherals)
        {
            if (currentPeripheral->peripheralName_ == peripheralName)
            {
                return currentPeripheral;
            }
        }

        return QSharedPointer<RenodeStructs::cpuPeripheral>();
    }

    //-----------------------------------------------------------------------------
    // Function: RenodeUtilities::setupPeripheralConfiguration()
    //-----------------------------------------------------------------------------
    void setupPeripheralConfiguration(QJsonObject const& configurationObject,
        QVector<QSharedPointer<RenodeStructs::cpuPeripheral> > const& peripherals,
        QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > peripheralTemplates)
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

                    QSharedPointer<RenodeStructs::cpuPeripheral> matchingPeripheral = findPeripheral(peripheralName, peripherals);
                    if (matchingPeripheral)
                    {
                        matchingPeripheral->className_ = peripheralObject.value(RenodeConstants::PERIPHERALCLASS).toString(matchingPeripheral->className_);
                        matchingPeripheral->filePath_ = peripheralObject.value(RenodeConstants::PERIPHERALPATH).toString(matchingPeripheral->filePath_);
                        matchingPeripheral->initable_ = peripheralObject.value(RenodeConstants::PERIPHERALINITABLE).toBool(matchingPeripheral->initable_);

                        QString templateName = peripheralObject.value(RenodeConstants::PERIPHERALTEMPLATE).toString("");
                        if (!templateName.isEmpty())
                        {
                            matchingPeripheral->template_ = RenodeUtilities::getTemplateFromList(templateName, peripheralTemplates);
                        }
                    }
                }
            }
        }
    }
    
    //-----------------------------------------------------------------------------
    // Function: RenodeUtilities::findMemory()
    //-----------------------------------------------------------------------------
    QSharedPointer<RenodeStructs::cpuMemories> findMemory(QString const& memoryName,
        QVector<QSharedPointer<RenodeStructs::cpuMemories> > const& memories)
    {
        for (auto const& currentMemory : memories)
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
    void setupMemoryConfiguration(QJsonObject const& configurationObject,
        QVector<QSharedPointer<RenodeStructs::cpuMemories> >& memories)
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

    //-----------------------------------------------------------------------------
    // Function: RenodeUtilities::getConfigurationObjectForCpu()
    //-----------------------------------------------------------------------------
    QSharedPointer<RenodeCpuRoutesContainer> createAndConfigureCpuContainer(QJsonObject const& configurationObject,
        QSharedPointer<SingleCpuRoutesContainer> cpuContainer, QVector<QSharedPointer<RenodeStructs::cpuPeripheral> >& peripherals,
        QVector<QSharedPointer<RenodeStructs::cpuMemories> >& memories,
        QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > templates)
    {
        QSharedPointer<RenodeCpuRoutesContainer> renodeCpuContainer(new RenodeCpuRoutesContainer(*cpuContainer.data()));
		if (auto multiCpuValue = configurationObject.value(RenodeConstants::CPUS); multiCpuValue.isArray())
        {
            QJsonArray cpuArray = multiCpuValue.toArray();
            if (!cpuArray.isEmpty())
            {
                for (auto const& cpuValue : cpuArray)
                {
                    if (cpuValue.isObject())
                    {
                        QJsonObject cpuObject = cpuValue.toObject();
                        if (cpuObject.value(RenodeConstants::CPUNAME) == renodeCpuContainer->getFileName())
                        {
                            renodeCpuContainer->setClassName(cpuObject.value(RenodeConstants::CPUCLASS).toString(renodeCpuContainer->getClassName()));
                            renodeCpuContainer->setTimeProvider(cpuObject.value(RenodeConstants::CPUTIME).toString(renodeCpuContainer->getTimeProvider()));
                            renodeCpuContainer->setCpuType(cpuObject.value(RenodeConstants::CPUTYPE).toString(renodeCpuContainer->getCpuType()));

                            setupPeripheralConfiguration(cpuObject, peripherals, templates);
                            setupMemoryConfiguration(cpuObject, memories);
                        }
                    }
                }
            }
        }

        renodeCpuContainer->setPeripherals(peripherals);
        renodeCpuContainer->setMemories(memories);

        return renodeCpuContainer;
    }
};

//-----------------------------------------------------------------------------
// Function: RenodeUtilities::getRenodeCpuRoutes()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<RenodeCpuRoutesContainer> > RenodeUtilities::getRenodeCpuRoutes(
    QJsonObject const& configurationObject, LibraryInterface* library, QSharedPointer<Component> component,
    QString const& viewName, QVector<QSharedPointer<RenodeStructs::peripheralTemplate>> templates)
{
    QVector<QSharedPointer<RenodeCpuRoutesContainer> > cpuDetails;
    for (auto defaultCPU : ConnectivityGraphUtilities::getDefaultCPUs(library, component, viewName))
    {
        QVector<QSharedPointer<RenodeStructs::cpuPeripheral> > peripherals;
        QVector<QSharedPointer<RenodeStructs::cpuMemories> > memories;
        QStringList peripheralNames;
        QStringList memoryNames;

        for (auto cpuInterfaceContainer : defaultCPU->getRoutes())
        {
            QSharedPointer<const ConnectivityInterface> cpuInterface = cpuInterfaceContainer->cpuInterface_;
            for (auto const& masterSlaveRoute : cpuInterfaceContainer->routes_)
            {
                for (auto routeInterface : masterSlaveRoute)
                {
                    QSharedPointer<const ConnectivityComponent> interfacedComponent = routeInterface->getInstance();
                    QSharedPointer<MemoryItem> interfaceMemory = routeInterface->getConnectedMemory();

                    if (interfaceMemory && interfaceMemory->getType().compare(MemoryDesignerConstants::MEMORYMAP_TYPE, Qt::CaseInsensitive) == 0)
                    {
                        MemoryConnectionAddressCalculator::ConnectionPathVariables pathAddresses =
                            MemoryConnectionAddressCalculator::calculatePathAddresses(cpuInterface, routeInterface, masterSlaveRoute);

                        quint64 memoryBaseAddress = pathAddresses.remappedAddress_;

                        QVector<QPair<General::Usage, QVector<QSharedPointer<MemoryItem> > > > subItemBlocks;

                        QPair<General::Usage, QVector<QSharedPointer<MemoryItem> > > currentItemBlock;

                        QMultiMap<quint64, QSharedPointer<MemoryItem> > orderedChildItems = getOrderedChildItems(interfaceMemory);
                        currentItemBlock.first = getFirstBlockUsage(orderedChildItems);

                        QMultiMapIterator blockIterator(orderedChildItems);
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

        QSharedPointer<RenodeCpuRoutesContainer> renodeCpuContainer =
            createAndConfigureCpuContainer(configurationObject, defaultCPU, peripherals, memories, templates);
        cpuDetails.append(renodeCpuContainer);
    }

    return cpuDetails;
}

//-----------------------------------------------------------------------------
// Function: RenodeUtilities::getTemplateNames()
//-----------------------------------------------------------------------------
QStringList RenodeUtilities::getTemplateNames(QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > templates)
{
    QStringList names;
    for (auto const& singleTemplate : templates)
    {
        names.append(singleTemplate->identifier_);
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: RenodeUtilities::getTemplateFromList()
//-----------------------------------------------------------------------------
QSharedPointer<RenodeStructs::peripheralTemplate> RenodeUtilities::getTemplateFromList(QString const& templateName,
    QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > templates)
{
    for (auto const& singleTemplate : templates)
    {
        if (singleTemplate->identifier_ == templateName)
        {
            return singleTemplate;
        }
    }

    return nullptr;
}
