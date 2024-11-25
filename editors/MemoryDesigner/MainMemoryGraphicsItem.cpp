//-----------------------------------------------------------------------------
// File: MainMemoryGraphicsItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 16.09.2016
//
// Description:
// Parent class for memory map and address space graphics items in memory designer.
//-----------------------------------------------------------------------------

#include "MainMemoryGraphicsItem.h"

#include <editors/common/diagramgrid.h>

#include <editors/MemoryDesigner/MemoryItem.h>
#include <editors/MemoryDesigner/MemoryColumn.h>
#include <editors/MemoryDesigner/MemoryConnectionItem.h>
#include <editors/MemoryDesigner/MemoryCollisionItem.h>
#include <editors/MemoryDesigner/MemoryDesignerChildGraphicsItem.h>
#include <editors/MemoryDesigner/MemoryExtensionGraphicsItem.h>
#include <editors/MemoryDesigner/MemoryDesignerConstants.h>
#include <editors/MemoryDesigner/ConnectivityComponent.h>

#include <QFont>
#include <QMenu>
#include <QGraphicsSceneEvent>

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::MainMemoryGraphicsItem()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem::MainMemoryGraphicsItem(QSharedPointer<MemoryItem const> memoryItem,
    QSharedPointer<ConnectivityComponent const> containingInstance, QString const& subItemType,
    bool filterSubItems, QVector<QString> identifierChain, QGraphicsItem* parent):
MemoryDesignerGraphicsItem(memoryItem->getName(), memoryItem->getDisplayName(), identifierChain,
    containingInstance, parent),
