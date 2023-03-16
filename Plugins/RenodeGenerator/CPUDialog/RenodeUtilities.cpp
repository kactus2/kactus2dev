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
    // Function: RenodeUtilities::getPeripheral()
    //-----------------------------------------------------------------------------
    QSharedPointer<RenodeStructs::cpuPeripherals> createPeripheral(QSharedPointer<MemoryItem> mapItem, quint64 mapBaseAddress,
        QVector<QSharedPointer<MemoryItem> > currentMemoryBlock, QStringList& peripheralNames)
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
    // Function: RenodeUtilities::createAndAddPeripheral()
    //-----------------------------------------------------------------------------
    void createAndAddPeripheral(QVector<QSharedPointer<MemoryItem> >& currentMemoryBlock, QVector<QSharedPointer<RenodeStructs::cpuPeripherals> >& peripherals,
        QSharedPointer<MemoryItem> interfaceMemory, quint64 const& memoryBaseAddress, QStringList& peripheralNames)
    {
        if (!currentMemoryBlock.isEmpty())
        {
            peripherals.append(createPeripheral(interfaceMemory, memoryBaseAddress, currentMemoryBlock, peripheralNames));

            currentMemoryBlock.clear();
        }
    }
};

//-----------------------------------------------------------------------------
// Function: RenodeUtilities::getRenodeCpuRoutes()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<RenodeCPUDetailRoutes> > RenodeUtilities::getRenodeCpuRoutes(LibraryInterface* library, QSharedPointer<Component> component, QString const& viewName)
{
    QVector<QSharedPointer<RenodeCPUDetailRoutes> > cpuDetails;
    for (auto defaultCPU : ConnectivityGraphUtilities::getDefaultCPUs(library, component, viewName))
    {
        QSharedPointer<const ConnectivityInterface> cpuInterface = defaultCPU->getCPUInterface();
        QVector<QSharedPointer<RenodeStructs::cpuPeripherals> > peripherals;
        QStringList peripheralNames;

        for (auto masterSlaveRoute : *defaultCPU->getRoutes())
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

                    QVector<QSharedPointer<MemoryItem> > currentMemoryBlock;

                    QMultiMapIterator<quint64, QSharedPointer<MemoryItem> > blockIterator(getOrderedChildItems(interfaceMemory));
                    while (blockIterator.hasNext())
                    {
                        blockIterator.next();
                        QSharedPointer<MemoryItem> blockItem = blockIterator.value();
                        if (blockItem->getType().compare(MemoryDesignerConstants::ADDRESSBLOCK_TYPE, Qt::CaseInsensitive) != 0)
                        {
                            createAndAddPeripheral(currentMemoryBlock, peripherals, interfaceMemory, memoryBaseAddress, peripheralNames);
                        }
                        else
                        {
                            if (blockItem->getUsage() == General::MEMORY)
                            {
                                createAndAddPeripheral(currentMemoryBlock, peripherals, interfaceMemory, memoryBaseAddress, peripheralNames);
                            }
                            else
                            {
                                currentMemoryBlock.append(blockItem);
                            }
                        }
                    }

                    createAndAddPeripheral(currentMemoryBlock, peripherals, interfaceMemory, memoryBaseAddress, peripheralNames);
                }
            }
        }

        QSharedPointer<RenodeCPUDetailRoutes> renodeCPU(new RenodeCPUDetailRoutes(*defaultCPU.data()));
        renodeCPU->setPeripherals(peripherals);
        cpuDetails.append(renodeCPU);
    }

    return cpuDetails;
}
