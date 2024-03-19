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

    QVector<QVector<QSharedPointer<ConnectivityInterface const> > > masterSlavePaths =
        pathSearcher.findMasterSlavePaths(connectionGraph, false);

    qreal spaceYPlacement = MemoryDesignerConstants::SPACEITEMINTERVAL;

    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems(
        new QVector<MainMemoryGraphicsItem*> ());
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems(
        new QVector<MainMemoryGraphicsItem*> ());

    for(auto const& singlePath : masterSlavePaths)
    {
        if (!singlePath.isEmpty())
        {
            createConnection(
                singlePath, placedMapItems, memoryMapColumn, spaceYPlacement, placedSpaceItems, spaceColumn);
        }
    }

    compressGraphicsItems(placedSpaceItems, spaceYPlacement, spaceColumn);
    repositionOverlappingSpaceItems(placedSpaceItems);

    if (condenseMemoryItems_)
    {
        repositionCompressedMemoryMaps(placedMapItems, memoryMapColumn);
    }

    moveUnconnectedMemoryItems(placedSpaceItems, spaceColumn);
    moveUnconnectedMemoryItems(placedMapItems, memoryMapColumn);

    reDrawConnections(placedSpaceItems);

    createOverlappingConnectionMarkers(placedSpaceItems);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::createConnection()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::createConnection(
    QVector<QSharedPointer<ConnectivityInterface const> > const& connectionPath,
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedMapItems, MemoryColumn* memoryMapColumn,
    qreal& spaceYPlacement, QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
    MemoryColumn* spaceColumn)
{
    QSharedPointer<ConnectivityInterface const> startInterface = getStartInterface(connectionPath);
    QSharedPointer<ConnectivityInterface const> endInterface = connectionPath.last();

    MainMemoryGraphicsItem* connectionStartItem =
        getMainGraphicsItem(startInterface, MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME);

    MainMemoryGraphicsItem* connectionEndItem = getConnectionEndItem(startInterface, endInterface);

    if (!connectionStartItem || !connectionEndItem)
    {
        return;
    }

    if (!placedSpaceItems->contains(connectionStartItem))
    {
        if (placedMapItems->contains(connectionEndItem))
        {
            quint64 endBase = connectionEndItem->getBaseAddress();

            qreal endPosition =
                connectionEndItem->scenePos().y() - endBase * MemoryDesignerConstants::RANGEINTERVAL;

            spaceColumn->setGraphicsItemPosition(connectionStartItem, endPosition);

            qreal newSpaceYPlacement = endPosition + connectionStartItem->getHeightWithSubItems() +
                MemoryDesignerConstants::SPACEITEMINTERVAL;
            if (newSpaceYPlacement > spaceYPlacement)
            {
                spaceYPlacement = newSpaceYPlacement;
            }
        }
        else
        {
            spaceColumn->setGraphicsItemPosition(connectionStartItem, spaceYPlacement);

            spaceYPlacement +=
                connectionStartItem->getHeightWithSubItems() + MemoryDesignerConstants::SPACEITEMINTERVAL;
        }
    }

    MemoryConnectionAddressCalculator::ConnectionPathVariables pathVariables =
        MemoryConnectionAddressCalculator::calculatePathAddresses(startInterface, endInterface, connectionPath);

    QVector<MainMemoryGraphicsItem*> spaceChain;
    spaceChain.append(connectionStartItem);
    if (!filterAddressSpaceChains_)
    {
        spaceChain = createSpaceChain(connectionStartItem, spaceColumn, pathVariables.spaceChain_, connectionPath,
            placedSpaceItems, spaceYPlacement);
        if (!spaceChain.isEmpty())
        {
            connectionStartItem = spaceChain.last();
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

    qreal yTransfer = getConnectionInitialTransferY(pathVariables.baseAddressNumber_,
        pathVariables.mirroredSlaveAddressChange_, pathVariables.hasRemapRange_,
        pathVariables.memoryMapBaseAddress_, pathVariables.spaceChainBaseAddress_);

    if (!placedMapItems->contains(connectionEndItem))
    {
        placeMemoryMapItem(pathVariables.remappedAddress_, pathVariables.remappedEndAddress_, connectionEndItem,
            connectionStartItem, yTransfer, pathVariables.hasRemapRange_, pathVariables.memoryMapBaseAddress_,
            spaceItemPlaced, pathVariables.isChainedSpaceConnection_, placedMapItems, placedSpaceItems,
            memoryMapColumn, spaceColumn, spaceYPlacement);
    }
    else
    {
        placeSpaceItemToOtherColumn(
            connectionStartItem, spaceChain, spaceColumn, connectionEndItem, pathVariables.remappedAddress_);

        spaceYPlacement = spaceYPlacement - (connectionStartItem->getHeightWithSubItems() +
            MemoryDesignerConstants::SPACEITEMINTERVAL);
    }

    createMemoryConnectionItem(connectionPath, connectionStartItem, connectionEndItem,
        pathVariables.remappedAddress_, pathVariables.remappedEndAddress_, pathVariables.memoryMapBaseAddress_,
        pathVariables.hasRemapRange_, yTransfer, spaceYPlacement);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::getStartInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ConnectivityInterface const> MemoryConnectionHandler::getStartInterface(
    QVector<QSharedPointer<ConnectivityInterface const> > connectionPath)
{
    foreach (QSharedPointer<ConnectivityInterface const> pathInterface, connectionPath)
    {
        if ((pathInterface->getMode() == General::MASTER || pathInterface->getMode() == General::INITIATOR) && pathInterface->getConnectedMemory())
        {
            return pathInterface;
        }
    }

    return connectionPath.first();
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::getMainGraphicsItem()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryConnectionHandler::getMainGraphicsItem(
    QSharedPointer<ConnectivityInterface const> connectionInterface, QString columnType) const
{
    MainMemoryGraphicsItem* graphicsItem = 0;

    QSharedPointer<MemoryItem const> memoryItem = connectionInterface->getConnectedMemory();
    QSharedPointer<ConnectivityComponent const> connectionInstance = connectionInterface->getInstance();
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
    QSharedPointer<ConnectivityInterface const> startInterface,
    QSharedPointer<ConnectivityInterface const> endInterface) const
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
quint64 MemoryConnectionHandler::getStartingBaseAddress(QSharedPointer<ConnectivityInterface const> startInterface,
    QSharedPointer<ConnectivityInterface const> endInterface) const
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
    MemoryColumn* spaceColumn, qreal& spaceYPlacement)
{
    qreal memoryMapYTransfer = yTransfer;
    if (hasRemapRange)
    {
        memoryMapYTransfer += (memoryMapBaseAddress * MemoryDesignerConstants::RANGEINTERVAL);
    }

    qreal newEndItemPositionY = connectionStartItem->pos().y() + memoryMapYTransfer;
    connectionEndItem->setY(newEndItemPositionY);

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
void MemoryConnectionHandler::createMemoryConnectionItem(
    QVector<QSharedPointer<ConnectivityInterface const> > connectionPath,
    MainMemoryGraphicsItem* connectionStartItem, MainMemoryGraphicsItem* connectionEndItem,
    quint64 remappedAddress, quint64 remappedEndAddress, quint64 memoryMapBaseAddress, bool hasRemapRange,
    qreal yTransfer, qreal& spaceYPlacement)
{
    changeConnectionEndItemRanges(connectionEndItem, remappedAddress, memoryMapBaseAddress, hasRemapRange);

    MemoryConnectionItem* newConnectionItem = new MemoryConnectionItem(connectionStartItem,
        remappedAddress, remappedEndAddress, connectionEndItem, connectionStartItem->scene(), yTransfer);
    connectionsToMemoryMaps_.append(newConnectionItem);

    connectionStartItem->hideCollidingRangeLabels(remappedAddress, remappedEndAddress);
    connectionEndItem->hideCollidingRangeLabels(remappedAddress, remappedEndAddress);

    qreal placementOfNextSpace = connectionStartItem->getLowestPointOfConnectedItems() +
        MemoryDesignerConstants::SPACEITEMINTERVAL;

    if (placementOfNextSpace > spaceYPlacement)
    {
        spaceYPlacement = placementOfNextSpace;
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::getLocalMemoryMapItem()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryConnectionHandler::getLocalMemoryMapItem(
    QSharedPointer<ConnectivityInterface const> spaceInterface) const
{
    MainMemoryGraphicsItem* localMapItem = 0;

    QSharedPointer<MemoryItem> memoryItemForLocalMap =
        MemoryDesignerConstants::getMemoryItemForLocalMap(spaceInterface);
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
// Function: MemoryConnectionHandler::createSpaceChain()
//-----------------------------------------------------------------------------
QVector<MainMemoryGraphicsItem*> MemoryConnectionHandler::createSpaceChain(
    MainMemoryGraphicsItem* connectionStartItem, MemoryColumn* spaceColumn,
    QVector<MemoryConnectionAddressCalculator::ChainedSpace> spaceChain,
    QVector<QSharedPointer<const ConnectivityInterface>> connectionPath,
    QSharedPointer<QVector<MainMemoryGraphicsItem *>> placedSpaceItems, qreal& spaceYPlacement)
{
    QVector<MainMemoryGraphicsItem*> spaceItemChain;
    spaceItemChain.append(connectionStartItem);

    //! Multiple chains not working...

    MainMemoryGraphicsItem* spaceChainStartItem = connectionStartItem;

    MemoryConnectionAddressCalculator::ChainedSpace previousChainedSpace;

    for(auto const& chainedSpace : spaceChain)
    {
        MainMemoryGraphicsItem* connectionMiddleItem = getMainGraphicsItem(
            chainedSpace.spaceInterface_, MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME);

        if (connectionMiddleItem != spaceChainStartItem)
        {
            connectionMiddleItem->changeAddressRange(chainedSpace.spaceConnectionBaseAddress_);

            spaceItemChain.append(connectionMiddleItem);

            createSpaceConnection(connectionPath, spaceChainStartItem,
                chainedSpace.spaceConnectionBaseAddress_, connectionMiddleItem, spaceColumn, placedSpaceItems,
                spaceItemChain, spaceYPlacement);
        }

        previousChainedSpace = chainedSpace;
        spaceChainStartItem = connectionMiddleItem;
    }

    return spaceItemChain;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::createSpaceConnection()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::createSpaceConnection(
    QVector<QSharedPointer<ConnectivityInterface const> > connectionPath,
    MainMemoryGraphicsItem* connectionStartItem, quint64 connectionBaseAddress,
    MainMemoryGraphicsItem* connectionMiddleItem, MemoryColumn* spaceColumn,
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems,
    QVector<MainMemoryGraphicsItem*> spaceItemChain, qreal& spaceYPlacement)
{
    if (connectionMiddleItem)
    {
        qreal startItemPositionY = connectionStartItem->pos().y();
        if (!placedSpaceItems->contains(connectionMiddleItem))
        {
            placeSpaceItem(
                connectionMiddleItem, startItemPositionY, spaceColumn, placedSpaceItems, connectionStartItem);

            connectionMiddleItem->hideFirstAndLastSegmentRange();
        }

        bool recalculateYPosition = false;
        if (!placedSpaceItems->contains(connectionMiddleItem))
        {
            quint64 middleItemY = connectionMiddleItem->pos().y();

            changeMasterAddressSpaceColumn(connectionStartItem, middleItemY, spaceColumn, spaceItemChain);

            if (!placedSpaceItems->contains(connectionStartItem))
            {
                placedSpaceItems->append(connectionStartItem);
            }
            recalculateYPosition = true;
        }

        quint64 startItemBaseAddress = connectionStartItem->getOriginalBaseAddress();

        quint64 middleItemRangeStart = startItemBaseAddress + connectionBaseAddress;
        quint64 middleItemRangeEnd = middleItemRangeStart +
            (connectionMiddleItem->getLastAddress() - connectionMiddleItem->getBaseAddress());
        qreal yTransfer = (startItemBaseAddress + connectionBaseAddress - connectionStartItem->getBaseAddress()) *
            MemoryDesignerConstants::RANGEINTERVAL;

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
void MemoryConnectionHandler::placeSpaceItem(MainMemoryGraphicsItem* spaceItem, qreal positionY,
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

    originalColumn->setGraphicsItemPosition(spaceItem, positionY);

    positionY += spaceItem->getHeightWithSubItems() + MemoryDesignerConstants::SPACEITEMINTERVAL;
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
    
    while (currentColumnPosition >= 0)
    {
        MemoryColumn* currentColumn = columnHandler_->findColumnAtPosition(QPointF(currentColumnPosition, 0));
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
    QVector<MainMemoryGraphicsItem*> spaceChain, MemoryColumn* originalColumn, MainMemoryGraphicsItem* targetItem,
    quint64 connectionBaseAddress)
{
    originalColumn->removeItem(spaceItem);

    QRectF spaceRectangle = spaceItem->getSceneRectangleWithSubItems();
    qreal spaceHeight = spaceRectangle.height();
    int spaceLineWidth = spaceItem->pen().width();

    qreal itemPositionDifference = connectionBaseAddress * MemoryDesignerConstants::RANGEINTERVAL;
    qreal connectedItemY = targetItem->scenePos().y();
    qreal newItemPosition = connectedItemY - itemPositionDifference;
    if (newItemPosition < MemoryDesignerConstants::SPACEITEMINTERVAL)
    {
        qreal connectedItemTransfer = MemoryDesignerConstants::SPACEITEMINTERVAL - newItemPosition;
        targetItem->moveItemAndConnectedItems(connectedItemTransfer);

        newItemPosition = MemoryDesignerConstants::SPACEITEMINTERVAL;
    }

    if (newItemPosition > MemoryDesignerConstants::SPACEITEMINTERVAL)
    {
        spaceItem->moveItemAndConnectedItems(newItemPosition - MemoryDesignerConstants::SPACEITEMINTERVAL);
    }

    if (spaceChain.size() == 1)
    {
        spaceItem->setY(newItemPosition);
        spaceRectangle.setY(newItemPosition);
        spaceRectangle.setHeight(spaceHeight);
    }

    QVector<MemoryColumn*> spaceColumns = columnHandler_->getAddressSpaceColumns();
    for (int i = 0; i < spaceColumns.size(); i++)
    {
        MemoryColumn* currentColumn = spaceColumns.at(i);
        if (!currentColumn->itemOverlapsAnotherColumnItem(spaceItem, spaceRectangle, spaceLineWidth))
        {
            currentColumn->addItem(spaceItem);
            return;
        }
    }

    MemoryColumn* newSpaceColumn = columnHandler_->createAddressSpaceColumn();
    newSpaceColumn->addItem(spaceItem);
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
// Function: MemoryConnectionHandler::moveUnconnectedMemoryItems()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::moveUnconnectedMemoryItems(QSharedPointer<QVector<MainMemoryGraphicsItem*> > memoryItems, MemoryColumn* memoryColumn)
{
    memoryColumn->moveUnconnectedMemoryItems(memoryItems);
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
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > placedSpaceItems, qreal& spaceYPlacement,
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

    while (currentColumnPosition >= 0)
    {
        MemoryColumn* currentColumn = columnHandler_->findColumnAtPosition(QPointF(currentColumnPosition, 0));
        if (currentColumn &&
            currentColumn->name().contains(MemoryDesignerConstants::ADDRESSSPACECOLUMN_NAME, Qt::CaseInsensitive))
        {
            spaceRectangle.setX(currentColumnPosition);
/*
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
*/
            if (!currentColumn->itemOverlapsAnotherColumnItem(spaceItem, spaceRectangle, spaceLineWidth))
            {
                currentColumn->addItem(spaceItem);
                return;
            }
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
    }

    if (condenseMemoryItems_)
    {
        repositionCompressedMemoryMaps(placedMapItems, memoryMapColumn);
    }

    spaceColumn->compressUnconnectedMemoryItems(condenseMemoryItems_, placedSpaceItems);
    memoryMapColumn->compressUnconnectedMemoryItems(condenseMemoryItems_, placedMapItems);

    moveUnconnectedMemoryItems(placedSpaceItems, spaceColumn);
    moveUnconnectedMemoryItems(placedMapItems, memoryMapColumn);

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
        QQueue<int> setIndexes = findPathSetIndexes(currentPath, pathSets);
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
QVector<MemoryConnectionItem*> MemoryConnectionHandler::createConnectionSet(QSharedPointer<QVector<Path>> connectionSet,
    QSharedPointer<QVector<MainMemoryGraphicsItem*>> placedMapItems,
    MemoryColumn* memoryMapColumn,
    qreal& spaceYPlacement,
    QSharedPointer<QVector<MainMemoryGraphicsItem*>> placedSpaceItems,
    MemoryColumn* spaceColumn)
{
    for (auto const& connectionPath : *connectionSet)
    {
        createConnectionFromSet(connectionPath, placedMapItems, memoryMapColumn, placedSpaceItems, spaceColumn);
    }

    if (placedSpaceItems->isEmpty())
    {
        return QVector<MemoryConnectionItem*>();
    }

    MainMemoryGraphicsItem* highestItemInSet = placedSpaceItems->first();
    highestItemInSet = getHighestPlacedItemInSet(highestItemInSet, placedSpaceItems);
    highestItemInSet = getHighestPlacedItemInSet(highestItemInSet, placedMapItems);

    qreal yMovement = spaceYPlacement - highestItemInSet->scenePos().y();
    highestItemInSet->moveItemAndConnectedItems(yMovement);

    QVector<MemoryConnectionItem*> connections = compressConnectedMemoryItems(placedSpaceItems, placedMapItems);

    spaceYPlacement = highestItemInSet->getLowestPointOfConnectedItems() + MemoryDesignerConstants::SPACEITEMINTERVAL;

    return connections;
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::getHighestPlacedItemInSet()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryConnectionHandler::getHighestPlacedItemInSet(MainMemoryGraphicsItem* currentHighestItem, QSharedPointer<QVector<MainMemoryGraphicsItem*> > itemSet)
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
// Function: MemoryConnectionHandler::createConnectionFromSet()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::createConnectionFromSet(Path const& connectionPath,
    QSharedPointer<QVector<MainMemoryGraphicsItem *>> placedMapItems,
    MemoryColumn* memoryMapColumn,
    QSharedPointer<QVector<MainMemoryGraphicsItem *>> placedSpaceItems,
    MemoryColumn* spaceColumn)
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

    MemoryConnectionAddressCalculator::ConnectionPathVariables pathVariables =
        MemoryConnectionAddressCalculator::calculatePathAddresses(startInterface, endInterface, connectionPath);

    qreal yTransfer = getConnectionInitialTransferY(pathVariables.baseAddressNumber_,
        pathVariables.mirroredSlaveAddressChange_, pathVariables.hasRemapRange_,
        pathVariables.memoryMapBaseAddress_, pathVariables.spaceChainBaseAddress_);

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
            connectionStartItem, placedSpaceItems, spaceColumn, connectionEndItem, pathVariables.remappedAddress_);

        placedSpaceItems->append(connectionStartItem);
    }

    createConnectionItem(connectionStartItem, connectionEndItem,
        pathVariables.remappedAddress_, pathVariables.remappedEndAddress_, pathVariables.memoryMapBaseAddress_,
        pathVariables.hasRemapRange_, yTransfer);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::placeMemoryMap()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::placeMemoryMap(MainMemoryGraphicsItem* mapItem,
    MainMemoryGraphicsItem* startItem,
    qreal yTransfer,
    MemoryConnectionAddressCalculator::ConnectionPathVariables const& pathVariables,
    QSharedPointer<QVector<MainMemoryGraphicsItem *>> placedMapItems,
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
// Function: MemoryConnectionHandler::positionSpaceItem()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::positionSpaceItem(MainMemoryGraphicsItem* spaceItem,
    QSharedPointer<QVector<MainMemoryGraphicsItem *>> placedSpaceItems,
    MemoryColumn* originalColumn,
    MainMemoryGraphicsItem const* targetItem,
    quint64 connectionBaseAddress)
{
    originalColumn->removeItem(spaceItem);

    QRectF spaceRectangle = spaceItem->getSceneRectangleWithSubItems();
    qreal spaceHeight = spaceRectangle.height();
    int spaceLineWidth = spaceItem->pen().width();

    qreal itemPositionDifference = connectionBaseAddress * MemoryDesignerConstants::RANGEINTERVAL;
    qreal connectedItemY = targetItem->scenePos().y();
    qreal newItemPosition = connectedItemY - itemPositionDifference;

    spaceItem->setY(newItemPosition);
    spaceRectangle.setY(newItemPosition);
    spaceRectangle.setHeight(spaceHeight);

    placeSpaceItemToColumn(spaceItem, spaceRectangle, spaceLineWidth, placedSpaceItems);
}

//-----------------------------------------------------------------------------
// Function: MemoryConnectionHandler::placeSpaceItemToColumn()
//-----------------------------------------------------------------------------
void MemoryConnectionHandler::placeSpaceItemToColumn(MainMemoryGraphicsItem* spaceItem, QRectF const& spaceRectangle, int spaceLineWidth, QSharedPointer<QVector<MainMemoryGraphicsItem *>> placedSpaceItems)
{
    QVector<MemoryColumn*> spaceColumns = columnHandler_->getAddressSpaceColumns();
    for (int i = 0; i < spaceColumns.size(); i++)
    {
        MemoryColumn* currentColumn = spaceColumns.at(i);
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
// Function: MemoryConnectionHandler::compressConnectedMemoryItems()
//-----------------------------------------------------------------------------
QVector<MemoryConnectionItem*> MemoryConnectionHandler::compressConnectedMemoryItems(QSharedPointer<QVector<MainMemoryGraphicsItem *>> placedSpaceItems, QSharedPointer<QVector<MainMemoryGraphicsItem *>> placedMapItems)
{
    const qreal CUTMODIFIER = 3 * MemoryDesignerConstants::RANGEINTERVAL;

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
        connectionItem->compressToUnCutCoordinates(visitedItems, uncutCoordinates, CUTMODIFIER, condenseMemoryItems_);
    }

    QMap<qreal, qreal> areaMovements = calculateAreaMovements(uncutCoordinates, CUTMODIFIER);
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
