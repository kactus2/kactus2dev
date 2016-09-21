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
        collisionItem->moveBy(mouseMoveDelta.x(), mouseMoveDelta.y());
    }
}

//-----------------------------------------------------------------------------
// Function: MainMemoryGraphicsItem::moveByConnection()
//-----------------------------------------------------------------------------
void MainMemoryGraphicsItem::moveByConnection(MemoryConnectionItem* movementOrigin, QPointF movementDelta)
{
    moveBy(movementDelta.x(), movementDelta.y());

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
        collisionItem->moveBy(0, movementDelta.y());
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
QList<MemoryConnectionItem*> MainMemoryGraphicsItem::getMemoryConnections() const
{
    return memoryConnections_;
}
