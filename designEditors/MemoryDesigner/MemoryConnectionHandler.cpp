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

    repositionOverlappingSpaceItems(placedSpaceItems);

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

            bool isChainedSpaceConnection = false;

            quint64 spaceChainConnectionBaseAddress = 0;
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
                        spaceChainConnectionBaseAddress += baseAddressNumber;
                        if (pathInterface->isConnectedToMemory())
                        {
                            MainMemoryGraphicsItem* connectionMiddleItem = getMainGraphicsItem(
                                pathInterface, MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME);
                            if (connectionMiddleItem)
                            {
                                connectionMiddleItem->changeAddressRange(spaceChainConnectionBaseAddress);

                                if (filterAddressSpaceChains_)
                                {
                                    connectionMiddleItem->hide();
                                }
                                else
                                {
//                                     MemoryConnectionItem* newSpaceConnection = createSpaceConnection(
//                                         connectionStartItem, spaceChainConnectionBaseAddress, connectionMiddleItem,
//                                         pathInterface , spaceColumn, placedSpaceItems, spaceItemChain,
//                                         spaceYPlacement);
                                    createSpaceConnection(connectionStartItem, spaceChainConnectionBaseAddress,
                                        connectionMiddleItem, pathInterface , spaceColumn, placedSpaceItems,
                                        spaceItemChain, spaceYPlacement);

                                    spaceItemChain.append(connectionMiddleItem);

                                    connectionStartItem = connectionMiddleItem;

                                    isChainedSpaceConnection = true;
                                }
                            }
                        }

                        baseAddressNumber = pathInterface->getBaseAddress().toULongLong();
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

            quint64 remappedAddress =
                baseAddressNumber + spaceChainConnectionBaseAddress + mirroredSlaveAddressChange;
            quint64 remappedEndAddress =
                endAddressNumber + spaceChainConnectionBaseAddress + mirroredSlaveAddressChange;

            unsigned int yTransfer = baseAddressNumber + memoryMapBaseAddress + mirroredSlaveAddressChange;
            if (filterAddressSpaceChains_)
            {
                yTransfer += spaceChainConnectionBaseAddress;
            }
            yTransfer = yTransfer * MemoryDesignerConstants::RANGEINTERVAL;

            QPointF startConnectionPosBefore = connectionStartItem->pos();

            if (!placedMapItems->contains(connectionEndItem))
            {
                connectionEndItem->hideMemoryRangeLabels();
                connectionEndItem->setPos(connectionEndItem->pos().x(), connectionStartItem->pos().y() + yTransfer);

                qreal startItemPositionBefore = connectionStartItem->pos().y();

                if (spaceItemPlaced && !isChainedSpaceConnection)
                {
                    repositionMemoryMap(placedMapItems, placedSpaceItems, connectionStartItem, spaceColumn,
                        connectionEndItem, yTransfer);
                }
                else
                {
                    checkMemoryMapRepositionToOverlapColumn(
                        placedMapItems, connectionEndItem, memoryMapColumn, connectionStartItem);
                }

                qreal startItemPositionAfter = connectionStartItem->pos().y();

                spaceYPlacement += startItemPositionAfter - startItemPositionBefore;

                placedMapItems->append(connectionEndItem);
            }
            else
            {
                placeSpaceItemToOtherColumn(connectionStartItem, spaceColumn, connectionEndItem, yTransfer);

                spaceYPlacement = spaceYPlacement - (connectionStartItem->boundingRect().height() +
                    MemoryDesignerConstants::SPACEITEMINTERVAL);
            }

            connectionEndItem->changeAddressRange(remappedAddress);

            qreal spaceItemEndPointBefore = connectionStartItem->getSceneEndPoint();

            MemoryConnectionItem* newConnectionItem =
                new MemoryConnectionItem(connectionStartItem, remappedAddress, remappedEndAddress,
                connectionEndItem, spaceColumn->scene(), condenseMemoryItems_, yTransfer);
            connectionsToMemoryMaps_.append(newConnectionItem);

            qreal spaceItemEndPointAfter = connectionStartItem->getSceneEndPoint();

            spaceYPlacement += spaceItemEndPointAfter - spaceItemEndPointBefore;
            spaceYPlacement += connectionStartItem->getSubItemHeightAddition();
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
void MemoryConnectionHandler::createSpaceConnection(MainMemoryGraphicsItem* connectionStartItem,
    quint64 connectionBaseAddress, MainMemoryGraphicsItem* connectionMiddleItem,
    QSharedPointer<ConnectivityInterface> newSpaceInterface, MemoryColumn* spaceColumn,
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
    QVector<MainMemoryGraphicsItem*> spaceItemChain, int& spaceYPlacement)
{
    if (connectionMiddleItem)
    {
        connectionMiddleItem->hideMemoryRangeLabels();

        int startItemPositionY = connectionStartItem->pos().y();
        if (!placedSpaceItems->contains(connectionMiddleItem))
        {
            placeSpaceItem(
                connectionMiddleItem, startItemPositionY, spaceColumn, placedSpaceItems, connectionStartItem);

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

        quint64 startItemBaseAddress = connectionStartItem->getBaseAddress();

        quint64 middleItemRangeStart = startItemBaseAddress + connectionBaseAddress;
        quint64 middleItemRangeEnd = middleItemRangeStart +
            (connectionMiddleItem->getLastAddress() - connectionMiddleItem->getBaseAddress());
        int yTransfer = (startItemBaseAddress + connectionBaseAddress ) * MemoryDesignerConstants::RANGEINTERVAL;

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
            new MemoryConnectionItem(connectionStartItem, middleItemRangeStart, middleItemRangeEnd,
                connectionMiddleItem, spaceColumn->scene(), condenseMemoryItems_, yTransfer);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::placeSpaceItem()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::placeSpaceItem(MainMemoryGraphicsItem* spaceItem, int positionY,
    MemoryColumn* originalColumn, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
    MainMemoryGraphicsItem* connectionStartItem)
{
    QRectF spaceRectangle = spaceItem->sceneBoundingRect();
    qreal rectangleHeight = spaceRectangle.height();
    spaceRectangle.setY(positionY);
    spaceRectangle.setHeight(rectangleHeight);
    int spaceLineWidth = spaceItem->pen().width();

    foreach (MainMemoryGraphicsItem* comparisonSpace, *placedSpaceItems)
    {
        MemoryColumn* comparisonColumn = dynamic_cast<MemoryColumn*>(comparisonSpace->parentItem());
        if (comparisonSpace != connectionStartItem && comparisonColumn && comparisonColumn == originalColumn)
        {
            QRectF comparisonRectangle = comparisonSpace->sceneBoundingRect();
            int comparisonLineWidht = comparisonSpace->pen().width();
            if (MemoryDesignerConstants::itemOverlapsAnotherItem(
                spaceRectangle, spaceLineWidth, comparisonRectangle, comparisonLineWidht))
            {
                changeSpaceItemColumn(spaceItem, spaceRectangle, spaceLineWidth, positionY, originalColumn);
                return;
            }
        }
    }

    originalColumn->moveGraphicsItem(spaceItem, positionY, MemoryDesignerConstants::SPACEITEMINTERVAL);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::changeMasterAddressSpaceColumn()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::changeMasterAddressSpaceColumn(MainMemoryGraphicsItem* masterSpaceItem,
    qreal spaceItemY, MemoryColumn* originalColumn, QVector<MainMemoryGraphicsItem*> spaceItemChain)
{
    originalColumn->removeItem(masterSpaceItem);

    masterSpaceItem->setY(spaceItemY);

    QRectF masterSpaceRectangle = masterSpaceItem->sceneBoundingRect();
    qreal masterSpaceHeight = masterSpaceRectangle.height();
    masterSpaceRectangle.setY(spaceItemY);
    masterSpaceRectangle.setHeight(masterSpaceHeight);
    int masterSpaceLineWidth = masterSpaceItem->pen().width();

    qreal columnWidth = originalColumn->boundingRect().width();
    qreal currentColumnPosition = originalColumn->pos().x() - columnWidth;
    MemoryColumn* currentColumn = 0;
    while (currentColumnPosition >= 0)
    {
        currentColumn = columnHandler_->findColumnAtPosition(QPointF(currentColumnPosition, 0));
        if (currentColumn &&
            currentColumn->name().contains(MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME, Qt::CaseInsensitive))
        {
            if (!currentColumn->itemOverlapsAnotherColumnItem(masterSpaceRectangle, masterSpaceLineWidth))
            {
                currentColumn->addItem(masterSpaceItem);
                return;
            }
            else
            {
                changeCollidingMasterAddressSpaceColumn(currentColumn, spaceItemChain);
                if (!currentColumn->itemOverlapsAnotherColumnItem(masterSpaceRectangle, masterSpaceLineWidth))
                {
                    currentColumn->addItem(masterSpaceItem);
                    return;
                }
            }
        }

        currentColumnPosition -= columnWidth;
    }

    MemoryColumn* newSpaceColumn = columnHandler_->createAddressSpaceColumn();
    newSpaceColumn->setZValue(-1);
    newSpaceColumn->addItem(masterSpaceItem);
    masterSpaceItem->setY(spaceItemY);
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
// Function: MemoryConnectionHandler::checkMemoryMapRepositionToOverlapColumn()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::checkMemoryMapRepositionToOverlapColumn(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMaps, MainMemoryGraphicsItem* memoryItem,
    MemoryColumn* originalColumn, MainMemoryGraphicsItem* connectionStartItem)
{
    QRectF selectedItemRect = memoryItem->sceneBoundingRect();

    int selectedItemPenWidth = memoryItem->pen().width();

    QVector<MainMemoryGraphicsItem*> connectedSpaceItems;
    connectedSpaceItems.append(connectionStartItem);
    foreach (MainMemoryGraphicsItem* chainedSpaceItem, connectionStartItem->getChainedSpaceItems())
    {
        connectedSpaceItems.append(chainedSpaceItem);
    }

    if (originalColumn->memoryMapOverlapsInColumn(
        memoryItem, selectedItemRect, selectedItemPenWidth, connectedSpaceItems, placedMaps))
    {
        foreach (MemoryColumn* memoryColumn, columnHandler_->getMapOverlapColumns())
        {
            selectedItemRect.setX(selectedItemRect.x() + memoryColumn->boundingRect().width());

            if (!memoryColumn->memoryMapOverlapsInColumn(
                memoryItem, selectedItemRect, selectedItemPenWidth, connectedSpaceItems, placedMaps))
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

    QRectF spaceRectangle = spaceItem->sceneBoundingRect();
    qreal spaceHeight = spaceRectangle.height();

    spaceRectangle.setY(targetItem->pos().y() + yTransfer);
    spaceRectangle.setHeight(spaceHeight);

    int spacePenWidth = spaceItem->pen().width();

    foreach (MemoryColumn* currentSpaceColumn, columnHandler_->getAddressSpaceColumns())
    {
        if (!currentSpaceColumn->itemOverlapsAnotherColumnItem(spaceRectangle, spacePenWidth))
        {
            currentSpaceColumn->addItem(spaceItem);
            spaceItem->setY(spaceRectangle.y());
            return;
        }
    }

    MemoryColumn* newSpaceColumn = columnHandler_->createAddressSpaceColumn();
    newSpaceColumn->addItem(spaceItem);
    spaceItem->setY(targetItem->pos().y());
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
            QRectF mapRectangle = mapItem->sceneBoundingRect();
            int mapPenWidth = mapItem->pen().width();

            if (mapItem->hasExtensionItem())
            {
                qreal itemHeight = mapRectangle.height();
                qreal extensionHeight = mapItem->getExtensionItem()->boundingRect().height();
                mapRectangle.setHeight(itemHeight + extensionHeight);
            }

            QVector<MainMemoryGraphicsItem*> connectedSpaceItems = mapItem->getChainedSpaceItems();

            int columnWidth = originalColumn->sceneBoundingRect().width();

            QPointF columnPoint (originalColumn->pos().x() - columnWidth, mapRectangle.y());
            MemoryColumn* comparisonColumn = columnHandler_->findColumnAtPosition(columnPoint);
            if (comparisonColumn)
            {
                while (comparisonColumn && !comparisonColumn->name().contains(
                    MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME, Qt::CaseInsensitive))
                {
                    mapRectangle.setX(mapRectangle.x() - columnWidth);

                    if (!comparisonColumn->memoryMapOverlapsInColumn(
                        mapItem, mapRectangle, mapPenWidth, connectedSpaceItems, placedMapItems))
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

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::repositionOverlappingSpaceItems()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::repositionOverlappingSpaceItems(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems)
{
    for (int spaceIndex = placedSpaceItems->size() - 1; spaceIndex > 0; --spaceIndex)
    {
        MainMemoryGraphicsItem* spaceItem = placedSpaceItems->at(spaceIndex);
        QRectF spaceRectangle = getConnectionAdjustedItemRectangle(spaceItem);
        int spaceLineWidth = spaceItem->pen().width();

        MemoryColumn* spaceItemColumn = dynamic_cast<MemoryColumn*> (spaceItem->parentItem());
        if (spaceItemColumn)
        {
            for (int comparisonIndex = spaceIndex - 1; comparisonIndex > 0; --comparisonIndex)
            {
                MainMemoryGraphicsItem* comparisonSpace = placedSpaceItems->at(comparisonIndex);
                QRectF comparisonRectangle = getConnectionAdjustedItemRectangle(comparisonSpace);
                int comparisonLineWidth = comparisonSpace->pen().width();

                MemoryColumn* comparisonColumn = dynamic_cast<MemoryColumn*>(comparisonSpace->parentItem());
                if (comparisonColumn && spaceItemColumn == comparisonColumn &&
                    MemoryDesignerConstants::itemOverlapsAnotherItem(
                    spaceRectangle, spaceLineWidth, comparisonRectangle, comparisonLineWidth))
                {
                    changeSpaceItemColumn(
                        spaceItem, spaceRectangle, spaceLineWidth, spaceItem->scenePos().y(), spaceItemColumn);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::getConnectionAdjustedItemRectangle()
//-----------------------------------------------------------------------------
QRectF MemoryConnectionHandler::getConnectionAdjustedItemRectangle(MainMemoryGraphicsItem* memoryItem) const
{
    QRectF itemRectangle = memoryItem->sceneBoundingRect();
    if (!memoryItem->getMemoryConnections().isEmpty())
    {
        qreal lastConnectionLow = memoryItem->getLastConnection()->sceneBoundingRect().bottom();
        if (itemRectangle.bottom() < lastConnectionLow)
        {
            itemRectangle.setBottom(lastConnectionLow);
        }
    }

    return itemRectangle;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::changeSpaceItemColumn()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::changeSpaceItemColumn(MainMemoryGraphicsItem* spaceItem, QRectF spaceRectangle,
    int spaceLineWidth, qreal newPositionY, MemoryColumn* originalColumn)
{
    originalColumn->removeItem(spaceItem);
    spaceItem->setY(newPositionY);

    qreal columnWidth = originalColumn->boundingRect().width();
    qreal currentColumnPosition = originalColumn->scenePos().x() - columnWidth;

    MemoryColumn* currentColumn = 0;
    while (currentColumnPosition >= 0)
    {
        currentColumn = columnHandler_->findColumnAtPosition(QPointF(currentColumnPosition, 0));
        if (currentColumn &&
            currentColumn->name().contains(MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME, Qt::CaseInsensitive))
        {
            spaceRectangle.setX(currentColumnPosition);

            MainMemoryGraphicsItem* collidingItem =
                getCollidingSpaceItem(spaceRectangle, spaceLineWidth, currentColumn);
            while (collidingItem)
            {
                QRectF collidingRectangle = getConnectionAdjustedItemRectangle(collidingItem);
                int collidingLineWidth = collidingItem->pen().width();
                changeSpaceItemColumn(
                    collidingItem, collidingRectangle, collidingLineWidth, collidingRectangle.y(), currentColumn);

                collidingItem = getCollidingSpaceItem(spaceRectangle, spaceLineWidth, currentColumn);
            }

            currentColumn->addItem(spaceItem);
            return;
        }

        currentColumnPosition -= columnWidth;
    }

    MemoryColumn* newSpaceColumn = columnHandler_->createAddressSpaceColumn();
    newSpaceColumn->setZValue(-1);
    newSpaceColumn->addItem(spaceItem);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::getCollidingSpaceItem()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryConnectionHandler::getCollidingSpaceItem(QRectF spaceRectangle, int spaceLineWidth,
    MemoryColumn* currentColumn) const
{
    foreach (QGraphicsItem* columnItem, currentColumn->childItems())
    {
        MainMemoryGraphicsItem* comparisonItem = dynamic_cast<MainMemoryGraphicsItem*>(columnItem);
        if (comparisonItem)
        {
            QRectF comparisonRectangle = getConnectionAdjustedItemRectangle(comparisonItem);
            int comparisonLineWidth = comparisonItem->pen().width();
            if (MemoryDesignerConstants::itemOverlapsAnotherItem(
                spaceRectangle, spaceLineWidth, comparisonRectangle, comparisonLineWidth))
            {
                return comparisonItem;
            }
        }
    }

    return 0;
}
