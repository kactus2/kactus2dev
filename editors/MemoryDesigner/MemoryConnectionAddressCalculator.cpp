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

#include <editors/MemoryDesigner/ConnectivityInterface.h>
#include <editors/MemoryDesigner/MemoryDesignerConstants.h>
#include <editors/MemoryDesigner/MemoryItem.h>

//-----------------------------------------------------------------------------
// Function: MemoryConnectionAddressCalculator::calculatePathAddresses()
//-----------------------------------------------------------------------------
MemoryConnectionAddressCalculator::ConnectionPathVariables MemoryConnectionAddressCalculator::
    calculatePathAddresses(QSharedPointer<const ConnectivityInterface> startInterface,
        QSharedPointer<const ConnectivityInterface> endInterface,
        QVector<QSharedPointer<ConnectivityInterface const> > connectionPath)
{
    MemoryConnectionAddressCalculator::ConnectionPathVariables newPathVariables;
    newPathVariables.hasRemapRange_ = false;
    newPathVariables.isChainedSpaceConnection_ = false;
    newPathVariables.baseAddressNumber_ = getStartingBaseAddress(startInterface, endInterface);
    newPathVariables.mirroredSlaveAddressChange_ = 0;

    QSharedPointer<MemoryItem> mapItem = MemoryDesignerConstants::getMapItem(startInterface, endInterface);

    QPair<quint64, quint64> mapAddressRanges = getMemoryMapAddressRanges(mapItem);
    newPathVariables.spaceChainBaseAddress_ = 0;
    newPathVariables.memoryMapEndAddress_ = mapAddressRanges.second;

    QSharedPointer<MemoryItem> startSpace = startInterface->getConnectedMemory();
    MemoryConnectionAddressCalculator::ChainedSpace newChainedSpace;
    newChainedSpace.spaceConnectionBaseAddress_ = 0;
    newChainedSpace.spaceInterface_ = startInterface;

    newPathVariables.spaceChain_.append(newChainedSpace);

    for (QSharedPointer<ConnectivityInterface const> pathInterface : connectionPath)
    {
        if (pathInterface != startInterface && pathInterface != endInterface)
        {
            General::InterfaceMode interfaceMode = pathInterface->getMode();
            if ((interfaceMode == General::MIRRORED_SLAVE || interfaceMode == General::MIRRORED_TARGET) &&
                !pathInterface->getRemapAddress().isEmpty() && !pathInterface->getRemapRange().isEmpty())
            {
                newPathVariables.mirroredSlaveAddressChange_ += pathInterface->getRemapAddress().toULongLong();

                newPathVariables.memoryMapEndAddress_ = pathInterface->getRemapRange().toULongLong() - 1;
                newPathVariables.hasRemapRange_ = true;
            }
            else if ((interfaceMode == General::MASTER || interfaceMode == General::INITIATOR) &&
                pathInterface->isConnectedToMemory())
            {
                newPathVariables.spaceChainBaseAddress_ += newPathVariables.baseAddressNumber_;

                if (QSharedPointer<MemoryItem> middleSpace = pathInterface->getConnectedMemory(); middleSpace)
                {
                    quint64 chainOffset = newPathVariables.spaceChainBaseAddress_;

                    MemoryConnectionAddressCalculator::ChainedSpace middleChainedSpace;
                    middleChainedSpace.spaceInterface_ = pathInterface;
                    middleChainedSpace.spaceConnectionBaseAddress_ = chainOffset;

                    newPathVariables.spaceChain_.append(middleChainedSpace);
                    newPathVariables.isChainedSpaceConnection_ = true;
                }

                newPathVariables.baseAddressNumber_ = pathInterface->getBaseAddress().toULongLong();
            }
        }
    }

    newPathVariables.memoryMapBaseAddress_ = mapAddressRanges.first;
    newPathVariables.endAddressNumber_ =
        newPathVariables.baseAddressNumber_ + newPathVariables.memoryMapEndAddress_;
    newPathVariables.remappedAddress_ =
        MemoryConnectionAddressCalculator::getRemappedBaseAddress(newPathVariables.memoryMapBaseAddress_,
        newPathVariables.baseAddressNumber_, newPathVariables.spaceChainBaseAddress_,
        newPathVariables.mirroredSlaveAddressChange_, newPathVariables.hasRemapRange_);
    newPathVariables.remappedEndAddress_ = newPathVariables.endAddressNumber_ +
        newPathVariables.spaceChainBaseAddress_ + newPathVariables.mirroredSlaveAddressChange_;

    return newPathVariables;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionAddressCalculator::getStartingBaseAddress()
//-----------------------------------------------------------------------------
quint64 MemoryConnectionAddressCalculator::getStartingBaseAddress(
    QSharedPointer<ConnectivityInterface const> startInterface,
    QSharedPointer<ConnectivityInterface const> endInterface)
{
    quint64 baseAddressNumber = 0;
    if (startInterface != endInterface)
    {
        QString startInterfaceBaseAddress = startInterface->getBaseAddress();
        if (startInterfaceBaseAddress.compare(QStringLiteral("x"), Qt::CaseInsensitive) != 0)
        {
            baseAddressNumber = startInterfaceBaseAddress.toULongLong();
        }
    }

    return baseAddressNumber;
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

//-----------------------------------------------------------------------------
// Function: MemoryConnectionAddressCalculator::getRemappedBaseAddress()
//-----------------------------------------------------------------------------
quint64 MemoryConnectionAddressCalculator::getRemappedBaseAddress(quint64 memoryMapBaseAddress,
    quint64 baseAddressNumber, quint64 spaceChainConnectionBaseAddress, quint64 mirroredSlaveAddressChange,
    bool hasRemapRange)
{
    quint64 remappedAddress =
        baseAddressNumber + spaceChainConnectionBaseAddress + mirroredSlaveAddressChange;

    if (!hasRemapRange)
    {
        remappedAddress += memoryMapBaseAddress;
    }

    return remappedAddress;
}
