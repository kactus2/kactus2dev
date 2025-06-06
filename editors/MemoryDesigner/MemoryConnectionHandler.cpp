//-----------------------------------------------------------------------------
// File: MemoryConnectionHandler.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 03.01.2017
//
// Description:
// Constructs the memory connections for the memory design diagram.
//-----------------------------------------------------------------------------

#include "MemoryConnectionHandler.h"

#include <editors/MemoryDesigner/MasterSlavePathSearch.h>
#include <editors/MemoryDesigner/ConnectivityComponent.h>
#include <editors/MemoryDesigner/ConnectivityInterface.h>
#include <editors/MemoryDesigner/MemoryDesignerConstants.h>
#include <editors/MemoryDesigner/MainMemoryGraphicsItem.h>
#include <editors/MemoryDesigner/MemoryExtensionGraphicsItem.h>
#include <editors/MemoryDesigner/MemoryColumn.h>
#include <editors/MemoryDesigner/MemoryConnectionItem.h>
#include <editors/MemoryDesigner/MemoryCollisionItem.h>
#include <editors/MemoryDesigner/MemoryColumnHandler.h>
#include <editors/MemoryDesigner/MemoryItem.h>
#include <editors/MemoryDesigner/MemoryGraphicsItemHandler.h>

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::MemoryConnectionHandler()
//-----------------------------------------------------------------------------
MemoryConnectionHandler::MemoryConnectionHandler(QSharedPointer<MemoryColumnHandler> columnHandler, QSharedPointer<MemoryGraphicsItemHandler> itemHandler):
columnHandler_(columnHandler),
itemHandler_(itemHandler)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::setCondenseMemoryItems()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::setCondenseMemoryItems(bool condenseMemoryItems)
{
    condenseMemoryItems_ = condenseMemoryItems;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::memoryItemsAreCondensed()
//-----------------------------------------------------------------------------
bool MemoryConnectionHandler::memoryItemsAreCondensed() const
{
    return condenseMemoryItems_;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::setFilterAddressSpaceChains()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::setFilterAddressSpaceChains(bool filterChains)
{
    filterAddressSpaceChains_ = filterChains;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::addressSpaceChainsAreFiltered()
//-----------------------------------------------------------------------------
bool MemoryConnectionHandler::addressSpaceChainsAreFiltered() const
{
    return filterAddressSpaceChains_;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::clearConnectionLists()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::clearConnectionLists()
{
    connectionsToMemoryMaps_.clear();
    memoryCollisions_.clear();
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::reDrawConnectionsAndCollisions()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::reDrawConnectionsAndCollisions()
{
    for (auto connectionItem : connectionsToMemoryMaps_)
    {
        connectionItem->reDrawConnection();
    }
    for (auto collisionItem : memoryCollisions_)
    {
        collisionItem->reDrawCollision();
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::createConnectedItems()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::createConnectedItems(QSharedPointer<ConnectivityGraph> connectionGraph, MemoryColumn* spaceColumn, MemoryColumn* memoryMapColumn)
{
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems(
        new QVector<MainMemoryGraphicsItem*>());
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems(
        new QVector<MainMemoryGraphicsItem*>());
    QVector<MemoryConnectionItem*> connections;

    MasterSlavePathSearch pathSearcher;
    QVector<Path> masterSlavePaths = pathSearcher.findMasterSlavePaths(connectionGraph, false);
    if (masterSlavePaths.isEmpty() == false)
    {
        QVector<QSharedPointer<QVector<Path> > > pathSets = findPathSets(masterSlavePaths);

        qreal spaceYPlacement = MemoryDesignerConstants::SPACEITEMINTERVAL;

        for (auto const& singleSet : pathSets)
        {
            QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItemsInSet(new QVector<MainMemoryGraphicsItem*>());
            QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItemsInSet(new QVector<MainMemoryGraphicsItem*>());

            connections.append(createConnectionSet(singleSet, placedMapItemsInSet, memoryMapColumn, spaceColumn, spaceYPlacement, placedSpaceItemsInSet));

            placedMapItems->append(*placedMapItemsInSet);
            placedSpaceItems->append(*placedSpaceItemsInSet);

            // Reposition items in set
            repositionCompressedAddressSpaces(placedSpaceItemsInSet);
            repositionCompressedMemoryMaps(placedMapItemsInSet, memoryMapColumn);
        }
    }

    spaceColumn->compressUnconnectedMemoryItems(condenseMemoryItems_, placedSpaceItems);
    memoryMapColumn->compressUnconnectedMemoryItems(condenseMemoryItems_, placedMapItems);

    spaceColumn->moveUnconnectedMemoryItems(placedSpaceItems);
    memoryMapColumn->moveUnconnectedMemoryItems(placedMapItems);

    if (masterSlavePaths.isEmpty() == false)
    {
        redrawMemoryConnections(connections);
        createOverlappingConnectionMarkers(placedSpaceItems);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::findPathSets()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<QVector<MemoryConnectionHandler::Path> > > MemoryConnectionHandler::findPathSets(QVector<Path> const& masterSlavePaths) const
{
    QVector<QSharedPointer<QVector<Path> > > pathSets;

    for (Path currentPath : masterSlavePaths)
    {
        auto setIndexes = findPathSetIndexes(currentPath, pathSets);
        if (setIndexes.isEmpty())
        {
            QSharedPointer<QVector<Path> > newSet(new QVector<Path>());
            newSet->append(currentPath);
            pathSets.append(newSet);
        }
        else
        {
            //! Place into the first set
            auto firstSet = pathSets.at(setIndexes.dequeue());
            firstSet->append(currentPath);

            //! Place other sets into the first set
            while (setIndexes.isEmpty() == false)
            {
                auto secondarySet = pathSets.takeAt(setIndexes.dequeue());
                firstSet->append(*secondarySet);
            }
        }
    }

    return pathSets;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::findPathSetIndexes()
//-----------------------------------------------------------------------------
QQueue<int> MemoryConnectionHandler::findPathSetIndexes(Path const& currentPath, QVector<QSharedPointer<QVector<Path> > > const& pathSets) const
{
    QQueue<int> pathSetIndexes;

    for (int i = 0; i < pathSets.size(); ++i)
    {
        auto currentPathSet = pathSets.at(i);
        if (pathIsContainedWithPathSet(currentPath, currentPathSet))
        {
            pathSetIndexes.enqueue(i);
        }
    }

    return pathSetIndexes;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::pathIsContainedWithPathSet()
//-----------------------------------------------------------------------------
bool MemoryConnectionHandler::pathIsContainedWithPathSet(Path const& currentPath, QSharedPointer<QVector<Path>> pathSet) const
{
    for (auto const& comparisonPath : *pathSet)
    {
        for (auto const& pathInterface : currentPath)
        {
            if (pathInterface->isConnectedToMemory() && interfacedItemIsWithinPath(pathInterface, comparisonPath))
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::interfacedItemIsWithinPath()
//-----------------------------------------------------------------------------
bool MemoryConnectionHandler::interfacedItemIsWithinPath(QSharedPointer<const ConnectivityInterface> pathInterface, Path const& comparisonPath) const
{
    for (auto const& comparisonInterface : comparisonPath)
    {
        if (pathInterface->getConnectedMemory() == comparisonInterface->getConnectedMemory())
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::createConnectionSet()
//-----------------------------------------------------------------------------
QVector<MemoryConnectionItem*> MemoryConnectionHandler::createConnectionSet(QSharedPointer<QVector<Path>> pathSet,
    QSharedPointer<QVector<MainMemoryGraphicsItem*>> placedMapItems,
    MemoryColumn* memoryMapColumn,
    MemoryColumn* spaceColumn,
    qreal& spaceYPlacement,
    QSharedPointer<QVector<MainMemoryGraphicsItem*>> placedSpaceItems)
{
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedItems(new QVector<MainMemoryGraphicsItem*>());
    QSharedPointer<QVector<QSharedPointer<LinkedGraphicItem> > > roots(new QVector<QSharedPointer<LinkedGraphicItem> >());

    for (auto const& connectionPath : *pathSet)
    {
        createLinkedItemsForPath(placedItems, roots, spaceColumn, memoryMapColumn, connectionPath);
    }

    if (filterAddressSpaceChains_)
    {
        createOnlyEndsConnectionFromLinks(roots, placedSpaceItems, placedMapItems, memoryMapColumn);
    }
    else
    {
        createFullConnectionFromLinks(roots, placedSpaceItems, placedMapItems, memoryMapColumn);
    }

    if (placedSpaceItems->isEmpty())
    {
        return QVector<MemoryConnectionItem*>();
    }

    QVector<MemoryConnectionItem*> connections;

    auto separatedPathSets = separateBrokenPathSet(placedSpaceItems);
    for (auto pathSet : separatedPathSets)
    {
        auto highestItemInSet = pathSet->connectedSpaces_->first();
        highestItemInSet = getHighestPlacedItemInSet(highestItemInSet, pathSet->connectedSpaces_);
        highestItemInSet = getHighestPlacedItemInSet(highestItemInSet, pathSet->connectedMaps_);

        auto yMovement = spaceYPlacement - highestItemInSet->scenePos().y();

        // Move entire group of connections to sensible vertical position.
        // Round movement up to integer, but std::ceil instead of qCeil to preserve type.
        highestItemInSet->moveItemAndConnectedItems(std::ceil(yMovement));

        for (auto compressedConnection : compressConnectedMemoryItems(pathSet->connectedSpaces_, pathSet->connectedMaps_))
        {
            connections.append(compressedConnection);
        }

        spaceYPlacement = highestItemInSet->getLowestPointOfConnectedItems() + MemoryDesignerConstants::SPACEITEMINTERVAL;
    }

    return connections;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::createLinkedItemsForPath()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::createLinkedItemsForPath(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedItems,
    QSharedPointer<QVector<QSharedPointer<LinkedGraphicItem> > > roots,
    MemoryColumn* spaceColumn,
    MemoryColumn* mapColumn,
    const Path& connectionPath)
{
    //! Possible address changes due to mirrored target interfaces.
    QString remapValue = "";
    QString remapRange = "";

    //! Base addresses for the left and right side of the connection.
    quint64 leftBaseAddress = 0;
    quint64 rightBaseAddress = 0;

    //! Cumulative base address for the connections.
    quint64 connectionBaseAddress = 0;

    //! Amount the connection will be moved in relation to the connection start item.
    qreal connectedItemTransferY = 0;

    QSharedPointer<LinkedGraphicItem> previousItem = nullptr;
    QSharedPointer<const ConnectivityInterface> previousInterface = nullptr;

    for (auto const pathInterface : connectionPath)
    {
        if (General::InterfaceMode interfaceMode = pathInterface->getMode();
            (interfaceMode == General::MIRRORED_SLAVE || interfaceMode == General::MIRRORED_TARGET) && !pathInterface->getRemapAddress().isEmpty() && !pathInterface->getRemapRange().isEmpty())
        {
            remapValue = pathInterface->getRemapAddress();
            remapRange = pathInterface->getRemapRange();
        }
        //! Initiator and target interfaces must be connected to a memory item for some change to happen with the address calculation.
        else if (pathInterface->isConnectedToMemory())
        {
            //! A path with a connection to itself is a connection to a local memory map.
            if (previousInterface == pathInterface)
            {
                createLocalLinkedItem(previousItem, pathInterface, placedItems, mapColumn);
                continue;
            }

            //! If this is reached, there is a connection to be made to an address space (initiator) or memory map (target).
            if (interfaceMode == General::MASTER || interfaceMode == General::INITIATOR ||
                interfaceMode == General::SLAVE || interfaceMode == General::TARGET)
            {
                auto pathGraphicsItem = getPathGraphicsItem(pathInterface);
                if (pathGraphicsItem == nullptr)
                {
                    continue;
                }

                if (pathGraphicsItem->isVisible() == false)
                {
                    pathGraphicsItem->setVisible(true);
                }

                //! Base address of the item is calculated because an item (especially memory map) can have a base address different from 0.
                //! Remap value changes this item base address.
                quint64 itemBaseAddress = pathGraphicsItem->getOriginalBaseAddress();

                //! Connection is made to the full item, unless a remap range is given.
                quint64 connectionSize = pathGraphicsItem->getOriginalLastAddress() - itemBaseAddress + 1;
                if (remapRange.isEmpty() == false)
                {
                    connectionSize = remapRange.toULongLong();
                }
                else if (leftBaseAddress != rightBaseAddress)
                {
                    if (rightBaseAddress >= connectionSize + leftBaseAddress)
                    {
                        //! Currently a negative connection that is out of bound for the connected item is ignored.
                        return;
                    }
                    else
                    {
                        connectionSize = connectionSize - (rightBaseAddress - leftBaseAddress);
                    }
                }
                if (remapValue.isEmpty() == false)
                {
                    itemBaseAddress = remapValue.toULongLong();
                }

                leftBaseAddress += itemBaseAddress;
                rightBaseAddress += itemBaseAddress;

                connectionBaseAddress += itemBaseAddress;

                qreal connectionTransfer = connectionBaseAddress * MemoryDesignerConstants::RANGEINTERVAL;

                QSharedPointer<LinkedGraphicItem> currentLinkedItem = nullptr;

                //! An address space connection is created to an initiator interface.
                //! The base address of the interface must be added to the cumulative base address calculation.
                //! The connection base address is set to begin from the base address of the interface.
                if (interfaceMode == General::MASTER || interfaceMode == General::INITIATOR)
                {
                    currentLinkedItem = findOrCreateLinkedItem(
                        placedItems, roots,
                        previousItem, pathGraphicsItem,
                        leftBaseAddress, rightBaseAddress, connectionSize,
                        connectedItemTransferY, connectionTransfer,
                        spaceColumn);

                    auto interfaceBaseAddress = pathInterface->getBaseAddress().toLongLong();
                    if (interfaceBaseAddress < 0)
                    {
                        connectionBaseAddress = 0;
                        rightBaseAddress = leftBaseAddress + abs(interfaceBaseAddress);
                        connectedItemTransferY = interfaceBaseAddress * MemoryDesignerConstants::RANGEINTERVAL;
                    }
                    else
                    {
                        connectionBaseAddress = interfaceBaseAddress;
                        leftBaseAddress += interfaceBaseAddress;
                        rightBaseAddress = leftBaseAddress;
                        connectedItemTransferY = 0;
                    }

                }
                //! A memory map connection is created to a target interface.
                else if ((interfaceMode == General::TARGET || interfaceMode == General::SLAVE) && pathInterface == connectionPath.last())
                {
                    currentLinkedItem = findOrCreateLinkedItem(placedItems, roots, previousItem, pathGraphicsItem, leftBaseAddress, rightBaseAddress, connectionSize, connectedItemTransferY, connectionTransfer, mapColumn);
                }

                //! After the connection, the effects of the remap address and remap range are done and can be cleared.
                remapValue.clear();
                remapRange.clear();

                if (currentLinkedItem != nullptr)
                {
                    previousItem = currentLinkedItem;
                }
            }
        }

        previousInterface = pathInterface;
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::createLocalLinkedItem()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::createLocalLinkedItem(
    QSharedPointer<LinkedGraphicItem> previousItem,
    QSharedPointer<ConnectivityInterface const> localMapInterface,
    QSharedPointer<QVector<MainMemoryGraphicsItem *>> placedItems,
    MemoryColumn* mapColumn)
{
    auto localMapGraphicsItem = getLocalMemoryMapItem(localMapInterface);
    if (localMapGraphicsItem->isVisible() == false)
    {
        localMapGraphicsItem->setVisible(true);
    }

    quint64 mapBaseAddress = localMapGraphicsItem->getOriginalBaseAddress();
    quint64 mapRange = localMapGraphicsItem->getOriginalLastAddress() - localMapGraphicsItem->getOriginalBaseAddress() + 1;

    qreal yTransfer = mapBaseAddress * MemoryDesignerConstants::RANGEINTERVAL;

    createLinkedItem(previousItem->children_, localMapGraphicsItem, mapBaseAddress, mapBaseAddress, mapRange, 0, yTransfer, placedItems, mapColumn);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::findOrCreateLinkedItem()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryConnectionHandler::LinkedGraphicItem> MemoryConnectionHandler::findOrCreateLinkedItem(
    QSharedPointer<QVector<MainMemoryGraphicsItem *>> placedItems, QSharedPointer<QVector<QSharedPointer<LinkedGraphicItem>>> roots,
    QSharedPointer<LinkedGraphicItem> previousItem, MainMemoryGraphicsItem* pathGraphicsItem,
    quint64 const& connectionLeftBaseAddress, quint64 const& connectionRightBaseAddress, quint64 const& connectionRange,
    qreal const& itemTransferY, qreal const& connectionTransferY,
    MemoryColumn* itemColumn)
{
    auto itemSet = roots;
    if (previousItem != nullptr)
    {
        itemSet = previousItem->children_;
    }

    for (auto linkedItem : *itemSet)
    {
        if (foundLinkedItem(linkedItem, pathGraphicsItem, connectionLeftBaseAddress, connectionRightBaseAddress))
        {
            if (linkedItem->connectionRanges_.contains(connectionRange) == false)
            {
                linkedItem->connectionRanges_.append(connectionRange);
            }

            return linkedItem;
        }
    }

    return createLinkedItem(itemSet, pathGraphicsItem, connectionLeftBaseAddress, connectionRightBaseAddress, connectionRange, itemTransferY, connectionTransferY, placedItems, itemColumn);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::foundLinkedItem()
//-----------------------------------------------------------------------------
bool MemoryConnectionHandler::foundLinkedItem(QSharedPointer<LinkedGraphicItem> linkedItem, MainMemoryGraphicsItem* pathGraphicsItem, quint64 const& connectionLeftBaseAddress, quint64 const& connectionRightBaseAddress) const
{
    if (linkedItem->item_ && linkedItem->connectionLeftBaseAddress_ == connectionLeftBaseAddress && linkedItem->connectionRightBaseAddress_ == connectionRightBaseAddress)
    {
        if (linkedItem->item_ == pathGraphicsItem ||
            (linkedItem->item_->name() == pathGraphicsItem->name() && linkedItem->item_->getContainingInstance() == pathGraphicsItem->getContainingInstance()))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::createLinkedItem()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryConnectionHandler::LinkedGraphicItem> MemoryConnectionHandler::createLinkedItem(
    QSharedPointer<QVector<QSharedPointer<LinkedGraphicItem>>> containingSet,
    MainMemoryGraphicsItem* pathGraphicsItem,
    quint64 const& connectionLeftBaseAddress,
    quint64 const& connectionRightBaseAddress,
    quint64 const& connectionRange,
    qreal const& connectedItemTransferY,
    qreal const& connectionTransferY,
    QSharedPointer<QVector<MainMemoryGraphicsItem *>> placedItems,
    MemoryColumn* itemColumn)
{
    QSharedPointer<LinkedGraphicItem> newLinkedItem(new LinkedGraphicItem());
    newLinkedItem->connectionLeftBaseAddress_ = connectionLeftBaseAddress;
    newLinkedItem->connectionRightBaseAddress_ = connectionRightBaseAddress;
    newLinkedItem->connectionRanges_.append(connectionRange);
    newLinkedItem->memoryItemTransferY_ = connectedItemTransferY;
    newLinkedItem->connectionTransferY_ = connectionTransferY;

    if (placedItems->contains(pathGraphicsItem))
    {
        pathGraphicsItem = itemHandler_->cloneMemoryItem(pathGraphicsItem, itemColumn);
    }
    
    newLinkedItem->item_ = pathGraphicsItem;
    placedItems->append(pathGraphicsItem);
    containingSet->append(newLinkedItem);
    return newLinkedItem;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::separateBrokenPathSet()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<MemoryConnectionHandler::ConnectedItemSet> > MemoryConnectionHandler::separateBrokenPathSet(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems) const
{
    QVector<QSharedPointer<MemoryConnectionHandler::ConnectedItemSet> > itemSets;

    QVector<MainMemoryGraphicsItem*> remainingSpaces;
    for (auto spaceItem : *placedSpaceItems)
    {
        remainingSpaces.append(spaceItem);
    }

    QSharedPointer<QVector<MainMemoryGraphicsItem*> > visitedItems(new QVector<MainMemoryGraphicsItem*>());

    while (remainingSpaces.isEmpty() == false)
    {
        auto spaceItem = remainingSpaces.takeFirst();
        if (visitedItems->contains(spaceItem) == false)
        {
            auto connectedItemSet = getConnectedItemsForSpaceItem(spaceItem, visitedItems);
            itemSets.append(connectedItemSet);
        }
    }

    return itemSets;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::getConnectedItemsForSpaceItem()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryConnectionHandler::ConnectedItemSet> MemoryConnectionHandler::getConnectedItemsForSpaceItem(MainMemoryGraphicsItem* spaceItem, QSharedPointer<QVector<MainMemoryGraphicsItem *>> visitedItems) const
{
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > connectedSpaces(new QVector<MainMemoryGraphicsItem*>());
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > connectedMaps(new QVector<MainMemoryGraphicsItem*>());

    QVector<MainMemoryGraphicsItem*> itemsConnectedToSpace;
    getConnectedItems(spaceItem, itemsConnectedToSpace, visitedItems);
    for (auto connectedItem : itemsConnectedToSpace)
    {
        if (connectedItem->type() == GraphicsItemTypes::GFX_TYPE_ADDRESS_SPACE_ITEM)
        {
            connectedSpaces->append(connectedItem);
        }
        else
        {
            connectedMaps->append(connectedItem);
        }
    }

    QSharedPointer<MemoryConnectionHandler::ConnectedItemSet> combinedConnectedItems(new MemoryConnectionHandler::ConnectedItemSet());
    combinedConnectedItems->connectedSpaces_ = connectedSpaces;
    combinedConnectedItems->connectedMaps_ = connectedMaps;

    return combinedConnectedItems;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::getConnectedItems()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::getConnectedItems(MainMemoryGraphicsItem* memoryItem, QVector<MainMemoryGraphicsItem*>& itemsConnectedToItem, QSharedPointer<QVector<MainMemoryGraphicsItem *>> visitedItems) const
{
    visitedItems->append(memoryItem);
    itemsConnectedToItem.append(memoryItem);
    QVector<MainMemoryGraphicsItem*> connectedItems;

    auto connections = memoryItem->getMemoryConnections();
    for (auto iterator = connections.cbegin(), end = connections.cend(); iterator != end; ++iterator)
    {
        auto currentConnection = iterator.value();

        if (auto connectionStartItem = currentConnection->getConnectionStartItem(); visitedItems->contains(connectionStartItem) == false)
        {
            getConnectedItems(connectionStartItem, itemsConnectedToItem, visitedItems);
        }
        if (auto connectionEndItem = currentConnection->getConnectionEndItem(); visitedItems->contains(connectionEndItem) == false)
        {
            getConnectedItems(connectionEndItem, itemsConnectedToItem, visitedItems);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::connectionExists()
//-----------------------------------------------------------------------------
bool MemoryConnectionHandler::connectionExists(MainMemoryGraphicsItem* startItem, MainMemoryGraphicsItem* endItem, quint64 const& connectionBaseAddress, quint64 const& connectionEndAddress) const
{
    QMultiMapIterator connectionIterator(startItem->getMemoryConnections());
    while (connectionIterator.hasNext())
    {
        connectionIterator.next();
        if (connectionIterator.key() > connectionBaseAddress)
        {
            return false;
        }

        if (connectionIterator.key() == connectionBaseAddress &&
            connectionIterator.value()->getConnectionEndItem() == endItem && connectionIterator.value()->getRangeEndValue() == connectionEndAddress)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::moveStartItemNotEndItem()
//-----------------------------------------------------------------------------
bool MemoryConnectionHandler::moveStartItemNotEndItem(MainMemoryGraphicsItem const* endItem,
    quint64 const& connectionBaseAddress) const
{
    return endItem->getMemoryConnections().isEmpty() == false && endItem->getBaseAddress() == connectionBaseAddress;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::createOnlyEndsConnectionFromLinks()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::createOnlyEndsConnectionFromLinks(
    QSharedPointer<QVector<QSharedPointer<LinkedGraphicItem>>> roots,
    QSharedPointer<QVector<MainMemoryGraphicsItem *>> placedSpaces,
    QSharedPointer<QVector<MainMemoryGraphicsItem *>> placedMaps,
    MemoryColumn* mapColumn)
{
    for (auto currentRoot : *roots)
    {
        if (currentRoot->item_ == nullptr)
        {
            continue;
        }

        auto maximumRange = *std::max_element(currentRoot->connectionRanges_.begin(), currentRoot->connectionRanges_.end());

        auto leftLastAddress = currentRoot->connectionLeftBaseAddress_ + maximumRange - 1;

        createOnlyEndsConnectionsForRoot(currentRoot, currentRoot, placedSpaces, placedMaps, mapColumn, currentRoot->connectionLeftBaseAddress_, 0, leftLastAddress);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::createOnlyEndsConnectionsForRoot()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::createOnlyEndsConnectionsForRoot(QSharedPointer<LinkedGraphicItem> root,
    QSharedPointer<LinkedGraphicItem> currentLinkItem,
    QSharedPointer<QVector<MainMemoryGraphicsItem *>> placedSpaces,
    QSharedPointer<QVector<MainMemoryGraphicsItem *>> placedMaps,
    MemoryColumn* mapColumn,
    quint64 const& maximumLeftBaseAddress,
    quint64 const& rightBaseAddressChange,
    quint64 const& minimumLeftLastAddress)
{
    auto newRightBaseAddressChange = rightBaseAddressChange;

    //! Not in leaf
    if (currentLinkItem->children_->isEmpty() == false)
    {
        for (auto childItem : *currentLinkItem->children_)
        {
            //! Ignore connections that are beyond the root item.
            if (childItem->connectionLeftBaseAddress_ > currentLinkItem->item_->getOriginalLastAddress() + maximumLeftBaseAddress)
            {
                continue;
            }

            auto childItemMaximumLeftBaseAddress = maximumLeftBaseAddress;

            quint64 baseAddressDifference = 0;
            quint64 childRightBaseAddressChange = 0;
            quint64 availableRange = 0;

            //! Create only the largest available connection
            auto maximumChildItemRange = *std::max_element(childItem->connectionRanges_.begin(), childItem->connectionRanges_.end());
            if (maximumChildItemRange < 1)
            {
                continue;
            }

            //! If the connected item is connected to another item and this new connection is in different base address
            if (childItem->connectionLeftBaseAddress_ > currentLinkItem->connectionRightBaseAddress_)
            {
                //! Change the connection left base address
                baseAddressDifference = childItem->connectionLeftBaseAddress_ - currentLinkItem->connectionRightBaseAddress_;
                childItemMaximumLeftBaseAddress += baseAddressDifference;
            }
            else if (currentLinkItem->connectionRightBaseAddress_ > childItem->connectionLeftBaseAddress_)
            {
                //! Change the connection right base address
                baseAddressDifference = currentLinkItem->connectionRightBaseAddress_ - childItem->connectionLeftBaseAddress_;
                childRightBaseAddressChange = baseAddressDifference;
            }

            //! If the right base address change is beyond the connection area, discard and continue to the next connected item.
            if (childRightBaseAddressChange > childItemMaximumLeftBaseAddress + maximumChildItemRange - 1)
            {
                continue;
            }

            auto childItemMinimumLeftLastAddress = childItemMaximumLeftBaseAddress + maximumChildItemRange - 1 - childRightBaseAddressChange;
            childItemMinimumLeftLastAddress = qMin(childItemMinimumLeftLastAddress, minimumLeftLastAddress);

            if (childItemMaximumLeftBaseAddress > childItemMinimumLeftLastAddress)
            {
                continue;
            }
            
            //! Continue along to the next item
            createOnlyEndsConnectionsForRoot(
                root, childItem,
                placedSpaces, placedMaps,
                mapColumn,
                childItemMaximumLeftBaseAddress, newRightBaseAddressChange + childRightBaseAddressChange, childItemMinimumLeftLastAddress);
        }
    }
    //! In leaf
    else if (currentLinkItem->item_->type() == GFX_TYPE_MEMORY_ITEM)
    {
        //! A connection can be created between a root item and a memory item at the end of the connection path.
        if (root->item_->type() == GFX_TYPE_ADDRESS_SPACE_ITEM && placedSpaces->contains(root->item_) == false)
        {
            placedSpaces->append(root->item_);
        }

        auto newConnectionTransferY = maximumLeftBaseAddress * MemoryDesignerConstants::RANGEINTERVAL;
        auto minimumRange = minimumLeftLastAddress - maximumLeftBaseAddress + 1;

        auto newMaximumRightBaseAddress = currentLinkItem->connectionRightBaseAddress_ + newRightBaseAddressChange;
        auto itemTransfer = newConnectionTransferY - (newMaximumRightBaseAddress - currentLinkItem->connectionLeftBaseAddress_) * MemoryDesignerConstants::RANGEINTERVAL;

        placeMemoryMap(currentLinkItem->item_, root->item_, itemTransfer, placedMaps, mapColumn);

        placedMaps->append(currentLinkItem->item_);

        createConnectionItem(root->item_, currentLinkItem->item_, maximumLeftBaseAddress, newMaximumRightBaseAddress, currentLinkItem->connectionLeftBaseAddress_, minimumRange, newConnectionTransferY);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::createFullConnectionFromLinks()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::createFullConnectionFromLinks(
    QSharedPointer<QVector<QSharedPointer<LinkedGraphicItem>>> roots,
    QSharedPointer<QVector<MainMemoryGraphicsItem *>> placedSpaces,
    QSharedPointer<QVector<MainMemoryGraphicsItem *>> placedMaps,
    MemoryColumn* mapColumn)
{
    for (auto currentRoot : *roots)
    {
        if (currentRoot->item_ == nullptr)
        {
            continue;
        }
        else if (currentRoot->item_->type() == GFX_TYPE_ADDRESS_SPACE_ITEM)
        {
            placedSpaces->append(currentRoot->item_);
        }

        createConnectionsForLinkedItem(currentRoot, placedSpaces, placedMaps, mapColumn);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::createConnectionsForLinkedItem()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::createConnectionsForLinkedItem(
    QSharedPointer<LinkedGraphicItem> linkedItem,
    QSharedPointer<QVector<MainMemoryGraphicsItem *>> placedSpaces,
    QSharedPointer<QVector<MainMemoryGraphicsItem *>> placedMaps,
    MemoryColumn* mapColumn)
{
    for (auto child : *linkedItem->children_)
    {
        MainMemoryGraphicsItem* childItem = child->item_;

        qreal itemTransferY = child->memoryItemTransferY_ + child->connectionTransferY_;

        if (childItem->type() == GFX_TYPE_ADDRESS_SPACE_ITEM)
        {
            placeAddressSpace(childItem, linkedItem->item_, itemTransferY, placedSpaces);
        }
        else
        {
            placeMemoryMap(childItem, linkedItem->item_, itemTransferY, placedMaps, mapColumn);
            placedMaps->append(child->item_);
        }

        for (auto connectionRange : child->connectionRanges_)
        {
            createConnectionItem(linkedItem->item_, childItem,
                child->connectionLeftBaseAddress_, child->connectionRightBaseAddress_, child->connectionLeftBaseAddress_, connectionRange,
                child->connectionTransferY_);
        }

        createConnectionsForLinkedItem(child, placedSpaces, placedMaps, mapColumn);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::getPathGraphicsItem()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryConnectionHandler::getPathGraphicsItem(QSharedPointer<const ConnectivityInterface> pathInterface) const
{
    if (!pathInterface)
    {
        return nullptr;
    }

    QString columnType = "";
    if (QSharedPointer<MemoryItem const> memoryItem = pathInterface->getConnectedMemory(); memoryItem->getType() == MemoryDesignerConstants::ADDRESSSPACE_TYPE)
    {
        columnType = MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME;
    }
    else if (memoryItem->getType() == MemoryDesignerConstants::MEMORYMAP_TYPE)
    {
        columnType = MemoryDesignerConstants::MEMORYMAPCOLUMNCOMMON_NAME;
    }

    MainMemoryGraphicsItem* graphicsItem = nullptr;
    if (!columnType.isEmpty())
    {
        graphicsItem = getMainGraphicsItem(pathInterface, columnType);
    }

    return graphicsItem;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::getMainGraphicsItem()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryConnectionHandler::getMainGraphicsItem(
    QSharedPointer<ConnectivityInterface const> connectionInterface, QString const& columnType) const
{
    MainMemoryGraphicsItem* graphicsItem = nullptr;

    QSharedPointer<MemoryItem const> memoryItem = connectionInterface->getConnectedMemory();
    if (QSharedPointer<ConnectivityComponent const> connectionInstance = connectionInterface->getInstance(); memoryItem && connectionInstance)
    {
        for (auto currentColumn : columnHandler_->getSpecifiedColumns(columnType))
        {
            graphicsItem = currentColumn->findGraphicsItemByMemoryItem(memoryItem);
            if (graphicsItem)
            {
                break;
            }
        }
    }

    return graphicsItem;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::getFirstInitiatorInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectivityInterface const> MemoryConnectionHandler::getFirstInitiatorInterface(QVector<QSharedPointer<ConnectivityInterface const> > connectionPath) const
{
    for (auto pathInterface : connectionPath)
    {
        if ((pathInterface->getMode() == General::MASTER || pathInterface->getMode() == General::INITIATOR) && pathInterface->getConnectedMemory())
        {
            return pathInterface;
        }
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::getConnectionEndItem()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryConnectionHandler::getConnectionEndItem(
    QSharedPointer<ConnectivityInterface const> startInterface,
    QSharedPointer<ConnectivityInterface const> endInterface) const
{
    MainMemoryGraphicsItem* connectionEndItem(nullptr);

    if (startInterface == endInterface)
    {
        connectionEndItem = getLocalMemoryMapItem(endInterface);
    }
    else
    {
        connectionEndItem =
            getMainGraphicsItem(endInterface, MemoryDesignerConstants::MEMORYMAPCOLUMNCOMMON_NAME);
    }

    return connectionEndItem;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::getLocalMemoryMapItem()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryConnectionHandler::getLocalMemoryMapItem(
    QSharedPointer<ConnectivityInterface const> spaceInterface) const
{
    MainMemoryGraphicsItem* localMapItem = nullptr;

    if (auto memoryItemForLocalMap = MemoryDesignerConstants::getMemoryItemForLocalMap(spaceInterface); memoryItemForLocalMap)
    {
        for (MemoryColumn * currentColumn : columnHandler_->getSpecifiedColumns(MemoryDesignerConstants::MEMORYMAPCOLUMNCOMMON_NAME))
        {
            localMapItem = currentColumn->findGraphicsItemByMemoryItem(memoryItemForLocalMap);
            if (localMapItem)
            {
                break;
            }
        }
    }

    return localMapItem;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::placeAddressSpace()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::placeAddressSpace(MainMemoryGraphicsItem* spaceItem, MainMemoryGraphicsItem* startItem, qreal const& yTransfer, QSharedPointer<QVector<MainMemoryGraphicsItem *>> placedSpaces)
{
    auto originalColumn = dynamic_cast<MemoryColumn*>(spaceItem->parentItem());
    if (!originalColumn)
    {
        return;
    }

    auto completeTransferValue = yTransfer + startItem->scenePos().y();

    positionSpaceItem(spaceItem, completeTransferValue);

    originalColumn->removeItem(spaceItem);

    auto spaceColumns = columnHandler_->getAddressSpaceColumns();
    auto newColumnIndex = spaceColumns.indexOf(originalColumn) + 1;
    if (newColumnIndex < spaceColumns.size())
    {
        auto indexedColumn = spaceColumns.at(newColumnIndex);
        indexedColumn->addItem(spaceItem);
        if (itemCollidesWithOtherPlacedItems(spaceItem, placedSpaces))
        {
            placeCompressedSpaceToColumn(spaceItem, placedSpaces);
        }
    }
    else
    {
        MemoryColumn* newSpaceColumn = columnHandler_->createAddressSpaceColumn();
        newSpaceColumn->addItem(spaceItem);
    }

    placedSpaces->append(spaceItem);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::placeMemoryMap()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::placeMemoryMap(MainMemoryGraphicsItem* mapItem,
    MainMemoryGraphicsItem* startItem,
    qreal const& yTransfer,
    QSharedPointer<QVector<MainMemoryGraphicsItem *>> placedMapItems,
    MemoryColumn* mapColumn)
{
    mapItem->setY(startItem->pos().y() + yTransfer);

    if (!placedMapItems->isEmpty())
    {
        checkMemoryMapRepositionToOverlapColumn(placedMapItems, mapItem, mapColumn, startItem);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::checkMemoryMapRepositionToOverlapColumn()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::checkMemoryMapRepositionToOverlapColumn(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMaps,
    MainMemoryGraphicsItem* memoryItem, MemoryColumn* originalColumn,
    MainMemoryGraphicsItem* connectionStartItem)
{
    QRectF selectedItemRect = memoryItem->getSceneRectangleWithSubItems();

    int selectedItemPenWidth = memoryItem->pen().width();

    QVector<MainMemoryGraphicsItem*> connectedSpaceItems;
    connectedSpaceItems.append(connectionStartItem);
    for (MainMemoryGraphicsItem * chainedSpaceItem : connectionStartItem->getChainedSpaceItems())
    {
        connectedSpaceItems.append(chainedSpaceItem);
    }

    if (originalColumn->memoryMapOverlapsInColumn(memoryItem, selectedItemRect, selectedItemPenWidth, connectedSpaceItems, placedMaps))
    {
        for (MemoryColumn * memoryColumn : columnHandler_->getMapOverlapColumns())
        {
            selectedItemRect.setX(selectedItemRect.x() + memoryColumn->boundingRect().width());

            if (!memoryColumn->memoryMapOverlapsInColumn(memoryItem, selectedItemRect, selectedItemPenWidth, connectedSpaceItems, placedMaps))
            {
                originalColumn->removeItem(memoryItem);
                memoryColumn->addItem(memoryItem);
                return;
            }
        }

        auto overlapColumn = columnHandler_->createMemoryOverlapColumn();
        originalColumn->removeItem(memoryItem);
        overlapColumn->addItem(memoryItem);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::positionSpaceItem()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::positionSpaceItem(MainMemoryGraphicsItem* spaceItem, qreal ypos)
{
    spaceItem->setY(ypos);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::createConnectionItem()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::createConnectionItem(MainMemoryGraphicsItem* startItem, MainMemoryGraphicsItem* endItem,
    quint64 const& leftBaseAddress, quint64 const& rightBaseAddress, quint64 const& endItemBaseAddress,
    quint64 const& connectionRange,
    qreal const& yTransfer)
{
    changeConnectionEndItemRanges(endItem, endItemBaseAddress);

    quint64 leftEndAddress = leftBaseAddress + connectionRange - 1;
    quint64 rightEndAddress = rightBaseAddress + connectionRange - 1;

    auto newConnectionItem = new MemoryConnectionItem(startItem, leftBaseAddress, leftEndAddress, rightBaseAddress, rightEndAddress, endItem, startItem->scene(), yTransfer);
    connectionsToMemoryMaps_.append(newConnectionItem);

//     startItem->hideCollidingRangeLabels(remappedAddress, remappedEndAddress);
//     endItem->hideCollidingRangeLabels(remappedAddress, remappedEndAddress);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::changeConnectionEndItemRanges()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::changeConnectionEndItemRanges(MainMemoryGraphicsItem* connectionEndItem,
    quint64 const& remappedAddress) const
{
    connectionEndItem->changeAddress(remappedAddress);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::getHighestPlacedItemInSet()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryConnectionHandler::getHighestPlacedItemInSet(MainMemoryGraphicsItem* currentHighestItem, QSharedPointer<QVector<MainMemoryGraphicsItem*> > itemSet) const
{
    MainMemoryGraphicsItem* highestItem = currentHighestItem;
    for (auto comparisonItem : *itemSet)
    {
        if (comparisonItem->scenePos().y() < highestItem->scenePos().y())
        {
            highestItem = comparisonItem;
        }
    }

    return highestItem;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::compressConnectedMemoryItems()
//-----------------------------------------------------------------------------
QVector<MemoryConnectionItem*> MemoryConnectionHandler::compressConnectedMemoryItems(QSharedPointer<QVector<MainMemoryGraphicsItem*>> placedSpaceItems, QSharedPointer<QVector<MainMemoryGraphicsItem*>> placedMapItems) const
{
    MainMemoryGraphicsItem* masterItem = placedSpaceItems->first();
    masterItem = getHighestPlacedItemInSet(masterItem, placedSpaceItems);
    masterItem = getHighestPlacedItemInSet(masterItem, placedMapItems);

    QVector<MainMemoryGraphicsItem*> visitedItems;
    QVector<MemoryConnectionItem*> visitedConnections;
    QVector<qreal> uncutCoordinates = masterItem->getUncutCoordinatesFromSet(visitedItems, visitedConnections);
    std::sort(uncutCoordinates.begin(), uncutCoordinates.end());

    visitedItems.clear();
    for (auto connectionItem : visitedConnections)
    {
        connectionItem->compressToUnCutCoordinates(visitedItems, uncutCoordinates, MemoryDesignerConstants::CUTMODIFIER, condenseMemoryItems_);
    }

    QMap<qreal, qreal> areaMovements = calculateAreaMovements(uncutCoordinates, MemoryDesignerConstants::CUTMODIFIER);
    for (auto memoryItem : visitedItems)
    {
        qreal movementY = -areaMovements.value(memoryItem->sceneBoundingRect().top());
        memoryItem->moveBy(0, movementY);
    }

    for (auto connectionItem : visitedConnections)
    {
        qreal movementY = -areaMovements.value(connectionItem->sceneBoundingRect().top());
        connectionItem->moveItemBy(movementY);
    }

    for (auto memoryItem : visitedItems)
    {
        memoryItem->extendMemoryItem();
    }

    return visitedConnections;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::calculateAreaMovements()
//-----------------------------------------------------------------------------
QMap<qreal, qreal> MemoryConnectionHandler::calculateAreaMovements(QVector<qreal> uncutCoordinates, qreal const& CUTMODIFIER) const
{
    QMap<qreal, qreal> areaMovements;

    qreal areaTop = uncutCoordinates.takeFirst();
    qreal areaBottom = areaTop;
    qreal combinedTransferValue = 0;

    while (!uncutCoordinates.isEmpty())
    {
        areaBottom = uncutCoordinates.takeFirst();
        qreal singleCut = areaBottom - areaTop;
        if (condenseMemoryItems_)
        {
            singleCut = singleCut - CUTMODIFIER;
        }
        else
        {
            qreal requiredArea = MemoryDesignerConstants::getRequiredAreaForUsedArea(singleCut);
            singleCut = singleCut - requiredArea;
        }

        if (singleCut > 0)
        {
            combinedTransferValue += singleCut;
        }

        areaMovements.insert(areaBottom, combinedTransferValue);

        areaTop = areaBottom;
    }

    return areaMovements;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::repositionCompressedAddressSpaces()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::repositionCompressedAddressSpaces(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems)
{
/*
    auto spaceIter = std::next(placedSpaceItems->begin());
    while (spaceIter != placedSpaceItems->end())
    {
        //auto spaceItem = spaceIterator.previous();
        auto spaceItem = *spaceIter;
        if (itemCollidesWithOtherPlacedItems(spaceItem, placedSpaceItems))
        {
            placeCompressedSpaceToColumn(spaceItem, placedSpaceItems);
        }
        spaceIter = std::next(spaceIter);
    }
*/
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::itemCollidesWithOtherPlacedItems()
//-----------------------------------------------------------------------------
bool MemoryConnectionHandler::itemCollidesWithOtherPlacedItems(MainMemoryGraphicsItem const* memoryItem, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedItems) const
{
    for (auto collidingItem : memoryItem->collidingItems(Qt::IntersectsItemShape))
    {
        auto collidingMemoryItem = dynamic_cast<MainMemoryGraphicsItem*>(collidingItem);
        auto collidingExtensionItem = dynamic_cast<MemoryExtensionGraphicsItem*>(collidingItem);
        if ((collidingMemoryItem && placedItems->contains(collidingItem)) ||
            (collidingExtensionItem && placedItems->contains(collidingExtensionItem->parentItem())))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::placeCompressedSpaceToColumn()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::placeCompressedSpaceToColumn(MainMemoryGraphicsItem* spaceItem, QSharedPointer<QVector<MainMemoryGraphicsItem *>> placedSpaceItems)
{
    auto originalColumn = dynamic_cast<MemoryColumn*>(spaceItem->parentItem());
    if (!originalColumn)
    {
        return;
    }

    QRectF spaceRectangle = spaceItem->getSceneRectangleWithSubItems();
    int spaceLineWidth = spaceItem->pen().width();
    if (spaceItem->hasExtensionItem())
    {
        spaceRectangle.setHeight(spaceRectangle.height() + spaceItem->getExtensionItem()->boundingRect().height());
    }

    QVectorIterator<MemoryColumn*> columnIterator(columnHandler_->getAddressSpaceColumns());
    while (columnIterator.hasNext())
    {
        MemoryColumn* comparisonColumn = columnIterator.next();
        if (!comparisonColumn->itemOverlapsAnotherPlacedColumnItem(spaceItem, spaceRectangle, spaceLineWidth, placedSpaceItems))
        {
            if (originalColumn != comparisonColumn)
            {
                originalColumn->removeItem(spaceItem);
                comparisonColumn->addItem(spaceItem);
            }

            return;
        }
    }

    originalColumn->removeItem(spaceItem);
    MemoryColumn* newSpaceColumn = columnHandler_->createAddressSpaceColumn();
    newSpaceColumn->addItem(spaceItem);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::repositionCompressedMemoryMaps()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::repositionCompressedMemoryMaps(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems, MemoryColumn const* memoryMapColumn)
{
    for (MainMemoryGraphicsItem * mapItem : *placedMapItems)
    {
        QGraphicsItem* mapParentItem = mapItem->parentItem();
        MemoryColumn* originalColumn = dynamic_cast<MemoryColumn*>(mapParentItem);
        if (originalColumn && originalColumn != memoryMapColumn)
        {
            QRectF mapRectangle = mapItem->sceneBoundingRect();
            int mapPenWidth = mapItem->pen().width();

            qreal itemHeight = mapRectangle.height();
            qreal extensionHeight = 0;

            if (mapItem->hasExtensionItem())
            {
                extensionHeight = mapItem->getExtensionItem()->boundingRect().height();
            }

            if (qreal subItemsHeight = mapItem->getSubItemHeightAddition(); subItemsHeight > extensionHeight)
            {
                extensionHeight = subItemsHeight;
            }

            mapRectangle.setHeight(itemHeight + extensionHeight);

            QVector<MainMemoryGraphicsItem*> connectedSpaceItems = mapItem->getChainedSpaceItems();

            int columnWidth = originalColumn->sceneBoundingRect().width();

            QPointF columnPoint(originalColumn->pos().x() - columnWidth, mapRectangle.y());
            MemoryColumn* comparisonColumn = columnHandler_->findColumnAtPosition(columnPoint);
            if (comparisonColumn)
            {
                while (comparisonColumn && !comparisonColumn->name().contains(
                    MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME, Qt::CaseInsensitive))
                {
                    mapRectangle.setX(mapRectangle.x() - columnWidth);

                    if (!comparisonColumn->memoryMapOverlapsInColumn(mapItem, mapRectangle, mapPenWidth, connectedSpaceItems, placedMapItems))
                    {
                        originalColumn->removeItem(mapItem);
                        comparisonColumn->addItem(mapItem, true);

                        if (originalColumn->getItems().isEmpty() && originalColumn->name().contains(
                            MemoryDesignerConstants::MEMORYMAPOVERLAPCOLUMN_NAME, Qt::CaseInsensitive))
                        {
                            columnHandler_->removeColumnFromLayout(originalColumn);
                        }

                        break;
                    }

                    columnPoint.setX(columnPoint.x() - columnWidth);
                    comparisonColumn = columnHandler_->findColumnAtPosition(columnPoint);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::redrawMemoryConnections()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::redrawMemoryConnections(QVector<MemoryConnectionItem*> connectionItems) const
{
    for (auto connection : connectionItems)
    {
        connection->reDrawConnection();
        connection->repositionCollidingRangeLabels();
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::createOverlappingConnectionMarkers()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::createOverlappingConnectionMarkers(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems)
{
    for (MainMemoryGraphicsItem * spaceItem : *placedSpaceItems)
    {
        spaceItem->createOverlappingConnectionMarkers();

        for (MemoryCollisionItem * collisionItem : spaceItem->getMemoryCollisions())
        {
            memoryCollisions_.append(collisionItem);
        }
    }
}
