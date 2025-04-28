//-----------------------------------------------------------------------------
// File: MemoryConnectionAddressCalculator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.05.2018
//
// Description:
// Calculates address ranges for connected memory items.
//-----------------------------------------------------------------------------

#include "MemoryConnectionAddressCalculator.h"

#include <KactusAPI/include/PluginUtilityAdapter.h>

#include <editors/MemoryDesigner/ConnectivityComponent.h>

#include <editors/MemoryDesigner/ConnectivityInterface.h>
#include <editors/MemoryDesigner/MemoryDesignerConstants.h>
#include <editors/MemoryDesigner/MemoryItem.h>

//-----------------------------------------------------------------------------
// Function: MemoryConnectionAddressCalculator::calculatePathAddresses()
//-----------------------------------------------------------------------------
MemoryConnectionAddressCalculator::CalculatedPathAddresses MemoryConnectionAddressCalculator::calculatePathAddresses(
    QSharedPointer<const ConnectivityInterface> startInterface,
    QSharedPointer<const ConnectivityInterface> endInterface,
    QVector<QSharedPointer<ConnectivityInterface const> > connectionPath)
{
    //! Base address for the connection.
    quint64 connectionBaseAddress = 0;
    //! Last address available from the point of the starting item.
    quint64 availableLastAddress = 0;

    QString remapValue("");
    QString remapRange("");

    if (startInterface != endInterface)
    {
        auto pathMemoryItem = startInterface->getConnectedMemory();
        auto memoryItemRange = pathMemoryItem->getRange().toULongLong();
        availableLastAddress = memoryItemRange - 1;

        for (auto pathInterface : connectionPath)
        {
            //! Initiator interfaces add their base address to the base address of the connection.
            //! Size of the connected item, or a remap range value determines the last available address for the connection.
            if (auto pathMode = pathInterface->getMode(); (pathMode == General::MASTER || pathMode == General::INITIATOR) &&
                pathInterface->isConnectedToMemory())
            {
                if (remapRange.isEmpty())
                {
                    pathMemoryItem = pathInterface->getConnectedMemory();
                    memoryItemRange = pathMemoryItem->getRange().toULongLong();
                }
                else
                {
                    memoryItemRange = remapRange.toULongLong();
                    remapRange.clear();
                }
                if (remapValue.isEmpty() == false)
                {
                    connectionBaseAddress += remapValue.toULongLong();
                }

                if (quint64 newLastAddress = connectionBaseAddress + memoryItemRange - 1; newLastAddress < availableLastAddress)
                {
                    availableLastAddress = newLastAddress;
                }

                connectionBaseAddress += pathInterface->getBaseAddress().toULongLong();
                remapValue.clear();
            }

            //! Mirrored target interfaces remap the base address for the connected item.
            //! Range determines the size of the connection
            else if ((pathMode == General::MIRRORED_SLAVE || pathMode == General::MIRRORED_TARGET) &&
                pathInterface->getRemapAddress().isEmpty() == false &&
                pathInterface->getRemapRange().isEmpty() == false)
            {
                remapValue = pathInterface->getRemapAddress();
                remapRange = pathInterface->getRemapRange();
            }
        }
    }
    else
    {
        auto startMemory = startInterface->getConnectedMemory();
        auto startRange = startMemory->getRange().toULongLong();
        
        availableLastAddress = connectionBaseAddress + startRange - 1;
    }

    auto mapItem = MemoryDesignerConstants::getMapItem(startInterface, endInterface);
    auto [mapBaseAddress, mapLastAddress] = getMemoryMapAddressRanges(mapItem);
    auto mapSize = mapLastAddress - mapBaseAddress;

    if (remapValue.isEmpty() == false)
    {
        mapBaseAddress = remapValue.toULongLong();
    }
    if (remapRange.isEmpty() == false)
    {
        mapSize = remapRange.toULongLong() - 1;
    }

    CalculatedPathAddresses pathAddresses;

    connectionBaseAddress += mapBaseAddress;
    quint64 connectionLastAddress = connectionBaseAddress + mapSize;

    if (connectionBaseAddress < availableLastAddress)
    {
        if (connectionLastAddress > availableLastAddress)
        {
            connectionLastAddress = availableLastAddress;
        }

        pathAddresses.connectionBaseAddress_ = connectionBaseAddress;
        pathAddresses.connectionLastAddress_ = connectionLastAddress;
        pathAddresses.createConnection_ = true;
    }

    return pathAddresses;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionAddressCalculator::getMemoryMapAddressRanges()
//-----------------------------------------------------------------------------
QPair<quint64, quint64> MemoryConnectionAddressCalculator::getMemoryMapAddressRanges(
    QSharedPointer<MemoryItem> mapItem)
{
    quint64 baseAddress = 0;
    quint64 lastAddress = 0;
    if (mapItem && mapItem->getChildItems().size() > 0)
    {
        bool firstBlock = true;

        foreach(QSharedPointer<MemoryItem const> blockItem, mapItem->getChildItems())
        {
            if (blockItem->getType().compare(MemoryDesignerConstants::ADDRESSBLOCK_TYPE, Qt::CaseInsensitive) == 0)
            {
                quint64 blockBaseAddress = blockItem->getAddress().toULongLong();
                quint64 blockRange = blockItem->getRange().toULongLong();
                quint64 blockEndPoint = blockBaseAddress + blockRange - 1;

                if (firstBlock)
                {
                    baseAddress = blockBaseAddress;
                    firstBlock = false;
                }
                else
                {
                    if (blockBaseAddress < baseAddress)
                    {
                        baseAddress = blockBaseAddress;
                    }
                }

                if (blockEndPoint > lastAddress)
                {
                    lastAddress = blockEndPoint;
                }
            }
        }
    }

    QPair<quint64, quint64> memoryRanges(baseAddress, lastAddress);
    return memoryRanges;
}
