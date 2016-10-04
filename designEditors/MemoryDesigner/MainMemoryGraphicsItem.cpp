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

#include <designEditors/MemoryDesigner/MemoryColumn.h>
#include <designEditors/MemoryDesigner/MemoryConnectionItem.h>
#include <designEditors/MemoryDesigner/MemoryCollisionItem.h>

#include <QFont>
#include <QGraphicsSceneMouseEvent>

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::MainMemoryGraphicsItem()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem::MainMemoryGraphicsItem(QString const& itemName, QString const& instanceName,
    QString const& addressUnitBits, QGraphicsItem* parent):
MemoryDesignerGraphicsItem(itemName, parent),
instanceNameLabel_(new QGraphicsTextItem(instanceName, this)),
aubLabel_(new QGraphicsTextItem("", this)),
instanceName_(instanceName),
memoryConnections_(),
memoryCollisions_()
{
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
    
    foreach (MemoryConnectionItem* connectionItem, memoryConnections_)
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
    foreach (MemoryConnectionItem* connectionItem, memoryConnections_)
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

    foreach (MemoryConnectionItem* connectionItem, memoryConnections_)
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

    foreach (MemoryConnectionItem* connectionItem, memoryConnections_)
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

    foreach (MemoryConnectionItem* connectionItem, memoryConnections_)
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
    memoryConnections_.append(connectionItem);
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::addConnectionCollision()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::addConnectionCollision(MemoryCollisionItem* collisionItem)
{
    memoryCollisions_.append(collisionItem);
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::getMemoryConnections()
//-----------------------------------------------------------------------------
QVector<MemoryConnectionItem*> MainMemoryGraphicsItem::getMemoryConnections() const
{
    return memoryConnections_;
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::reDrawConnections()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::reDrawConnections()
{
    foreach (MemoryConnectionItem* connection, memoryConnections_)
    {
        connection->reDrawConnection();
    }
}
