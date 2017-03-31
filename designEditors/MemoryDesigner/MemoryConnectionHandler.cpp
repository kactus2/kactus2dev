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
#include <designEditors/MemoryDesigner/MemoryItem.h>

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

    qreal previousConnectionLow = 0;
    foreach (QVector<QSharedPointer<ConnectivityInterface> > singlePath, masterSlavePaths)
    {
        if (!singlePath.isEmpty())
        {
            createConnection(singlePath, placedMapItems, memoryMapColumn, spaceYPlacement, placedSpaceItems,
                spaceColumn, previousConnectionLow);
        }
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
    MemoryColumn* spaceColumn, qreal& previousConnectionLow)
{
    QSharedPointer<ConnectivityInterface> startInterface = connectionPath.first();
    QSharedPointer<ConnectivityInterface> endInterface = connectionPath.last();

    MainMemoryGraphicsItem* connectionStartItem =
        getMainGraphicsItem(startInterface, MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME);

    MainMemoryGraphicsItem* connectionEndItem = getConnectionEndItem(startInterface, endInterface);

    if (!connectionStartItem || !connectionEndItem)
    {
        return;
    }

    if (!placedSpaceItems->contains(connectionStartItem))
    {
        spaceColumn->moveGraphicsItem(
            connectionStartItem, spaceYPlacement, MemoryDesignerConstants::SPACEITEMINTERVAL);
    }

    quint64 baseAddressNumber = getStartingBaseAddress(startInterface, endInterface);

    MemoryConnectionHandler::ConnectionPathVariables pathVariables =
        examineConnectionPath(baseAddressNumber, connectionStartItem, connectionEndItem, startInterface,
        endInterface, connectionPath, spaceColumn, placedSpaceItems, spaceYPlacement);

    baseAddressNumber = pathVariables.baseAddressNumber_;
    connectionStartItem = pathVariables.spaceItemConnectedToMapItem_;

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

    quint64 memoryMapBaseAddress = connectionEndItem->getOriginalBaseAddress();
    quint64 endAddressNumber = baseAddressNumber + pathVariables.memoryMapEndAddress_;

    quint64 remappedAddress =
        getRemappedBaseAddress(memoryMapBaseAddress, baseAddressNumber, pathVariables.spaceChainBaseAddress_,
        pathVariables.mirroredSlaveAddressChange_, pathVariables.hasRemapRange_);

    quint64 remappedEndAddress =
        endAddressNumber + pathVariables.spaceChainBaseAddress_ + pathVariables.mirroredSlaveAddressChange_;

    qreal yTransfer = getConnectionInitialTransferY(baseAddressNumber, pathVariables.mirroredSlaveAddressChange_,
        pathVariables.hasRemapRange_, memoryMapBaseAddress, pathVariables.spaceChainBaseAddress_);

    if (!placedMapItems->contains(connectionEndItem))
    {
        placeMemoryMapItem(remappedAddress, remappedEndAddress, connectionEndItem, connectionStartItem, yTransfer,
            pathVariables.hasRemapRange_, memoryMapBaseAddress, spaceItemPlaced,
            pathVariables.isChainedSpaceConnection_, placedMapItems, placedSpaceItems, memoryMapColumn,
            spaceColumn, spaceYPlacement);
    }
    else
    {
        placeSpaceItemToOtherColumn(
            connectionStartItem, spaceColumn, connectionEndItem, remappedAddress);

        spaceYPlacement = spaceYPlacement - (connectionStartItem->getHeightWithSubItems() +
            MemoryDesignerConstants::SPACEITEMINTERVAL);
    }

    createMemoryConnectionItem(connectionStartItem, connectionEndItem, remappedAddress, remappedEndAddress,
        memoryMapBaseAddress, pathVariables.hasRemapRange_, yTransfer, previousConnectionLow, spaceYPlacement);
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
// Function: MemoryConnectionHandler::getConnectionEndItem()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryConnectionHandler::getConnectionEndItem(
    QSharedPointer<ConnectivityInterface> startInterface, QSharedPointer<ConnectivityInterface> endInterface) const
{
    MainMemoryGraphicsItem* connectionEndItem(0);

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
// Function: MemoryConnectionHandler::getStartingBaseAddress()
//-----------------------------------------------------------------------------
quint64 MemoryConnectionHandler::getStartingBaseAddress(QSharedPointer<ConnectivityInterface> startInterface,
    QSharedPointer<ConnectivityInterface> endInterface) const
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
// Function: MemoryConnectionHandler::MemoryConnectionHandler()
//-----------------------------------------------------------------------------
MemoryConnectionHandler::ConnectionPathVariables MemoryConnectionHandler::examineConnectionPath(
    quint64 baseAddressNumber, MainMemoryGraphicsItem* connectionStartItem,
    MainMemoryGraphicsItem* connectionEndItem, QSharedPointer<ConnectivityInterface> startInterface,
    QSharedPointer<ConnectivityInterface> endInterface,
    QVector<QSharedPointer<ConnectivityInterface> > connectionPath, MemoryColumn* spaceColumn,
    QSharedPointer<QVector<MainMemoryGraphicsItem* > > placedSpaceItems, int& spaceYPlacement)
{
    MemoryConnectionHandler::ConnectionPathVariables pathVariables;
    pathVariables.hasRemapRange_ = false;
    pathVariables.isChainedSpaceConnection_ = false;
    pathVariables.baseAddressNumber_ = baseAddressNumber;
    pathVariables.mirroredSlaveAddressChange_ = 0;
    pathVariables.memoryMapEndAddress_ = connectionEndItem->getOriginalLastAddress();
    pathVariables.spaceChainBaseAddress_ = 0;
    pathVariables.spaceChain_.append(connectionStartItem);
    pathVariables.spaceItemConnectedToMapItem_ = connectionStartItem;

    foreach(QSharedPointer<ConnectivityInterface> pathInterface, connectionPath)
    {
        if (pathInterface != startInterface && pathInterface != endInterface)
        {
            if (pathInterface->getMode().compare(
                QStringLiteral("mirroredSlave"), Qt::CaseInsensitive) == 0 &&
                !pathInterface->getRemapAddress().isEmpty() && !pathInterface->getRemapRange().isEmpty())
            {
                pathVariables.mirroredSlaveAddressChange_ += pathInterface->getRemapAddress().toULongLong();

                pathVariables.memoryMapEndAddress_ = pathInterface->getRemapRange().toULongLong() - 1;
                pathVariables.hasRemapRange_ = true;
            }
            else if (pathInterface->getMode().compare(QStringLiteral("master"), Qt::CaseInsensitive) == 0)
            {
                if (pathInterface->isConnectedToMemory())
                {
                    pathVariables.spaceChainBaseAddress_ += pathVariables.baseAddressNumber_;

                    MainMemoryGraphicsItem* connectionMiddleItem = getMainGraphicsItem(
                        pathInterface, MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME);
                    if (connectionMiddleItem)
                    {
                        connectionMiddleItem->changeAddressRange(pathVariables.spaceChainBaseAddress_);

                        if (filterAddressSpaceChains_)
                        {
                            connectionMiddleItem->hide();
                        }
                        else
                        {
                            createSpaceConnection(pathVariables.spaceItemConnectedToMapItem_,
                                pathVariables.spaceChainBaseAddress_, connectionMiddleItem, pathInterface,
                                spaceColumn, placedSpaceItems, pathVariables.spaceChain_, spaceYPlacement);

                            pathVariables.spaceChain_.append(connectionMiddleItem);
                            pathVariables.spaceItemConnectedToMapItem_ = connectionMiddleItem;
                            pathVariables.isChainedSpaceConnection_ = true;
                        }
                    }

                    pathVariables.baseAddressNumber_ = pathInterface->getBaseAddress().toULongLong();
                }
            }
        }
    }

    return pathVariables;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::getRemappedBaseAddress()
//-----------------------------------------------------------------------------
quint64 MemoryConnectionHandler::getRemappedBaseAddress(quint64 memoryMapBaseAddress, quint64 baseAddressNumber,
    quint64 spaceChainConnectionBaseAddress, quint64 mirroredSlaveAddressChange, bool hasRemapRange) const
{
    quint64 remappedAddress =
        baseAddressNumber + spaceChainConnectionBaseAddress + mirroredSlaveAddressChange;

    if (!hasRemapRange)
    {
        remappedAddress += memoryMapBaseAddress;
    }

    return remappedAddress;
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

    if (filterAddressSpaceChains_)
    {
        yTransfer += spaceChainConnectionBaseAddress;
    }

    yTransfer = yTransfer * MemoryDesignerConstants::RANGEINTERVAL;

    return yTransfer;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::placeMemoryMapItem()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::placeMemoryMapItem(quint64 connectionBaseAddress, quint64 connectionLastAddress,
    MainMemoryGraphicsItem* connectionEndItem, MainMemoryGraphicsItem* connectionStartItem, qreal yTransfer,
    bool hasRemapRange, quint64 memoryMapBaseAddress, bool spaceItemPlaced, bool isChainedSpaceConnection,
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems,
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems, MemoryColumn* memoryMapColumn,
    MemoryColumn* spaceColumn, int& spaceYPlacement)
{
    qreal memoryMapYTransfer = yTransfer;
    if (hasRemapRange)
    {
        memoryMapYTransfer += (memoryMapBaseAddress * MemoryDesignerConstants::RANGEINTERVAL);
    }

    connectionEndItem->setPos(
        connectionEndItem->pos().x(), connectionStartItem->pos().y() + memoryMapYTransfer);

    qreal startItemPositionBefore = connectionStartItem->pos().y();

    if (spaceItemPlaced && !isChainedSpaceConnection)
    {
        repositionMemoryMap(placedMapItems, placedSpaceItems, connectionStartItem, spaceColumn,
            connectionEndItem, memoryMapYTransfer);
    }
    else
    {
        checkMemoryMapRepositionToOverlapColumn(connectionBaseAddress, connectionLastAddress, placedMapItems,
            connectionEndItem, memoryMapColumn, connectionStartItem);
    }

    qreal startItemPositionAfter = connectionStartItem->pos().y();

    spaceYPlacement += startItemPositionAfter - startItemPositionBefore;

    placedMapItems->append(connectionEndItem);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::createMemoryConnectionItem()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::createMemoryConnectionItem(MainMemoryGraphicsItem* connectionStartItem,
    MainMemoryGraphicsItem* connectionEndItem, quint64 remappedAddress, quint64 remappedEndAddress,
    quint64 memoryMapBaseAddress, bool hasRemapRange, qreal yTransfer, qreal& previousConnectionLow,
    int& spaceYPlacement)
{
    changeConnectionEndItemRanges(connectionEndItem, remappedAddress, memoryMapBaseAddress, hasRemapRange);

    quint64 spaceItemEndPointBefore = connectionStartItem->getSceneEndPoint();

    MemoryConnectionItem* newConnectionItem = new MemoryConnectionItem(connectionStartItem,
        remappedAddress, remappedEndAddress, connectionEndItem, connectionStartItem->scene(), yTransfer);
    connectionsToMemoryMaps_.append(newConnectionItem);

    connectionStartItem->hideCollidingRangeLabels(remappedAddress, remappedEndAddress);
    connectionEndItem->hideCollidingRangeLabels(remappedAddress, remappedEndAddress);

    quint64 spaceItemEndPointAfter = connectionStartItem->getSceneEndPoint();

    if (spaceItemEndPointAfter > previousConnectionLow)
    {
        spaceYPlacement += spaceItemEndPointAfter - spaceItemEndPointBefore;
        previousConnectionLow = spaceItemEndPointAfter;
    }

    qreal endItemChangeToPlacement = connectionEndItem->getSceneRectangleWithSubItems().bottom() +
        MemoryDesignerConstants::SPACEITEMINTERVAL;
    if (endItemChangeToPlacement > spaceYPlacement)
    {
        spaceYPlacement = endItemChangeToPlacement;
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::getLocalMemoryMapItem()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryConnectionHandler::getLocalMemoryMapItem(
    QSharedPointer<ConnectivityInterface> spaceInterface) const
{
    MainMemoryGraphicsItem* localMapItem = 0;

    QSharedPointer<MemoryItem> memoryItemForLocalMap = getMemoryItemForLocalMap(spaceInterface);
    if (memoryItemForLocalMap)
    {
        QVector<MemoryColumn*> matchingColumns =
            columnHandler_->getSpecifiedColumns(MemoryDesignerConstants::MEMORYMAPCOLUMNCOMMON_NAME);
        foreach (MemoryColumn* currentColumn, matchingColumns)
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
// Function: MemoryConnectionHandler::getMemoryItemForLocalMap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryItem> MemoryConnectionHandler::getMemoryItemForLocalMap(
    QSharedPointer<ConnectivityInterface> spaceInterface) const
{
    QSharedPointer<MemoryItem> spaceMemoryItem = spaceInterface->getConnectedMemory();
    QSharedPointer<ConnectivityComponent> connectionInstance = spaceInterface->getInstance();
    if (spaceMemoryItem && connectionInstance)
    {
        foreach (QSharedPointer<MemoryItem> subSpaceItem, spaceMemoryItem->getChildItems())
        {
            if (subSpaceItem->getType().compare(MemoryDesignerConstants::MEMORYMAP_TYPE, Qt::CaseInsensitive) == 0)
            {
                return subSpaceItem;
            }
        }
    }

    return QSharedPointer<MemoryItem>();
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
                connectionMiddleItem->getHeightWithSubItems() + MemoryDesignerConstants::SPACEITEMINTERVAL;
            if (newYPosition > spaceYPlacement)
            {
                spaceYPlacement = newYPosition;
            }
        }

        if (!placedSpaceItems->contains(connectionMiddleItem) || !placedSpaceItems->contains(connectionStartItem))
        {
            new MemoryConnectionItem(connectionStartItem, middleItemRangeStart, middleItemRangeEnd,
                connectionMiddleItem, spaceColumn->scene(), yTransfer);
        }

        connectionStartItem->hideCollidingRangeLabels(middleItemRangeStart, middleItemRangeEnd);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::placeSpaceItem()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::placeSpaceItem(MainMemoryGraphicsItem* spaceItem, int positionY,
    MemoryColumn* originalColumn, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
    MainMemoryGraphicsItem* connectionStartItem)
{
    QRectF spaceRectangle = spaceItem->getSceneRectangleWithSubItems();
    qreal rectangleHeight = spaceRectangle.height();
    spaceRectangle.setY(positionY);
    spaceRectangle.setHeight(rectangleHeight);
    int spaceLineWidth = spaceItem->pen().width();

    foreach (MainMemoryGraphicsItem* comparisonSpace, *placedSpaceItems)
    {
        MemoryColumn* comparisonColumn = dynamic_cast<MemoryColumn*>(comparisonSpace->parentItem());
        if (comparisonSpace != connectionStartItem && comparisonColumn && comparisonColumn == originalColumn)
        {
            QRectF comparisonRectangle = comparisonSpace->getSceneRectangleWithSubItems();
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

    QRectF masterSpaceRectangle = masterSpaceItem->getSceneRectangleWithSubItems();
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
            if (!currentColumn->itemOverlapsAnotherColumnItem(
                masterSpaceItem, masterSpaceRectangle, masterSpaceLineWidth))
            {
                currentColumn->addItem(masterSpaceItem);
                return;
            }
            else
            {
                changeCollidingMasterAddressSpaceColumn(currentColumn, spaceItemChain);
                if (!currentColumn->itemOverlapsAnotherColumnItem(
                    masterSpaceItem, masterSpaceRectangle, masterSpaceLineWidth))
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
void MemoryConnectionHandler::checkMemoryMapRepositionToOverlapColumn(quint64 connectionBaseAddress,
    quint64 connectionLastAddress, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMaps,
    MainMemoryGraphicsItem* memoryItem, MemoryColumn* originalColumn, MainMemoryGraphicsItem* connectionStartItem)
{
    QRectF selectedItemRect = memoryItem->getSceneRectangleWithSubItems();

    int selectedItemPenWidth = memoryItem->pen().width();

    QVector<MainMemoryGraphicsItem*> connectedSpaceItems;
    connectedSpaceItems.append(connectionStartItem);
    foreach (MainMemoryGraphicsItem* chainedSpaceItem, connectionStartItem->getChainedSpaceItems())
    {
        connectedSpaceItems.append(chainedSpaceItem);
    }

    if (originalColumn->memoryMapOverlapsInColumn(connectionBaseAddress, connectionLastAddress, memoryItem,
        selectedItemRect, selectedItemPenWidth, connectedSpaceItems, placedMaps))
    {
        foreach (MemoryColumn* memoryColumn, columnHandler_->getMapOverlapColumns())
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
    QRectF endConnectionRectangle = endConnectionItem->getSceneRectangleWithSubItems();
    int endConnectionPenWidth = endConnectionItem->pen().width();
    
    foreach (MainMemoryGraphicsItem* memoryMapItem, *placedMapItems)
    {
        QRectF comparisonRectangle = memoryMapItem->getSceneRectangleWithSubItems();
        int comparisonPenWidth = memoryMapItem->pen().width();

        if (endConnectionItem != memoryMapItem && MemoryDesignerConstants::itemOverlapsAnotherItem(
            endConnectionRectangle, endConnectionPenWidth, comparisonRectangle, comparisonPenWidth))
        {
            int mapItemYTransfer = comparisonRectangle.bottom();

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
    foreach (MainMemoryGraphicsItem* spaceItem, *placedSpaceItems)
    {
        if (spaceItem != startItem && startItem->collidesWithItem(spaceItem))
        {
            int spaceItemYTransfer =
                spaceItem->getSceneRectangleWithSubItems().bottom() + MemoryDesignerConstants::SPACEITEMINTERVAL;

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
    MemoryColumn* originalColumn, MainMemoryGraphicsItem* targetItem, quint64 connectionBaseAddress)
{
    originalColumn->removeItem(spaceItem);

    QRectF spaceRectangle = spaceItem->getSceneRectangleWithSubItems();
    qreal spaceHeight = spaceRectangle.height();

    qreal spacePositionY = connectionBaseAddress * MemoryDesignerConstants::RANGEINTERVAL;
    MemoryConnectionItem* firstMapConnection = targetItem->getMemoryConnections().first();
    spacePositionY = firstMapConnection->sceneBoundingRect().top() - spacePositionY + 1;

    spaceRectangle.setY(spacePositionY);
    spaceRectangle.setHeight(spaceHeight);

    int spaceLineWidth = spaceItem->pen().width();
    qreal spaceTransferY = spacePositionY - spaceItem->sceneBoundingRect().top() - spaceLineWidth;

    foreach (MemoryColumn* currentSpaceColumn, columnHandler_->getAddressSpaceColumns())
    {
        if (!currentSpaceColumn->itemOverlapsAnotherColumnItem(spaceItem, spaceRectangle, spaceLineWidth))
        {
            currentSpaceColumn->addItem(spaceItem);
            spaceItem->moveItemAndConnectedItems(spaceTransferY);
            return;
        }
    }

    MemoryColumn* newSpaceColumn = columnHandler_->createAddressSpaceColumn();
    newSpaceColumn->addItem(spaceItem);
    spaceItem->moveItemAndConnectedItems(spaceTransferY);
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
    spaceColumn->moveUnconnectedMemoryItems(placedSpaceItems);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::moveUnconnectedMemoryMaps()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::moveUnconnectedMemoryMaps(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems, MemoryColumn* memoryMapColumn)
{
    memoryMapColumn->moveUnconnectedMemoryItems(placedMapItems);
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
// Function: MemoryConnectionHandler::changeConnectionEndItemRanges()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::changeConnectionEndItemRanges(MainMemoryGraphicsItem* connectionEndItem,
    quint64 remappedAddress, quint64 memoryMapBaseAddress, bool hasRemappedRange)
{
    quint64 addressRangeAddition = remappedAddress;
    if (!hasRemappedRange)
    {
        addressRangeAddition = addressRangeAddition - memoryMapBaseAddress;
    }

    connectionEndItem->changeAddressRange(addressRangeAddition);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::compressGraphicsItems()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::compressGraphicsItems(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems, int& spaceYPlacement,
    MemoryColumn* spaceColumn)
{
    QSharedPointer<QVector<MemoryConnectionItem*> > movedConnectionItems (new QVector<MemoryConnectionItem*>());

    QVector<MemoryColumn*> memoryColumns = columnHandler_->getMemoryColumns();
    for (int columnIndex = memoryColumns.size() - 1; columnIndex >= 0; columnIndex--)
    {
        MemoryColumn* memoryColumn = memoryColumns.at(columnIndex);

        if (memoryColumn->name().contains(MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME, Qt::CaseInsensitive))
        {
            memoryColumn->compressGraphicsItems(
                condenseMemoryItems_, spaceYPlacement, spaceColumn, placedSpaceItems, movedConnectionItems);
        }
    }

    for (int columnIndex = memoryColumns.size() - 1; columnIndex >= 0; columnIndex--)
    {
        MemoryColumn* memoryColumn = memoryColumns.at(columnIndex);

        if (memoryColumn->name().contains(MemoryDesignerConstants::MEMORYMAPCOLUMNCOMMON_NAME, Qt::CaseInsensitive))
        {
            memoryColumn->compressGraphicsItems(
                condenseMemoryItems_, spaceYPlacement, spaceColumn, placedSpaceItems, movedConnectionItems);
        }
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

            qreal itemHeight = mapRectangle.height();
            qreal extensionHeight = 0;

            if (mapItem->hasExtensionItem())
            {
                extensionHeight = mapItem->getExtensionItem()->boundingRect().height();
            }

            qreal subItemsHeight = mapItem->getSubItemHeightAddition();
            if (subItemsHeight > extensionHeight)
            {
                extensionHeight = subItemsHeight;
            }

            mapRectangle.setHeight(itemHeight + extensionHeight);

            QVector<MainMemoryGraphicsItem*> connectedSpaceItems = mapItem->getChainedSpaceItems();

            quint64 firstConnectionBaseAddress = mapItem->getLowestConnectedBaseAddress();
            quint64 lastConnectionLastAddress = mapItem->getHighestConnectedLastAddress();

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
    QRectF itemRectangle = memoryItem->getSceneRectangleWithSubItems();
    if (!memoryItem->getMemoryConnections().isEmpty())
    {
        MemoryConnectionItem* lastConnection = memoryItem->getLastConnection();
        qreal lastConnectionLow = lastConnection->sceneBoundingRect().bottom();
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
