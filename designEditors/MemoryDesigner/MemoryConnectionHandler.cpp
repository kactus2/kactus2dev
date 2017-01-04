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

#include <designEditors/MemoryDesigner/MasterSlavePathSearch.h>
#include <designEditors/MemoryDesigner/ConnectivityInterface.h>
#include <designEditors/MemoryDesigner/MemoryDesignerConstants.h>
#include <designEditors/MemoryDesigner/MainMemoryGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryExtensionGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryColumn.h>
#include <designEditors/MemoryDesigner/MemoryConnectionItem.h>
#include <designEditors/MemoryDesigner/MemoryCollisionItem.h>
#include <designEditors/MemoryDesigner/MemoryColumnHandler.h>

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
// Function: MemoryConnectionHandler::~MemoryConnectionHandler()
//-----------------------------------------------------------------------------
MemoryConnectionHandler::~MemoryConnectionHandler()
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
// Function: MemoryConnectionHandler::createMemoryConnections()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::createMemoryConnections(QSharedPointer<ConnectivityGraph> connectionGraph,
    MemoryColumn* spaceColumn, MemoryColumn* memoryMapColumn)
{
    MasterSlavePathSearch pathSearcher;

    QVector<QVector<QSharedPointer<ConnectivityInterface> > > masterSlavePaths =
        pathSearcher.findMasterSlavePaths(connectionGraph);

    int spaceYPlacement = MemoryDesignerConstants::SPACEITEMINTERVAL;

    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems(
        new QVector<MainMemoryGraphicsItem*> ());
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems(
        new QVector<MainMemoryGraphicsItem*> ());

    foreach (QVector<QSharedPointer<ConnectivityInterface> > singlePath, masterSlavePaths)
    {
        createConnection(
            singlePath, placedMapItems, memoryMapColumn, spaceYPlacement, placedSpaceItems, spaceColumn);
    }

    compressGraphicsItems(placedSpaceItems, spaceYPlacement, spaceColumn);
    if (condenseMemoryItems_)
    {
        repositionCompressedMemoryMaps(placedMapItems, memoryMapColumn);
    }

    moveUnconnectedAddressSpaces(placedSpaceItems, spaceColumn);
    moveUnconnectedMemoryMaps(placedMapItems, memoryMapColumn);

    reDrawConnections(placedSpaceItems);

    createOverlappingConnectionMarkers(placedSpaceItems);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::createConnection()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::createConnection(QVector<QSharedPointer<ConnectivityInterface> > connectionPath,
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems, MemoryColumn* memoryMapColumn,
    int& spaceYPlacement, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
    MemoryColumn* spaceColumn)
{
    QSharedPointer<ConnectivityInterface> startInterface = connectionPath.first();
    QSharedPointer<ConnectivityInterface> endInterface = connectionPath.last();

    if (startInterface && endInterface && startInterface != endInterface)
    {
        MainMemoryGraphicsItem* connectionStartItem =
            getMainGraphicsItem(startInterface, MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME);
        MainMemoryGraphicsItem* connectionEndItem =
            getMainGraphicsItem(endInterface, MemoryDesignerConstants::MEMORYMAPCOLUMNCOMMON_NAME);
        if (connectionStartItem && connectionEndItem)
        {
            connectionStartItem->hideMemoryRangeLabels();

            if (!placedSpaceItems->contains(connectionStartItem))
            {
                spaceColumn->moveGraphicsItem(
                    connectionStartItem, spaceYPlacement, MemoryDesignerConstants::SPACEITEMINTERVAL);
            }

            quint64 baseAddressNumber = startInterface->getBaseAddress().toULongLong();

            bool hasRemapRange = false;
            quint64 mirroredSlaveAddressChange = 0;
            quint64 memoryMapEndAddress = 0;

            QVector<MainMemoryGraphicsItem*> spaceItemChain;
            spaceItemChain.append(connectionStartItem);

            QVector<MemoryConnectionItem*> connectionChain;

            foreach(QSharedPointer<ConnectivityInterface> pathInterface, connectionPath)
            {
                if (pathInterface != startInterface && pathInterface != endInterface)
                {
                    if (pathInterface->getMode().compare(
                        QStringLiteral("mirroredSlave"), Qt::CaseInsensitive) == 0)
                    {
                        mirroredSlaveAddressChange += pathInterface->getRemapAddress().toULongLong();

                        memoryMapEndAddress = pathInterface->getRemapRange().toULongLong() - 1;
                        hasRemapRange = true;
                    }
                    else if (pathInterface->getMode().compare(QStringLiteral("master"), Qt::CaseInsensitive) == 0)
                    {
                        if (pathInterface->isConnectedToMemory())
                        {
                            MainMemoryGraphicsItem* connectionMiddleItem = getMainGraphicsItem(
                                pathInterface, MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME);
                            if (connectionMiddleItem)
                            {
                                if (filterAddressSpaceChains_)
                                {
                                    connectionMiddleItem->hide();
                                }
                                else
                                {
                                    MemoryConnectionItem* newSpaceConnection = createSpaceConnection(
                                        connectionStartItem, connectionMiddleItem, pathInterface, spaceColumn,
                                        placedSpaceItems, spaceItemChain, spaceYPlacement);
                                    if (newSpaceConnection)
                                    {
                                        connectionChain.append(newSpaceConnection);
                                    }

                                    spaceItemChain.append(connectionMiddleItem);

                                    connectionStartItem = connectionMiddleItem;
                                }
                            }
                        }

                        if (!placedSpaceItems->contains(connectionStartItem) && !filterAddressSpaceChains_)
                        {
                            baseAddressNumber = pathInterface->getBaseAddress().toULongLong();
                        }
                        else
                        {
                            mirroredSlaveAddressChange += pathInterface->getBaseAddress().toULongLong();
                        }
                    }
                }
            }

            if (!connectionStartItem->isVisible())
            {
                connectionStartItem->setVisible(true);
            }

            bool spaceItemPlaced = false;
            if (!placedSpaceItems->contains(connectionStartItem))
            {
                placedSpaceItems->append(connectionStartItem);
                spaceItemPlaced = true;
            }

            quint64 memoryMapBaseAddress = connectionEndItem->getBaseAddress();

            if (!hasRemapRange)
            {
                memoryMapEndAddress = connectionEndItem->getLastAddress();
            }

            quint64 endAddressNumber = baseAddressNumber + memoryMapEndAddress;

            quint64 remappedAddress = baseAddressNumber + mirroredSlaveAddressChange;
            quint64 remappedEndAddress = endAddressNumber + mirroredSlaveAddressChange;

            quint64 yTransfer = (memoryMapBaseAddress + mirroredSlaveAddressChange) *
                MemoryDesignerConstants::RANGEINTERVAL;

            QPointF startConnectionPosBefore = connectionStartItem->pos();

            if (!placedMapItems->contains(connectionEndItem))
            {
                connectionEndItem->hideMemoryRangeLabels();
                connectionEndItem->setPos(connectionEndItem->pos().x(), connectionStartItem->pos().y() + yTransfer);

                QPointF startItemPositionBefore = connectionStartItem->pos();

                if (spaceItemPlaced)
                {
                    repositionMemoryMap(placedMapItems, placedSpaceItems, connectionStartItem, spaceColumn,
                        connectionEndItem, yTransfer);
                }
                else
                {
                    checkMemoryMapRepositionToOverlapColumn(placedMapItems, connectionEndItem, memoryMapColumn,
                        remappedAddress, remappedEndAddress, connectionStartItem);
                }

                QPointF startItemPositionAfter = connectionStartItem->pos();

                spaceYPlacement += startItemPositionAfter.y() - startItemPositionBefore.y();

                placedMapItems->append(connectionEndItem);
            }
            else
            {
                placeSpaceItemToOtherColumn(connectionStartItem, spaceColumn, connectionEndItem, yTransfer);

                spaceYPlacement = spaceYPlacement - (connectionStartItem->boundingRect().height() +
                    MemoryDesignerConstants::SPACEITEMINTERVAL);
            }

            if (spaceItemPlaced)
            {
                connectionStartItem->changeChildItemRanges(baseAddressNumber);
            }
            connectionEndItem->changeChildItemRanges(remappedAddress);

            qreal spaceItemEndPointBefore = connectionStartItem->getSceneEndPoint();

            MemoryConnectionItem* newConnectionItem =
                new MemoryConnectionItem(connectionStartItem, remappedAddress, remappedEndAddress,
                connectionEndItem, spaceColumn->scene(), condenseMemoryItems_, yTransfer);
            connectionChain.append(newConnectionItem);
            setHeightForConnectionChain(connectionChain);
            connectionsToMemoryMaps_.append(newConnectionItem);

            qreal spaceItemEndPointAfter = connectionStartItem->getSceneEndPoint();

            spaceYPlacement += spaceItemEndPointAfter - spaceItemEndPointBefore;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::getMainGraphicsItem()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryConnectionHandler::getMainGraphicsItem(
    QSharedPointer<ConnectivityInterface> connectionInterface, QString columnType) const
{
    MainMemoryGraphicsItem* graphicsItem = 0;

    QSharedPointer<MemoryItem> memoryItem = connectionInterface->getConnectedMemory();
    QSharedPointer<ConnectivityComponent> connectionInstance = connectionInterface->getInstance();
    if (memoryItem && connectionInstance)
    {
        QVector<MemoryColumn*> matchingColumns = columnHandler_->getSpecifiedColumns(columnType);
        foreach (MemoryColumn* currentColumn, matchingColumns)
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
// Function: MemoryConnectionHandler::createSpaceConnection()
//-----------------------------------------------------------------------------
MemoryConnectionItem* MemoryConnectionHandler::createSpaceConnection(MainMemoryGraphicsItem* connectionStartItem,
    MainMemoryGraphicsItem* connectionMiddleItem, QSharedPointer<ConnectivityInterface> newSpaceInterface,
    MemoryColumn* spaceColumn, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
    QVector<MainMemoryGraphicsItem*> spaceItemChain, int& spaceYPlacement)
{
    MemoryConnectionItem* addressSpaceConnection = 0;

    if (connectionMiddleItem)
    {
        connectionMiddleItem->hideMemoryRangeLabels();

        int startItemPositionY = connectionStartItem->pos().y();
        if (!placedSpaceItems->contains(connectionMiddleItem))
        {
            spaceColumn->moveGraphicsItem(connectionMiddleItem, startItemPositionY,
                MemoryDesignerConstants::SPACEITEMINTERVAL);
            connectionMiddleItem->hideFirstAndLastSegmentRange();
        }

        bool recalculateYPosition = false;
        if (!placedSpaceItems->contains(connectionStartItem))
        {
            quint64 middleItemY = connectionMiddleItem->pos().y();

            changeMasterAddressSpaceColumn(connectionStartItem, middleItemY, spaceColumn, spaceItemChain);

            placedSpaceItems->append(connectionStartItem);
            recalculateYPosition = true;
        }

        quint64 interfaceBaseAddress = newSpaceInterface->getBaseAddress().toULongLong();
        quint64 startItemBaseAddress = connectionStartItem->getBaseAddress();
        quint64 baseAddressDifference = interfaceBaseAddress - startItemBaseAddress;
        int yTransfer = baseAddressDifference * MemoryDesignerConstants::RANGEINTERVAL;

        quint64 middleItemRangeStart = connectionMiddleItem->getBaseAddress() + interfaceBaseAddress;
        quint64 middleItemRangeEnd = connectionMiddleItem->getLastAddress() + interfaceBaseAddress;

        if (!placedSpaceItems->contains(connectionMiddleItem))
        {
            connectionMiddleItem->setPos(
                connectionMiddleItem->pos().x(), connectionMiddleItem->pos().y() + yTransfer);
        }

        if (recalculateYPosition)
        {
            quint64 newYPosition =
                connectionMiddleItem->sceneBoundingRect().height() + MemoryDesignerConstants::SPACEITEMINTERVAL;
            if (newYPosition > spaceYPlacement)
            {
                spaceYPlacement = newYPosition;
            }
        }

        if (!placedSpaceItems->contains(connectionMiddleItem) || !placedSpaceItems->contains(connectionStartItem))
        {
            addressSpaceConnection = new MemoryConnectionItem(connectionStartItem, middleItemRangeStart,
                middleItemRangeEnd, connectionMiddleItem, spaceColumn->scene(), condenseMemoryItems_, yTransfer);
        }
        else
        {
            addressSpaceConnection = getAddressSpaceChainConnection(connectionStartItem, connectionMiddleItem);
        }
    }

    return addressSpaceConnection;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::changeMasterAddressSpaceColumn()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::changeMasterAddressSpaceColumn(MainMemoryGraphicsItem* masterSpaceItem,
    qreal spaceItemY, MemoryColumn* originalColumn, QVector<MainMemoryGraphicsItem*> spaceItemChain)
{
    originalColumn->removeItem(masterSpaceItem);

    qreal columnWidth = originalColumn->boundingRect().width();
    qreal currentColumnPosition = originalColumn->pos().x() - columnWidth;
    MemoryColumn* currentColumn = 0;
    while (currentColumnPosition >= 0)
    {
        currentColumn = columnHandler_->findColumnAtPosition(QPointF(currentColumnPosition, 0));
        if (currentColumn &&
            currentColumn->name().contains(MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME, Qt::CaseInsensitive))
        {
            currentColumn->addItem(masterSpaceItem);
            masterSpaceItem->setPos(masterSpaceItem->pos().x(), spaceItemY);

            if (!masterSpaceItem->itemCollidesWithSimilarItems())
            {
                return;
            }
            else
            {
                changeCollidingMasterAddressSpaceColumn(currentColumn, spaceItemChain);

                if (!masterSpaceItem->itemCollidesWithSimilarItems())
                {
                    return;
                }
                else
                {
                    currentColumn->removeItem(masterSpaceItem);
                }
            }
        }

        currentColumnPosition -= columnWidth;
    }

    MemoryColumn* newSpaceColumn = columnHandler_->createAddressSpaceColumn();
    newSpaceColumn->setZValue(-1);
    newSpaceColumn->addItem(masterSpaceItem);
    masterSpaceItem->setPos(masterSpaceItem->pos().x(), spaceItemY);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::changeCollidingMasterAddressSpaceColumn()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::changeCollidingMasterAddressSpaceColumn(MemoryColumn* currentColumn,
    QVector<MainMemoryGraphicsItem*> spaceItemChain)
{
    foreach (MainMemoryGraphicsItem* currentSpaceChainItem, spaceItemChain)
    {
        GraphicsColumn* chainParent =
            dynamic_cast<GraphicsColumn*>(currentSpaceChainItem->parentItem());
        if (chainParent && chainParent == currentColumn)
        {
            changeMasterAddressSpaceColumn(
                currentSpaceChainItem, currentSpaceChainItem->pos().y(), currentColumn, spaceItemChain);
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::getAddressSpaceChainConnection()
//-----------------------------------------------------------------------------
MemoryConnectionItem* MemoryConnectionHandler::getAddressSpaceChainConnection(
    MainMemoryGraphicsItem* connectionStartItem, MainMemoryGraphicsItem* connectionMiddleItem) const
{
    QMapIterator<quint64, MemoryConnectionItem*> connectionIterator(connectionStartItem->getMemoryConnections());
    while (connectionIterator.hasNext())
    {
        connectionIterator.next();

        MemoryConnectionItem* connectionItem = connectionIterator.value();
        if (connectionMiddleItem->hasConnection(connectionItem))
        {
            return connectionItem;
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::setHeightForConnectionChain()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::setHeightForConnectionChain(QVector<MemoryConnectionItem*> connectionChain)
{
    foreach (MemoryConnectionItem* connectionItem, connectionChain)
    {
        connectionItem->setConnectionWidth();
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::checkMemoryMapRepositionToOverlapColumn()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::checkMemoryMapRepositionToOverlapColumn(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMaps, MainMemoryGraphicsItem* memoryItem,
    MemoryColumn* originalColumn, quint64 mapBaseAddress, quint64 mapLastAddress,
    MainMemoryGraphicsItem* connectionStartItem)
{
    QRectF selectedItemRect = memoryItem->sceneBoundingRect();

    int selectedItemPenWidth = memoryItem->pen().width();

    QVector<MainMemoryGraphicsItem*> connectedSpaceItems;
    connectedSpaceItems.append(connectionStartItem);

    if (originalColumn->memoryMapOverlapsInColumn(memoryItem, mapBaseAddress, mapLastAddress, selectedItemRect,
        selectedItemPenWidth, connectedSpaceItems, placedMaps))
    {
        foreach (MemoryColumn* memoryColumn, columnHandler_->getMapOverlapColumns())
        {
            selectedItemRect.setX(selectedItemRect.x() + memoryColumn->boundingRect().width());

            if (!memoryColumn->memoryMapOverlapsInColumn(memoryItem, mapBaseAddress, mapLastAddress,
                selectedItemRect, selectedItemPenWidth, connectedSpaceItems, placedMaps))
            {
                originalColumn->removeItem(memoryItem);
                memoryColumn->addItem(memoryItem);
                return;
            }
        }

        MemoryColumn* overlapColumn = columnHandler_->createMemoryOverlapColumn();
        originalColumn->removeItem(memoryItem);
        overlapColumn->addItem(memoryItem);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::repositionMemoryMap()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::repositionMemoryMap(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
    MainMemoryGraphicsItem* startConnectionItem, MemoryColumn* addressSpaceColumn,
    MainMemoryGraphicsItem* endConnectionItem, int memoryMapYTransfer)
{
    QRectF endConnectionRectangle = endConnectionItem->sceneBoundingRect();
    int endConnectionPenWidth = endConnectionItem->pen().width();
    
    foreach (MemoryDesignerGraphicsItem* memoryMapItem, *placedMapItems)
    {
        QRectF comparisonRectangle = memoryMapItem->sceneBoundingRect();
        int comparisonPenWidth = memoryMapItem->pen().width();

        if (endConnectionItem != memoryMapItem && MemoryDesignerConstants::itemOverlapsAnotherItem(
            endConnectionRectangle, endConnectionPenWidth, comparisonRectangle, comparisonPenWidth))
        {
            int mapItemYTransfer = memoryMapItem->sceneBoundingRect().bottom();

            endConnectionItem->setPos(endConnectionItem->x(), mapItemYTransfer);

            startConnectionItem->setPos(
                startConnectionItem->pos().x(), endConnectionItem->pos().y() - memoryMapYTransfer);

            repositionSpaceItemToMemoryMap(
                placedSpaceItems, startConnectionItem, endConnectionItem, addressSpaceColumn, memoryMapYTransfer);

            addressSpaceColumn->onMoveItem(startConnectionItem);
            addressSpaceColumn->onReleaseItem(startConnectionItem);

            repositionMemoryMap(placedMapItems, placedSpaceItems, startConnectionItem, addressSpaceColumn,
                endConnectionItem, memoryMapYTransfer);

            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::repositionSpaceItemToMemoryMap()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::repositionSpaceItemToMemoryMap(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems, MainMemoryGraphicsItem* startItem,
    MainMemoryGraphicsItem* endItem, MemoryColumn* spaceColumn, int memoryMapYTransfer)
{
    foreach (MemoryDesignerGraphicsItem* spaceItem, *placedSpaceItems)
    {
        if (spaceItem != startItem && startItem->collidesWithItem(spaceItem))
        {
            int spaceItemYTransfer =
                spaceItem->sceneBoundingRect().bottom() + MemoryDesignerConstants::SPACEITEMINTERVAL;

            startItem->setPos(startItem->x(), spaceItemYTransfer);

            endItem->setPos(endItem->x(), startItem->y() + memoryMapYTransfer);

            repositionSpaceItemToMemoryMap(placedSpaceItems, startItem, endItem, spaceColumn, memoryMapYTransfer);

            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::placeSpaceItemToOtherColumn()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::placeSpaceItemToOtherColumn(MainMemoryGraphicsItem* spaceItem,
    MemoryColumn* originalColumn, MainMemoryGraphicsItem* targetItem, int yTransfer)
{
    originalColumn->removeItem(spaceItem);

    foreach (MemoryColumn* currentSpaceColumn, columnHandler_->getAddressSpaceColumns())
    {
        currentSpaceColumn->addItem(spaceItem);
        spaceItem->setPos(spaceItem->pos().x(), targetItem->pos().y() + yTransfer);

        if (spaceItem->itemCollidesWithSimilarItems())
        {
            currentSpaceColumn->removeItem(spaceItem);
        }
        else
        {
            return;
        }
    }

    MemoryColumn* newSpaceColumn = columnHandler_->createAddressSpaceColumn();
    newSpaceColumn->addItem(spaceItem);
    spaceItem->setPos(spaceItem->pos().x(), targetItem->pos().y());
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::reDrawConnections()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::reDrawConnections(QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems)
{
    foreach (MainMemoryGraphicsItem* spaceItem, *placedSpaceItems)
    {
        spaceItem->reDrawConnections();
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::moveUnconnectedAddressSpaces()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::moveUnconnectedAddressSpaces(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems, MemoryColumn* spaceColumn)
{
    if (!placedSpaceItems->isEmpty())
    {
        spaceColumn->moveUnconnectedMemoryItems(placedSpaceItems);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::moveUnconnectedMemoryMaps()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::moveUnconnectedMemoryMaps(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems, MemoryColumn* memoryMapColumn)
{
    if (!placedMapItems->isEmpty())
    {
        memoryMapColumn->moveUnconnectedMemoryItems(placedMapItems);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::createOverlappingConnectionMarkers()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::createOverlappingConnectionMarkers(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems)
{
    foreach (MainMemoryGraphicsItem* spaceItem, *placedSpaceItems)
    {
        spaceItem->createOverlappingConnectionMarkers();
        
        foreach (MemoryCollisionItem* collisionItem, spaceItem->getMemoryCollisions())
        {
            memoryCollisions_.append(collisionItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::compressGraphicsItems()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::compressGraphicsItems(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems, int& spaceYPlacement,
    MemoryColumn* spaceColumn)
{
    QSharedPointer<QVector<MemoryConnectionItem*> > movedConnectionItems (new QVector<MemoryConnectionItem*>());

    foreach (MemoryColumn* memoryColumn, columnHandler_->getMemoryColumns())
    {
        memoryColumn->compressGraphicsItems(
            condenseMemoryItems_, spaceYPlacement, spaceColumn, placedSpaceItems, movedConnectionItems);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::repositionCompressedMemoryMaps()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::repositionCompressedMemoryMaps(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems, MemoryColumn* memoryMapColumn)
{
    foreach (MainMemoryGraphicsItem* mapItem, *placedMapItems)
    {
        QGraphicsItem* mapParentItem = mapItem->parentItem();
        MemoryColumn* originalColumn = dynamic_cast<MemoryColumn*>(mapParentItem);
        if (originalColumn && originalColumn != memoryMapColumn)
        {
            quint64 mapBaseAddress = mapItem->getBaseAddress();
            quint64 mapLastAddress = mapItem->getLastAddress();

            QRectF mapRectangle = mapItem->sceneBoundingRect();
            int mapPenWidth = mapItem->pen().width();

            if (mapItem->hasExtensionItem())
            {
                mapRectangle.setHeight(
                    mapRectangle.height() + mapItem->getExtensionItem()->boundingRect().height());
            }

            QVector<MainMemoryGraphicsItem*> connectedSpaceItems;
            foreach (MemoryConnectionItem* connectionItem, mapItem->getMemoryConnections())
            {
                MainMemoryGraphicsItem* connectionStartItem = connectionItem->getConnectionStartItem();
                if (!connectedSpaceItems.contains(connectionStartItem))
                {
                    connectedSpaceItems.append(connectionStartItem);
                }
            }

            int columnWidth = originalColumn->sceneBoundingRect().width();

            QPointF columnPoint (originalColumn->pos().x() - columnWidth, mapRectangle.y());
            MemoryColumn* comparisonColumn = columnHandler_->findColumnAtPosition(columnPoint);
            if (comparisonColumn)
            {
                while (comparisonColumn && !comparisonColumn->name().contains(
                    MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME, Qt::CaseInsensitive))
                {
                    mapRectangle.setX(mapRectangle.x() - columnWidth);

                    if (!comparisonColumn->memoryMapOverlapsInColumn(mapItem, mapBaseAddress, mapLastAddress,
                        mapRectangle, mapPenWidth, connectedSpaceItems, placedMapItems))
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
// Function: MemoryConnectionHandler::reDrawConnectionsAndCollisions()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::reDrawConnectionsAndCollisions()
{
    foreach (MemoryConnectionItem* connectionItem, connectionsToMemoryMaps_)
    {
        connectionItem->reDrawConnection();
    }
    foreach (MemoryCollisionItem* collisionItem, memoryCollisions_)
    {
        collisionItem->reDrawCollision();
    }
}
