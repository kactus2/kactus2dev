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
// Function: MainMemoryGraphicsItem::~MainMemoryGraphicsItem()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem::~MainMemoryGraphicsItem()
{

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
// Function: MainMemoryGraphicsItem::reDrawConnections()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::reDrawConnections()
{
    foreach (MemoryConnectionItem* connection, getMemoryConnections())
    {
        if (connection->getConnectionStartItem() == this)
        {
            connection->reDrawConnection();
            connection->repositionCollidingRangeLabels();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::getSceneEndPoint()
//-----------------------------------------------------------------------------
quint64 MainMemoryGraphicsItem::getSceneEndPoint() const
{
    quint64 sceneEndPoint = sceneBoundingRect().bottom();

    QMultiMapIterator<quint64, MemoryConnectionItem*> connectionIterator(getMemoryConnections());
    while (connectionIterator.hasNext())
    {
        connectionIterator.next();
        MemoryConnectionItem* connectionItem = connectionIterator.value();
        if (connectionItem)
        {
            quint64 connectionEndPoint = connectionItem->getSceneEndPoint();
            if (connectionEndPoint > sceneEndPoint)
            {
                sceneEndPoint = connectionEndPoint;
            }
        }
    }

    quint64 heightWithOverlappingSubItem = sceneBoundingRect().bottom() + getSubItemHeightAddition();
    if (heightWithOverlappingSubItem > sceneEndPoint)
    {
        sceneEndPoint = heightWithOverlappingSubItem;
    }

    return sceneEndPoint;
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::changeChildItemRanges()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::changeChildItemRanges(quint64 offset)
{
    SubMemoryLayout::changeChildItemRanges(offset);
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
// Function: MainMemoryGraphicsItem::hideFirstAndLastSegmentRange()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::hideFirstAndLastSegmentRange()
{
    if (getSubMemoryItems().size() > 0)
    {
        MemoryDesignerChildGraphicsItem* firstSubItem = getSubMemoryItems().first();
        MemoryDesignerChildGraphicsItem* lastSubItem = getSubMemoryItems().last();

        firstSubItem->hideStartRangeLabel();
        lastSubItem->hideEndRangeLabel();
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
    if (getMemoryConnections().size() > 1)
    {
        QMultiMapIterator<quint64, MemoryConnectionItem*> connectionIterator(getMemoryConnections());
        while (connectionIterator.hasNext())
        {
            connectionIterator.next();

            QMultiMapIterator<quint64, MemoryConnectionItem*> comparisonIterator(connectionIterator);
            while (comparisonIterator.hasNext())
            {
                comparisonIterator.next();

                MemoryConnectionItem* selectedItem = connectionIterator.value();
                MemoryConnectionItem* comparisonItem = comparisonIterator.value();

                QRectF connectionRect = selectedItem->sceneBoundingRect();
                QRectF comparisonRect = comparisonItem->sceneBoundingRect();

                if (selectedItem && comparisonItem && selectedItem != comparisonItem &&
                    selectedItem->getConnectionStartItem() == this &&
                    comparisonItem->getConnectionStartItem() == this &&
                    MemoryDesignerConstants::itemOverlapsAnotherItem(connectionRect,
                    selectedItem->pen().width(), comparisonRect, comparisonItem->pen().width()))
                {
                    MemoryCollisionItem* newCollisionItem =
                        new MemoryCollisionItem(selectedItem, comparisonItem, scene());
                    memoryCollisions_.append(newCollisionItem);
                }
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
// Function: MainMemoryGraphicsItem::compressToUnCutAddresses()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::compressToUnCutAddresses(QVector<quint64> unCutAddresses, const int CUTMODIFIER,
    bool memoryItemsAreCompressed)
{
    if (!subItemsAreFiltered())
    {
        compressSubItemsToUnCutAddresses(unCutAddresses, CUTMODIFIER, memoryItemsAreCompressed);
    }

    MemoryDesignerGraphicsItem::compressToUnCutAddresses(unCutAddresses, CUTMODIFIER, memoryItemsAreCompressed);
    setCompressed(true);
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
        MemoryConnectionItem* currentConnection = connectionIterator.value();

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

        MemoryConnectionItem* connectionItem = connectionIterator.value();
        
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
