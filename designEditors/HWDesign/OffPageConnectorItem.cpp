//-----------------------------------------------------------------------------
// File: OffPageConnectorItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.1.2012
//
// Description:
// Off-page connector for the block diagram.
//-----------------------------------------------------------------------------

#include "OffPageConnectorItem.h"

#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsConnection.h>
#include <designEditors/common/diagramgrid.h>

#include <IPXACTmodels/Component/BusInterface.h>

#include <QBrush>
#include <QPen>

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::OffPageConnectorItem()
//-----------------------------------------------------------------------------
OffPageConnectorItem::OffPageConnectorItem(HWConnectionEndpoint* parent)
    : HWConnectionEndpoint(parent),
      parent_(parent)        
{
    Q_ASSERT(parent != 0);

    setType(parent->getType());

    int squareSize = GridSize;
    /*  /\
     *  ||
     */
    QPolygonF shape;
    shape << QPointF(-squareSize / 2,  squareSize / 2)
          << QPointF(-squareSize / 2, -squareSize / 2)
          << QPointF( squareSize / 2, -squareSize / 2)
          << QPointF( squareSize / 2,  squareSize / 2);
    setPolygon(shape);

    // Add a line as a child graphics item.
    QGraphicsLineItem* line = new QGraphicsLineItem(0.0, 0.0, 0.0, 3*GridSize, this);
    line->setFlag(ItemStacksBehindParent);
    
    QPen newPen = line->pen();
    newPen.setWidth(3);
    line->setPen(newPen);

    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemSendsScenePositionChanges);

    updateInterface();
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::OffPageConnectorItem()
//-----------------------------------------------------------------------------
OffPageConnectorItem::~OffPageConnectorItem()
{
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::setTypes()
//-----------------------------------------------------------------------------
void OffPageConnectorItem::setTypes(VLNV const& busType, VLNV const& absType, General::InterfaceMode mode)
{
    parent_->setTypes(busType, absType, mode);
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::updateInterface()
//-----------------------------------------------------------------------------
void OffPageConnectorItem::updateInterface()
{
    // Retrieve the correct brush from the parent diagram port.
    setBrush(parent_->brush());
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::name()
//-----------------------------------------------------------------------------
QString OffPageConnectorItem::name() const
{
    return parent_->name();
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::setName()
//-----------------------------------------------------------------------------
void OffPageConnectorItem::setName(QString const& name)
{
	parent_->setName(name);
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::description()
//-----------------------------------------------------------------------------
QString OffPageConnectorItem::description() const
{
    return parent_->description();
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::setDescription()
//-----------------------------------------------------------------------------
void OffPageConnectorItem::setDescription(QString const& description)
{
    parent_->setDescription(description);
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::addConnection()
//-----------------------------------------------------------------------------
void OffPageConnectorItem::addConnection(GraphicsConnection* connection)
{
    HWConnectionEndpoint::addConnection(connection);
    connection->setRoutingMode(GraphicsConnection::ROUTING_MODE_OFFPAGE);

    setVisible(true);
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::removeConnection()
//-----------------------------------------------------------------------------
void OffPageConnectorItem::removeConnection(GraphicsConnection* connection)
{
    HWConnectionEndpoint::removeConnection(connection);
    connection->setRoutingMode(GraphicsConnection::ROUTING_MODE_NORMAL);

    if (getConnections().size() == 0)
    {
        setVisible(false);
    }
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::onConnect()
//-----------------------------------------------------------------------------
bool OffPageConnectorItem::onConnect(ConnectionEndpoint const* other)
{
    return parent_->onConnect(other);
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::onDisonnect()
//-----------------------------------------------------------------------------
void OffPageConnectorItem::onDisconnect(ConnectionEndpoint const* other)
{
    parent_->onDisconnect(other);
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::isConnectionValid()
//-----------------------------------------------------------------------------
bool OffPageConnectorItem::isConnectionValid(ConnectionEndpoint const* other) const
{
    return parent_->isConnectionValid(other);
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::isExclusive()
//-----------------------------------------------------------------------------
bool OffPageConnectorItem::isExclusive() const
{
    return parent_->isExclusive();
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::setDirection()
//-----------------------------------------------------------------------------
void OffPageConnectorItem::setDirection(QVector2D const& dir)
{
    // Translate the direction to a valid one.
    if (dir.x() < 0.0)
    {
        parent_->setDirection(QVector2D(-1.0f, 0.0f));
    }
    else
    {
        parent_->setDirection(QVector2D(1.0f, 0.0f));
    }
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::getDirection()
//-----------------------------------------------------------------------------
QVector2D OffPageConnectorItem::getDirection() const
{
    return parent_->getDirection();
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::isDirectionFixed()
//-----------------------------------------------------------------------------
bool OffPageConnectorItem::isDirectionFixed() const
{
    if (getConnections().size() > 0)
    {
        return true;
    }
    else
    {
        return parent_->isDirectionFixed();
    }
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::encompassingComp()
//-----------------------------------------------------------------------------
ComponentItem* OffPageConnectorItem::encompassingComp() const
{
    return parent_->encompassingComp();
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::getOwnerComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> OffPageConnectorItem::getOwnerComponent() const
{
    return parent_->getOwnerComponent();
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::getBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> OffPageConnectorItem::getBusInterface() const
{
    return parent_->getBusInterface();
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::getPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> OffPageConnectorItem::getPort() const
{
    return parent_->getPort();
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::onConnect()
//-----------------------------------------------------------------------------
bool OffPageConnectorItem::isHierarchical() const
{
    return parent_->isHierarchical();
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::setInterfaceMode()
//-----------------------------------------------------------------------------
void OffPageConnectorItem::setInterfaceMode(General::InterfaceMode mode)
{
    parent_->setInterfaceMode(mode);
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::itemChange()
//-----------------------------------------------------------------------------
QVariant OffPageConnectorItem::itemChange(GraphicsItemChange change, QVariant const& value)
{
    if (change == ItemScenePositionHasChanged)
    {
        foreach (GraphicsConnection* interconnection, getConnections())
        {
            interconnection->updatePosition();
        }
    }

    return QGraphicsItem::itemChange(change, value);
}
