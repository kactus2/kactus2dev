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

#include <QFont>
#include <QGraphicsSceneMouseEvent>

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::MainMemoryGraphicsItem()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem::MainMemoryGraphicsItem(QSharedPointer<MemoryItem> memoryItem, QString const& instanceName,
    QString const& subItemType, QGraphicsItem* parent):
MemoryDesignerGraphicsItem(memoryItem->getName(), parent),
SubMemoryLayout(memoryItem, subItemType, this),
instanceNameLabel_(new QGraphicsTextItem(instanceName, this)),
aubLabel_(new QGraphicsTextItem("", this)),
instanceName_(instanceName),
memoryCollisions_(),
compressed_(false)
{
    QString addressUnitBits = memoryItem->getAUB();
    QString aubText = "AUB: ";

    if (addressUnitBits.isEmpty())
    {
        aubText.append("8");
    }
    else
    {
        aubText.append(addressUnitBits);
    }

    aubLabel_->setPlainText(aubText);

    // setFlag(ItemIsSelectable);
    // setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemSendsScenePositionChanges);

    QFont labelFont = getNameLabel()->font();
    instanceNameLabel_->setFont(labelFont);
    aubLabel_->setFont(labelFont);
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
// Function: MainMemoryGraphicsItem::getAUBLabel()
//-----------------------------------------------------------------------------
QGraphicsTextItem* MainMemoryGraphicsItem::getAUBLabel() const
{
    return aubLabel_;
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
// Function: MainMemoryGraphicsItem::moveConnectedConnectionsInY()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::moveConnectedConnectionsInY(qreal yTransfer)
{
    foreach (MemoryConnectionItem* connectionItem, getMemoryConnections())
    {
        connectionItem->onMoveConnectionInY(this, yTransfer);
    }
    foreach (MemoryCollisionItem* collisionItem, memoryCollisions_)
    {
        collisionItem->moveBy(0, yTransfer);
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
// Function: MainMemoryGraphicsItem::moveByConnectionInY()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::moveByConnectionInY(MemoryConnectionItem* movementOrigin, qreal yTransfer)
{
    moveBy(0, yTransfer);

    foreach (MemoryConnectionItem* connectionItem, getMemoryConnections())
    {
        if (connectionItem != movementOrigin)
        {
            connectionItem->onMoveConnectionInY(this, yTransfer);
        }
    }

    foreach (MemoryCollisionItem* collisionItem, memoryCollisions_)
    {
        collisionItem->moveBy(0, yTransfer);
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
// Function: MainMemoryGraphicsItem::getContainingInstanceName()
//-----------------------------------------------------------------------------
QString MainMemoryGraphicsItem::getContainingInstanceName() const
{
    return instanceName_;
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
// Function: MainMemoryGraphicsItem::addConnectionCollision()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::addConnectionCollision(MemoryCollisionItem* collisionItem)
{
    memoryCollisions_.append(collisionItem);
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::reDrawConnections()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::reDrawConnections()
{
    foreach (MemoryConnectionItem* connection, getMemoryConnections())
    {
        connection->reDrawConnection();
    }
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::getSceneEndPoint()
//-----------------------------------------------------------------------------
quint64 MainMemoryGraphicsItem::getSceneEndPoint() const
{
    quint64 sceneEndPoint = sceneBoundingRect().bottom();

    foreach (MemoryConnectionItem* connectionItem, getConnectionsInVector())
    {
        quint64 connectionEndPoint = connectionItem->getSceneEndPoint();
        if (connectionEndPoint > sceneEndPoint)
        {
            sceneEndPoint = connectionEndPoint;
        }
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
        quint64 connectionRangeEnd = connection->getRangeEndValue().toULongLong(0, 16);
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
// Function: MainMemoryGraphicsItem::resizeSubItemNameLabels()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::resizeSubItemNameLabels()
{
    QMapIterator<quint64, MemoryDesignerChildGraphicsItem*> subItemIterator(getSubMemoryItems());
    while (subItemIterator.hasNext())
    {
        subItemIterator.next();

        MemoryDesignerChildGraphicsItem* subItem = subItemIterator.value();
        subItem->fitNameLabel();
    }
}
