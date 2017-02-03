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

#include <designEditors/common/diagramgrid.h>

#include <designEditors/MemoryDesigner/MemoryItem.h>
#include <designEditors/MemoryDesigner/MemoryColumn.h>
#include <designEditors/MemoryDesigner/MemoryConnectionItem.h>
#include <designEditors/MemoryDesigner/MemoryCollisionItem.h>
#include <designEditors/MemoryDesigner/MemoryDesignerChildGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryExtensionGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryDesignerConstants.h>

#include <QFont>
#include <QGraphicsSceneMouseEvent>

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::MainMemoryGraphicsItem()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem::MainMemoryGraphicsItem(QSharedPointer<MemoryItem> memoryItem, QString const& instanceName,
    QString const& subItemType, bool filterSubItems, QGraphicsItem* parent):
MemoryDesignerGraphicsItem(memoryItem->getName(), instanceName, parent),
SubMemoryLayout(memoryItem, subItemType, filterSubItems, this),
instanceNameLabel_(new QGraphicsTextItem(instanceName, this)),
memoryItem_(memoryItem),
memoryCollisions_(),
compressed_(false),
extensionItem_(0)
{
    // setFlag(ItemIsSelectable);
    // setFlag(ItemIsMovable);
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
// Function: MainMemoryGraphicsItem::mouseMoveEvent()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF positionBeforeMovement = scenePos();

    QGraphicsRectItem::mouseMoveEvent(event);

    MemoryColumn* memoryColumn = dynamic_cast<MemoryColumn*>(parentItem());
    if (memoryColumn)
    {
        memoryColumn->onMoveItem(this);
    }

    moveConnectedConnections(positionBeforeMovement);
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF positionBeforeRelease = scenePos();

    QGraphicsRectItem::mouseReleaseEvent(event);

    MemoryColumn* memoryColumn = dynamic_cast<MemoryColumn*>(parentItem());
    if (memoryColumn)
    {
        memoryColumn->onReleaseItem(this);
    }

    moveConnectedConnections(positionBeforeRelease);
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::moveConnectedConnections()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::moveConnectedConnections(QPointF beforePosition)
{
    QPointF afterPosition = scenePos();
    QPointF mouseMoveDelta = afterPosition - beforePosition;
    
    foreach (MemoryConnectionItem* connectionItem, getMemoryConnections())
    {
        connectionItem->onMoveConnection(this, mouseMoveDelta);
    }

    foreach (MemoryCollisionItem* collisionItem, memoryCollisions_)
    {
        qreal collisionMoveY = collisionItem->pos().y() + mouseMoveDelta.y();
        collisionItem->setPos(0, collisionMoveY);
    }
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::moveByConnection()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::moveByConnection(MemoryConnectionItem* movementOrigin, QPointF movementDelta)
{
    qreal newPositionX = pos().x() + movementDelta.x();
    qreal newPositionY = pos().y() + movementDelta.y();
    setPos(newPositionX, newPositionY);

    foreach (MemoryConnectionItem* connectionItem, getMemoryConnections())
    {
        if (connectionItem != movementOrigin)
        {
            QPointF newMovement (0, movementDelta.y());
            connectionItem->onMoveConnection(this, newMovement);
        }
    }

    foreach (MemoryCollisionItem* collisionItem, memoryCollisions_)
    {
        qreal newCollisionY = collisionItem->pos().y() + movementDelta.y();
        collisionItem->setPos(0, newCollisionY);
    }
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::moveConnectedItems()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::moveConnectedItems(qreal yTransfer)
{
    moveBy(0, yTransfer);

    foreach (MemoryConnectionItem* connectionItem, getMemoryConnections())
    {
        connectionItem->moveConnectedItem(this, yTransfer);
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

    QMapIterator<quint64, MemoryConnectionItem*> connectionIterator(getMemoryConnections());
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

    QMapIterator<quint64, MemoryConnectionItem*> connectionIterator(getMemoryConnections());

    quint64 lastConnectionRangeEnd = 0;

    while (connectionIterator.hasNext())
    {
        connectionIterator.next();

        MemoryConnectionItem* connection = connectionIterator.value();
        quint64 connectionRangeEnd = connection->getRangeEndValue();
        if (connectionRangeEnd > lastConnectionRangeEnd)
        {
            lastConnection = connection;
            lastConnectionRangeEnd = connectionRangeEnd;
        }
    }

    return lastConnection;
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
QSharedPointer<MemoryItem> MainMemoryGraphicsItem::getMemoryItem() const
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
        QMapIterator<quint64, MemoryConnectionItem*> connectionIterator(getMemoryConnections());
        while (connectionIterator.hasNext())
        {
            connectionIterator.next();

            QMapIterator<quint64, MemoryConnectionItem*> comparisonIterator(connectionIterator);
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
void MainMemoryGraphicsItem::compressToUnCutAddresses(QVector<quint64> unCutAddresses, const int CUTMODIFIER)
{
    if (!subItemsAreFiltered())
    {
        compressSubItemsToUnCutAddresses(unCutAddresses, CUTMODIFIER);
    }

    MemoryDesignerGraphicsItem::compressToUnCutAddresses(unCutAddresses, CUTMODIFIER);
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