SubMemoryLayout(memoryItem, subItemType, filterSubItems, this),
instanceNameLabel_(new QGraphicsTextItem(containingInstance->getName(), this)),
memoryItem_(memoryItem),
memoryCollisions_(),
compressed_(false),
extensionItem_(0)
{
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemSendsScenePositionChanges);

    QFont labelFont = getNameLabel()->font();
    instanceNameLabel_->setFont(labelFont);
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::getInstanceNameLabel()
//-----------------------------------------------------------------------------
QGraphicsTextItem* MainMemoryGraphicsItem::getInstanceNameLabel() const
{
    return instanceNameLabel_;
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::moveItemAndConnectedItems()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::moveItemAndConnectedItems(qreal yTransfer)
{
    moveBy(0, yTransfer);

    QSharedPointer<QVector<MainMemoryGraphicsItem*> > visitedMemories (new QVector<MainMemoryGraphicsItem*> ());
    visitedMemories->append(this);

    QMultiMap<quint64, MemoryConnectionItem*> allConnectedConnections =
        getAllConnectionsFromConnectedItems(visitedMemories);

    QSharedPointer<QVector<MainMemoryGraphicsItem*> > movedItems(new QVector<MainMemoryGraphicsItem*>());

    QMultiMapIterator<quint64, MemoryConnectionItem*> connectionIterator(allConnectedConnections);
    while (connectionIterator.hasNext())
    {
        connectionIterator.next();
        MemoryConnectionItem* connectionItem = connectionIterator.value();

        connectionItem->moveConnectedItems(movedItems, this, yTransfer);
    }
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::addMemoryConnection()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::addMemoryConnection(MemoryConnectionItem* connectionItem)
{
    MemoryDesignerGraphicsItem::addMemoryConnection(connectionItem);

    addConnectionToSubItems(connectionItem);
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::getLastConnection()
//-----------------------------------------------------------------------------
MemoryConnectionItem* MainMemoryGraphicsItem::getLastConnection() const
{
    MemoryConnectionItem* lastConnection = 0;
    quint64 lastConnectionRangeEnd = 0;

    QMultiMapIterator<quint64, MemoryConnectionItem*> connectionIterator(getMemoryConnections());
    while (connectionIterator.hasNext())
    {
        connectionIterator.next();

        MemoryConnectionItem* connection = connectionIterator.value();
        quint64 connectionRangeEnd = connection->getRangeEndValue();
        if (connectionRangeEnd >= lastConnectionRangeEnd)
        {
            lastConnection = connection;
            lastConnectionRangeEnd = connectionRangeEnd;
        }
    }

    return lastConnection;
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::getFirstConnection()
//-----------------------------------------------------------------------------
MemoryConnectionItem* MainMemoryGraphicsItem::getFirstConnection() const
{
    MemoryConnectionItem* firstConnection = 0;

    if (!getMemoryConnections().isEmpty())
    {
        firstConnection = getMemoryConnections().first();
        quint64 firstConnectionRangeStart = firstConnection->getRangeStartValue();

        QMultiMapIterator<quint64, MemoryConnectionItem*> connectionIterator(getMemoryConnections());
        while (connectionIterator.hasNext())
        {
            connectionIterator.next();

            MemoryConnectionItem* connection = connectionIterator.value();
            quint64 connectionRangeStart = connection->getRangeStartValue();
            if (connectionRangeStart <= firstConnectionRangeStart)
            {
                firstConnection = connection;
                firstConnectionRangeStart = connectionRangeStart;
            }
        }
    }

    return firstConnection;
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::isCompressed()
//-----------------------------------------------------------------------------
bool MainMemoryGraphicsItem::isCompressed() const
{
    return compressed_;
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::setCompressed()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::setCompressed(bool newCompressValue)
{
    compressed_ = newCompressValue;
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::setExtensionItem()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::setExtensionItem(MemoryExtensionGraphicsItem* newExtensionItem)
{
    extensionItem_ = newExtensionItem;

    connect(extensionItem_, SIGNAL(openComponentDocument(const VLNV&, QVector<QString>)),
        this, SIGNAL(openComponentDocument(const VLNV&, QVector<QString>)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::getExtensionItem()
//-----------------------------------------------------------------------------
MemoryExtensionGraphicsItem* MainMemoryGraphicsItem::getExtensionItem() const
{
    return extensionItem_;
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::hasExtensionItem()
//-----------------------------------------------------------------------------
bool MainMemoryGraphicsItem::hasExtensionItem() const
{
    if (extensionItem_)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::getMemoryItem()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryItem const> MainMemoryGraphicsItem::getMemoryItem() const
{
    return memoryItem_;
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::getItemWidth()
//-----------------------------------------------------------------------------
qreal MainMemoryGraphicsItem::getItemWidth() const
{
    qreal itemWidth = boundingRect().width() - 1;
    if (!subItemsAreFiltered())
    {
        qreal subItemWidth = getSubMemoryItems().first()->boundingRect().width();
        itemWidth = itemWidth - subItemWidth;
    }

    return itemWidth;
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::labelCollidesWithRangeLabels()
//-----------------------------------------------------------------------------
bool MainMemoryGraphicsItem::labelCollidesWithRangeLabels(QGraphicsTextItem* label, qreal fontHeight) const
{
    foreach (MemoryConnectionItem* connection, getMemoryConnections())
    {
        if (connection->labelCollidesWithRanges(label, fontHeight, this))
        {
            return true;
        }
    }

    return MemoryDesignerGraphicsItem::labelCollidesWithRangeLabels(label, fontHeight);
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::createOverlappingConnectionMarkers()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::createOverlappingConnectionMarkers()
{
    auto memConnections = getMemoryConnections();

    if (memConnections.size() < 2)
        return;

    for (auto const& selectedItem : memConnections)
    {
        for (auto const& comparisonItem : memConnections)
        {
            if (!selectedItem || !comparisonItem || selectedItem == comparisonItem)
            {
                continue;
            }

            QRectF connectionRect = selectedItem->sceneBoundingRect();
            QRectF comparisonRect = comparisonItem->sceneBoundingRect();

            if (selectedItem->getConnectionStartItem() == this &&
                comparisonItem->getConnectionStartItem() == this &&
                MemoryDesignerConstants::itemOverlapsAnotherItem(connectionRect,
                    selectedItem->pen().width(), comparisonRect, comparisonItem->pen().width()) &&
                connectionRect.width() < comparisonRect.width())
            {
                MemoryCollisionItem* newCollisionItem =
                    new MemoryCollisionItem(selectedItem, comparisonItem, scene());
                memoryCollisions_.append(newCollisionItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::getMemoryCollisions()
//-----------------------------------------------------------------------------
QVector<MemoryCollisionItem*> MainMemoryGraphicsItem::getMemoryCollisions() const
{
    return memoryCollisions_;
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::isConnectedToSpaceItems()
//-----------------------------------------------------------------------------
bool MainMemoryGraphicsItem::isConnectedToSpaceItems(QVector<MainMemoryGraphicsItem*> spaceItems) const
{
    foreach (MemoryConnectionItem* connectionItem, getMemoryConnections())
    {
        MainMemoryGraphicsItem* connectedSpaceItem = connectionItem->getConnectionStartItem();
        if (connectedSpaceItem && connectedSpaceItem != this &&
            (spaceItems.contains(connectedSpaceItem) || connectedSpaceItem->isConnectedToSpaceItems(spaceItems)))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::getChainedSpaceItems()
//-----------------------------------------------------------------------------
QVector<MainMemoryGraphicsItem*> MainMemoryGraphicsItem::getChainedSpaceItems() const
{
    QVector<MainMemoryGraphicsItem*> chainedSpaces;

    foreach (MemoryConnectionItem* connectionItem, getMemoryConnections())
    {
        MainMemoryGraphicsItem* connectedSpaceItem = connectionItem->getConnectionStartItem();
        if (connectedSpaceItem && connectedSpaceItem != this)
        {
            chainedSpaces.append(connectedSpaceItem);
            foreach (MainMemoryGraphicsItem* chainedSpaceItem, connectedSpaceItem->getChainedSpaceItems())
            {
                chainedSpaces.append(chainedSpaceItem);
            }
        }
    }

    return chainedSpaces;
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::compressToUnCutCoordinates()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::compressToUnCutCoordinates(QVector<qreal> unCutCoordinates, const qreal CUTMODIFIER,
    bool memoryItemsAreCompressed)
{
    if (!subItemsAreFiltered())
    {
        compressSubItemsToUnCutCoordinates(unCutCoordinates, CUTMODIFIER, memoryItemsAreCompressed);
    }

    MemoryDesignerGraphicsItem::compressToUnCutCoordinates(
        unCutCoordinates, CUTMODIFIER, memoryItemsAreCompressed);
    setCompressed(true);
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::changeAddressRange()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::changeAddressRange(quint64 offsetChange)
{
    MemoryDesignerGraphicsItem::changeAddressRange(offsetChange);

    SubMemoryLayout::changeChildItemRanges(offsetChange);
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::getAllConnectionsFromConnectedItems()
//-----------------------------------------------------------------------------
QMultiMap<quint64, MemoryConnectionItem*> MainMemoryGraphicsItem::getAllConnectionsFromConnectedItems(
    QSharedPointer<QVector<MainMemoryGraphicsItem*> > visitedMemoryItems) const
{
    QMultiMap<quint64, MemoryConnectionItem*> allConnections = getMemoryConnections();
    QMultiMapIterator<quint64, MemoryConnectionItem*> connectionIterator (allConnections);
    while (connectionIterator.hasNext())
    {
        connectionIterator.next();
        auto currentConnection = connectionIterator.value();

        MainMemoryGraphicsItem* connectedItem = currentConnection->getConnectionStartItem();
        if (connectedItem == this)
        {
            connectedItem = currentConnection->getConnectionEndItem();
        }

        if (connectedItem && !visitedMemoryItems->contains(connectedItem))
        {
            visitedMemoryItems->append(connectedItem);
            
            QMultiMap<quint64, MemoryConnectionItem*> secondConnections =
                connectedItem->getAllConnectionsFromConnectedItems(visitedMemoryItems);
            QMultiMapIterator<quint64, MemoryConnectionItem*> secondConnectionIterator (secondConnections);
            while (secondConnectionIterator.hasNext())
            {
                secondConnectionIterator.next();
                MemoryConnectionItem* secondConnectionItem = secondConnectionIterator.value();

                if (!allConnections.values().contains(secondConnectionItem))
                {
                    quint64 secondConnectionBaseAddress = secondConnectionIterator.key();
                    if (allConnections.keys().contains(secondConnectionBaseAddress))
                    {
                        QVector<MemoryConnectionItem*> sameKeyItems =
                            allConnections.values(secondConnectionBaseAddress).toVector();
                        sameKeyItems.append(secondConnectionItem);

                        int amountOfSameKeys = allConnections.remove(secondConnectionBaseAddress) + 1;
                        for (int connectionIndex = amountOfSameKeys - 1; connectionIndex >= 0; --connectionIndex)
                        {
                            MemoryConnectionItem* sameKeyConnection = sameKeyItems.at(connectionIndex);
                            allConnections.insert(secondConnectionBaseAddress, sameKeyConnection);
                        }
                    }
                    else
                    {
                        allConnections.insert(secondConnectionBaseAddress, secondConnectionItem);
                    }
                }
            }
        }
    }

    return allConnections;
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::getLowestPointOfConnectedItems()
//-----------------------------------------------------------------------------
qreal MainMemoryGraphicsItem::getLowestPointOfConnectedItems()
{
    qreal lowestPoint = sceneBoundingRect().bottom();

    QSharedPointer<QVector<MainMemoryGraphicsItem*> > visitedItems (new QVector<MainMemoryGraphicsItem*>());
    QMultiMap<quint64, MemoryConnectionItem*> allConnections = getAllConnectionsFromConnectedItems(visitedItems);

    visitedItems->clear();

    QMultiMapIterator<quint64, MemoryConnectionItem*> connectionIterator (allConnections);
    while (connectionIterator.hasNext())
    {
        connectionIterator.next();

        auto connectionItem = connectionIterator.value();
        qreal connectionLowPoint = connectionItem->getConnectionLowPoint(this);
        if (connectionLowPoint > lowestPoint)
        {
            lowestPoint = connectionLowPoint;
        }
    }

    return lowestPoint;
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::getLowestConnectedBaseAddress()
//-----------------------------------------------------------------------------
quint64 MainMemoryGraphicsItem::getLowestConnectedBaseAddress() const
{
    quint64 lowestBaseAddress = getBaseAddress();
    if (!getMemoryConnections().isEmpty())
    {
        quint64 lowestConnectionBaseAddress = getFirstConnection()->getRangeStartValue();
        if (lowestConnectionBaseAddress < lowestBaseAddress)
        {
            lowestBaseAddress = lowestConnectionBaseAddress;
        }
    }

    return lowestBaseAddress;
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::getHighestConnectedLastAddress()
//-----------------------------------------------------------------------------
quint64 MainMemoryGraphicsItem::getHighestConnectedLastAddress() const
{
    quint64 highestLastAddress = getLastAddress();
    if (!getMemoryConnections().isEmpty())
    {
        quint64 highestConnectionLastAddress = getLastConnection()->getRangeEndValue();
        if (highestConnectionLastAddress > highestLastAddress)
        {
            highestLastAddress = highestConnectionLastAddress;
        }
    }

    return highestLastAddress;
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::getUncutCoordinatesFromSet()
//-----------------------------------------------------------------------------
QVector<qreal> MainMemoryGraphicsItem::getUncutCoordinatesFromSet(QVector<MainMemoryGraphicsItem*>& visitedItems, QVector<MemoryConnectionItem*>& visitedConnections)
{
    visitedItems.append(this);

    QVector<qreal> uncutCoordinates;
    for (auto coordinate : getUnCutCoordinates())
    {
        if (uncutCoordinates.contains(coordinate) == false)
        {
            uncutCoordinates.append(coordinate);
        }
    }

    QMultiMapIterator connectionIterator(getMemoryConnections());
    while (connectionIterator.hasNext())
    {
        connectionIterator.next();
        MemoryConnectionItem* connectionItem = connectionIterator.value();

        if (visitedConnections.contains(connectionItem) == false)
        {
            visitedConnections.append(connectionItem);

            if (auto connectionTop = connectionItem->sceneBoundingRect().top(); uncutCoordinates.contains(connectionTop) == false)
            {
                uncutCoordinates.append(connectionTop);
            }
            if (auto connectionBottom = connectionItem->sceneBoundingRect().bottom(); uncutCoordinates.contains(connectionBottom) == false)
            {
                uncutCoordinates.append(connectionBottom);
            }

            auto startItem = connectionItem->getConnectionStartItem();
            auto endItem = connectionItem->getConnectionEndItem();
            if (startItem && startItem != this && !visitedItems.contains(startItem))
            {
                for (auto coordinate : startItem->getUncutCoordinatesFromSet(visitedItems, visitedConnections))
                {
                    if (uncutCoordinates.contains(coordinate) == false)
                    {
                        uncutCoordinates.append(coordinate);
                    }
                }
            }
            if (endItem && endItem != this && !visitedItems.contains(endItem))
            {
                for (auto coordinate : endItem->getUncutCoordinatesFromSet(visitedItems, visitedConnections))
                {
                    if (uncutCoordinates.contains(coordinate) == false)
                    {
                        uncutCoordinates.append(coordinate);
                    }
                }
            }
        }
    }

    std::sort(uncutCoordinates.begin(), uncutCoordinates.end());
    return uncutCoordinates;
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::extendMemoryItem()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::extendMemoryItem()
{
    auto firstConnection = getFirstConnection();
    auto lastConnection = getLastConnection();

    if (firstConnection && lastConnection)
    {
        QPointF itemTopLeft = boundingRect().topLeft();
        QPointF itemLowRight = boundingRect().bottomRight();

        int lineWidth = pen().width();

        qreal connectionTop =
            mapFromItem(firstConnection, firstConnection->boundingRect().topLeft()).y() + lineWidth;
        qreal connectionLow =
            mapFromItem(lastConnection, lastConnection->boundingRect().bottomRight()).y();

        bool connectionsAreBeyond = false;

        qreal extensionTop = itemLowRight.y();
        if (connectionTop < itemTopLeft.y())
        {
            extensionTop = connectionTop;
            connectionsAreBeyond = true;
        }

        qreal extensionLow = itemTopLeft.y();
        if (connectionLow > itemLowRight.y())
        {
            extensionLow = connectionLow;
            connectionsAreBeyond = true;
        }

        if (connectionsAreBeyond)
        {
            qreal positionX = itemTopLeft.x() + lineWidth;
            qreal extensionWidth = itemLowRight.x() - itemTopLeft.x() - 2*lineWidth;
            qreal positionY = extensionTop - lineWidth;
            qreal extensionHeight = extensionLow - extensionTop;

            auto extensionItem = new MemoryExtensionGraphicsItem(positionX, positionY, extensionWidth, extensionHeight, getContainingInstance(), this);
            setExtensionItem(extensionItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::compressItemAndChildItems()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::compressItemAndChildItems(bool compressMemoryItems)
{
    if (!isCompressed())
    {
        qreal memoryMapNewHeight = 0;
        qreal subItemHeight = getMinimumHeightForSubItems();

        if (getMemoryConnections().isEmpty())
        {
            memoryMapNewHeight =
                compressChildItems(getBaseAddress(), getLastAddress(), subItemHeight, compressMemoryItems);
        }

        if (memoryMapNewHeight > 0)
        {
            condense(memoryMapNewHeight);

            setCompressed(true);
        }
    }
}