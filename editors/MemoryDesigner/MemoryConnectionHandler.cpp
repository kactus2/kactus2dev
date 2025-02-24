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
#include <editors/MemoryDesigner/ConnectivityInterface.h>
#include <editors/MemoryDesigner/MemoryDesignerConstants.h>
#include <editors/MemoryDesigner/MainMemoryGraphicsItem.h>
#include <editors/MemoryDesigner/MemoryExtensionGraphicsItem.h>
#include <editors/MemoryDesigner/MemoryColumn.h>
#include <editors/MemoryDesigner/MemoryConnectionItem.h>
#include <editors/MemoryDesigner/MemoryCollisionItem.h>
#include <editors/MemoryDesigner/MemoryColumnHandler.h>
#include <editors/MemoryDesigner/MemoryItem.h>

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::MemoryConnectionHandler()
//-----------------------------------------------------------------------------
MemoryConnectionHandler::MemoryConnectionHandler(QSharedPointer<MemoryColumnHandler> columnHandler):
condenseMemoryItems_(true),
filterAddressSpaceChains_(true),
connectionsToMemoryMaps_(),
memoryCollisions_(),
columnHandler_(columnHandler)
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
    MasterSlavePathSearch pathSearcher;
    QVector<Path> masterSlavePaths = pathSearcher.findMasterSlavePaths(connectionGraph, false);
    if (masterSlavePaths.isEmpty())
    {
        return;
    }

    QVector<QSharedPointer<QVector<Path> > > pathSets = findPathSets(masterSlavePaths);

    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems(
        new QVector<MainMemoryGraphicsItem*>());
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems(
        new QVector<MainMemoryGraphicsItem*>());
    QVector<MemoryConnectionItem*> connections;
    
    qreal spaceYPlacement = MemoryDesignerConstants::SPACEITEMINTERVAL;

    for (auto const& singleSet : pathSets)
    {
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItemsInSet(new QVector<MainMemoryGraphicsItem*>());
        QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItemsInSet(new QVector<MainMemoryGraphicsItem*>());

        connections.append(createConnectionSet(singleSet, placedMapItemsInSet, memoryMapColumn, spaceYPlacement, placedSpaceItemsInSet, spaceColumn));

        placedMapItems->append(*placedMapItemsInSet);
        placedSpaceItems->append(*placedSpaceItemsInSet);

        // Reposition items in set
        repositionCompressedAddressSpaces(placedSpaceItemsInSet);
        repositionCompressedMemoryMaps(placedMapItemsInSet, memoryMapColumn);
    }

    spaceColumn->compressUnconnectedMemoryItems(condenseMemoryItems_, placedSpaceItems);
    memoryMapColumn->compressUnconnectedMemoryItems(condenseMemoryItems_, placedMapItems);

    spaceColumn->moveUnconnectedMemoryItems(placedSpaceItems);
    memoryMapColumn->moveUnconnectedMemoryItems(placedMapItems);

    redrawMemoryConnections(connections);
    createOverlappingConnectionMarkers(placedSpaceItems);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::findPathSets()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<QVector<MemoryConnectionHandler::Path> > > MemoryConnectionHandler::findPathSets(QVector<Path> masterSlavePaths) const
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
QQueue<int> MemoryConnectionHandler::findPathSetIndexes(Path currentPath, QVector<QSharedPointer<QVector<Path> > > pathSets) const
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
bool MemoryConnectionHandler::pathIsContainedWithPathSet(Path currentPath, QSharedPointer<QVector<Path>> pathSet) const
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
bool MemoryConnectionHandler::interfacedItemIsWithinPath(QSharedPointer<const ConnectivityInterface> pathInterface, MemoryConnectionHandler::Path comparisonPath) const
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
    qreal& spaceYPlacement,
    QSharedPointer<QVector<MainMemoryGraphicsItem*>> placedSpaceItems,
    MemoryColumn* spaceColumn)
{
    for (auto const& connectionPath : *pathSet)
    {
        createConnectionFromSet(connectionPath, placedMapItems, memoryMapColumn, placedSpaceItems, spaceColumn, spaceYPlacement);
    }

    if (placedSpaceItems->isEmpty())
    {
        return QVector<MemoryConnectionItem*>();
    }

    auto highestItemInSet = placedSpaceItems->first();
    highestItemInSet = getHighestPlacedItemInSet(highestItemInSet, placedSpaceItems);
    highestItemInSet = getHighestPlacedItemInSet(highestItemInSet, placedMapItems);

    auto yMovement = spaceYPlacement - highestItemInSet->scenePos().y();
    
    // Move entire group of connections to sensible vertical position.
    // Round movement up to integer, but std::ceil instead of qCeil to preserve type.
    highestItemInSet->moveItemAndConnectedItems(std::ceil(yMovement));

    auto connections = compressConnectedMemoryItems(placedSpaceItems, placedMapItems);

    spaceYPlacement = highestItemInSet->getLowestPointOfConnectedItems() + MemoryDesignerConstants::SPACEITEMINTERVAL;

    return connections;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::createConnectionFromSet()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::createConnectionFromSet(Path const& connectionPath,
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
    MemoryColumn* memoryMapColumn,
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
    MemoryColumn* spaceColumn,
    qreal initialPlacementOffset)
{
    QSharedPointer<ConnectivityInterface const> startInterface = getStartInterface(connectionPath);
    QSharedPointer<ConnectivityInterface const> endInterface = connectionPath.last();

    auto connectionStartItem = getMainGraphicsItem(startInterface, MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME);
    auto connectionEndItem = getConnectionEndItem(startInterface, endInterface);

    if (!connectionStartItem || !connectionEndItem)
    {
        return;
    }

    if (connectionStartItem->isVisible() == false)
    {
        connectionStartItem->setVisible(true);
    }
    if (connectionEndItem->isVisible() == false)
    {
        connectionEndItem->setVisible(true);
    }

    // Path variables also contain address space chain info
    auto pathVariables = MemoryConnectionAddressCalculator::calculatePathAddresses(startInterface, endInterface, connectionPath);
    auto yTransfer = getConnectionInitialTransferY(pathVariables.baseAddressNumber_,
        pathVariables.mirroredSlaveAddressChange_, pathVariables.hasRemapRange_,
        pathVariables.memoryMapBaseAddress_, pathVariables.spaceChainBaseAddress_);

    MainMemoryGraphicsItem* lastPlacedItem = connectionStartItem;

    if (placedSpaceItems->contains(connectionStartItem) == false)
    {
        placedSpaceItems->prepend(connectionStartItem);
    }

    // If end item has already been placed -> shared endpoint with other path -> set position of start item accordingly
    if (placedMapItems->contains(connectionEndItem))
    {
        positionSpaceItem(connectionStartItem, connectionEndItem->scenePos().y() - yTransfer);
    }

    if (!filterAddressSpaceChains_)
    {
        // Start placing address spaces from left to right
        for (auto spaceIt = std::next(pathVariables.spaceChain_.begin()); spaceIt != pathVariables.spaceChain_.end(); ++spaceIt)
        {
            auto prevSpaceIt = std::prev(spaceIt);

            auto spaceItem = getMainGraphicsItem(spaceIt->spaceInterface_, MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME);
            auto prevSpaceItem = getMainGraphicsItem(prevSpaceIt->spaceInterface_, MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME);

            if (placedSpaceItems->contains(spaceItem) == false)
            {
                placedSpaceItems->append(spaceItem);
            }

            positionSpaceItem(spaceItem, prevSpaceItem->scenePos().y() + prevSpaceIt->spaceInterface_->getBaseAddress().toULongLong() * MemoryDesignerConstants::RANGEINTERVAL);

            auto prevSpaceBaseAddr = prevSpaceIt->spaceInterface_->getBaseAddress().toULongLong();
            auto connectionYOffset = prevSpaceBaseAddr * MemoryDesignerConstants::RANGEINTERVAL;

            createConnectionItem(prevSpaceItem, spaceItem, prevSpaceBaseAddr,
                prevSpaceBaseAddr + spaceItem->getLastAddress(), pathVariables.memoryMapBaseAddress_, 
                pathVariables.hasRemapRange_, connectionYOffset);

            lastPlacedItem = spaceItem;
            yTransfer = spaceIt->spaceInterface_->getBaseAddress().toULongLong() * MemoryDesignerConstants::RANGEINTERVAL;
        }

        if (placedMapItems->contains(connectionEndItem) == false)
        {
            if (placedSpaceItems->contains(connectionStartItem) == false)
            {
                placedSpaceItems->append(connectionStartItem);
            }

            placeMemoryMap(connectionEndItem, lastPlacedItem, yTransfer, pathVariables, placedMapItems, memoryMapColumn);
            placedMapItems->append(connectionEndItem);
        }

        createConnectionItem(lastPlacedItem, connectionEndItem,
            pathVariables.remappedAddress_, pathVariables.remappedEndAddress_, pathVariables.memoryMapBaseAddress_,
            pathVariables.hasRemapRange_, yTransfer);
    }
    else
    {
        if (placedMapItems->contains(connectionEndItem) == false)
        {
            if (placedSpaceItems->contains(connectionStartItem) == false)
            {
                placedSpaceItems->append(connectionStartItem);
            }

            placeMemoryMap(connectionEndItem, connectionStartItem, yTransfer, pathVariables, placedMapItems, memoryMapColumn);
            placedMapItems->append(connectionEndItem);
        }
        else if (placedSpaceItems->contains(connectionStartItem) == false)
        {
            positionSpaceItem(
                connectionStartItem, connectionEndItem->scenePos().y() - yTransfer);

            placedSpaceItems->append(connectionStartItem);
        }

        lastPlacedItem = connectionStartItem;

        // Create connection between last space and memory map
        createConnectionItem(lastPlacedItem, connectionEndItem,
            pathVariables.remappedAddress_, pathVariables.remappedEndAddress_, pathVariables.memoryMapBaseAddress_,
            pathVariables.hasRemapRange_, yTransfer);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::getMainGraphicsItem()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryConnectionHandler::getMainGraphicsItem(
    QSharedPointer<ConnectivityInterface const> connectionInterface, QString columnType) const
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
// Function: MemoryConnectionHandler::getStartInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectivityInterface const> MemoryConnectionHandler::getStartInterface(
    QVector<QSharedPointer<ConnectivityInterface const> > connectionPath)
{
    for (auto pathInterface : connectionPath)
    {
        if ((pathInterface->getMode() == General::MASTER || pathInterface->getMode() == General::INITIATOR) && pathInterface->getConnectedMemory())
        {
            return pathInterface;
        }
    }

    return connectionPath.first();
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
// Function: MemoryConnectionHandler::getConnectionInitialTransferY()
//-----------------------------------------------------------------------------
qreal MemoryConnectionHandler::getConnectionInitialTransferY(quint64 baseAddressNumber,
    quint64 mirroredSlaveAddressChange, bool hasRemapRange, quint64 memoryMapBaseAddress,
    quint64 spaceChainConnectionBaseAddress) const
{
    qreal yTransfer = baseAddressNumber + mirroredSlaveAddressChange;

    if (!hasRemapRange)
    {
        yTransfer += memoryMapBaseAddress;
    }

    yTransfer += spaceChainConnectionBaseAddress;
    yTransfer = yTransfer * MemoryDesignerConstants::RANGEINTERVAL;

    return yTransfer;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::placeMemoryMap()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::placeMemoryMap(MainMemoryGraphicsItem* mapItem,
    MainMemoryGraphicsItem* startItem,
    qreal yTransfer,
    MemoryConnectionAddressCalculator::ConnectionPathVariables const& pathVariables,
    QSharedPointer<QVector<MainMemoryGraphicsItem*>> placedMapItems,
    MemoryColumn* mapColumn)
{
    qreal memoryMapYTransfer = yTransfer;
    if (pathVariables.hasRemapRange_)
    {
        memoryMapYTransfer += (pathVariables.memoryMapBaseAddress_ * MemoryDesignerConstants::RANGEINTERVAL);
    }

    qreal newEndItemPositionY = startItem->pos().y() + memoryMapYTransfer;
    mapItem->setY(newEndItemPositionY);

    if (!placedMapItems->isEmpty())
    {
        checkMemoryMapRepositionToOverlapColumn(
            pathVariables.remappedAddress_, pathVariables.remappedEndAddress_, placedMapItems, mapItem, mapColumn, startItem);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::checkMemoryMapRepositionToOverlapColumn()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::checkMemoryMapRepositionToOverlapColumn(quint64 connectionBaseAddress,
    quint64 connectionLastAddress, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMaps,
    MainMemoryGraphicsItem* memoryItem, MemoryColumn* originalColumn, MainMemoryGraphicsItem* connectionStartItem)
{
    QRectF selectedItemRect = memoryItem->getSceneRectangleWithSubItems();

    int selectedItemPenWidth = memoryItem->pen().width();

    QVector<MainMemoryGraphicsItem*> connectedSpaceItems;
    connectedSpaceItems.append(connectionStartItem);
    for (MainMemoryGraphicsItem * chainedSpaceItem : connectionStartItem->getChainedSpaceItems())
    {
        connectedSpaceItems.append(chainedSpaceItem);
    }

    if (originalColumn->memoryMapOverlapsInColumn(connectionBaseAddress, connectionLastAddress, memoryItem,
        selectedItemRect, selectedItemPenWidth, connectedSpaceItems, placedMaps))
    {
        for (MemoryColumn * memoryColumn : columnHandler_->getMapOverlapColumns())
        {
            selectedItemRect.setX(selectedItemRect.x() + memoryColumn->boundingRect().width());

            if (!memoryColumn->memoryMapOverlapsInColumn(connectionBaseAddress, connectionLastAddress, memoryItem,
                selectedItemRect, selectedItemPenWidth, connectedSpaceItems, placedMaps))
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
    QRectF spaceRectangle = spaceItem->getSceneRectangleWithSubItems();
    qreal spaceHeight = spaceRectangle.height();

    spaceItem->setY(ypos);
    spaceRectangle.setY(ypos);
    spaceRectangle.setHeight(spaceHeight);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::placeSpaceItemToColumn()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::placeSpaceItemToColumn(MainMemoryGraphicsItem* spaceItem, QRectF const& spaceRectangle, int spaceLineWidth, QSharedPointer<QVector<MainMemoryGraphicsItem*>> placedSpaceItems) const
{
    for (auto currentColumn : columnHandler_->getAddressSpaceColumns())
    {
        if (!currentColumn->itemOverlapsAnotherPlacedColumnItem(spaceItem, spaceRectangle, spaceLineWidth, placedSpaceItems))
        {
            currentColumn->addItem(spaceItem);
            return;
        }
    }

    MemoryColumn* newSpaceColumn = columnHandler_->createAddressSpaceColumn();
    newSpaceColumn->addItem(spaceItem);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::createConnectionItem()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::createConnectionItem(MainMemoryGraphicsItem* startItem,
    MainMemoryGraphicsItem* endItem,
    quint64 remappedAddress,
    quint64 remappedEndAddress,
    quint64 memoryMapBaseAddress,
    bool hasRemapRange,
    qreal yTransfer)
{
    changeConnectionEndItemRanges(endItem, remappedAddress, memoryMapBaseAddress, hasRemapRange);

    auto newConnectionItem = new MemoryConnectionItem(startItem, remappedAddress, remappedEndAddress, endItem, startItem->scene(), yTransfer);
    connectionsToMemoryMaps_.append(newConnectionItem);

    startItem->hideCollidingRangeLabels(remappedAddress, remappedEndAddress);
    endItem->hideCollidingRangeLabels(remappedAddress, remappedEndAddress);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::changeConnectionEndItemRanges()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::changeConnectionEndItemRanges(MainMemoryGraphicsItem* connectionEndItem,
    quint64 remappedAddress, quint64 memoryMapBaseAddress, bool hasRemappedRange) const
{
    quint64 addressRangeAddition = remappedAddress;
    if (!hasRemappedRange)
    {
        addressRangeAddition = addressRangeAddition - memoryMapBaseAddress;
    }

    connectionEndItem->changeAddressRange(addressRangeAddition);
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

            quint64 firstConnectionBaseAddress = mapItem->getLowestConnectedBaseAddress();
            quint64 lastConnectionLastAddress = mapItem->getHighestConnectedLastAddress();

            int columnWidth = originalColumn->sceneBoundingRect().width();

            QPointF columnPoint(originalColumn->pos().x() - columnWidth, mapRectangle.y());
            MemoryColumn* comparisonColumn = columnHandler_->findColumnAtPosition(columnPoint);
            if (comparisonColumn)
            {
                while (comparisonColumn && !comparisonColumn->name().contains(
                    MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME, Qt::CaseInsensitive))
                {
                    mapRectangle.setX(mapRectangle.x() - columnWidth);

                    if (!comparisonColumn->memoryMapOverlapsInColumn(
                        firstConnectionBaseAddress, lastConnectionLastAddress, mapItem, mapRectangle, mapPenWidth,
                        connectedSpaceItems, placedMapItems))
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
