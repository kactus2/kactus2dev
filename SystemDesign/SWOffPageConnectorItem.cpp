//-----------------------------------------------------------------------------
// File: SWOffPageConnectorItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.1.2012
//
// Description:
// Off-page connector for the block diagram.
//-----------------------------------------------------------------------------

#include "SWOffPageConnectorItem.h"

#include <models/ComInterface.h>
#include <models/ApiInterface.h>

#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsConnection.h>
#include <common/diagramgrid.h>

#include <QBrush>
#include <QPen>

//-----------------------------------------------------------------------------
// Function: SWOffPageConnectorItem()
//-----------------------------------------------------------------------------
SWOffPageConnectorItem::SWOffPageConnectorItem(SWConnectionEndpoint* parent)
    : SWConnectionEndpoint(parent),
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

    //nameLabel_->setFlag(ItemStacksBehindParent);

    // Add a line as a child graphics item.
    QGraphicsLineItem* line = new QGraphicsLineItem(0.0, 0.0, 0.0, GridSize * 3, this);
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
// Function: SWOffPageConnectorItem()
//-----------------------------------------------------------------------------
SWOffPageConnectorItem::~SWOffPageConnectorItem()
{
}

//-----------------------------------------------------------------------------
// Function: name()
//-----------------------------------------------------------------------------
QString SWOffPageConnectorItem::name() const
{
    return parent_->name();
}

//-----------------------------------------------------------------------------
// Function: setName()
//-----------------------------------------------------------------------------
void SWOffPageConnectorItem::setName(QString const& name)
{
	parent_->setName(name);
}

bool SWOffPageConnectorItem::isHierarchical() const
{
    return parent_->isHierarchical();
}

//-----------------------------------------------------------------------------
// Function: onConnect()
//-----------------------------------------------------------------------------
bool SWOffPageConnectorItem::onConnect(ConnectionEndpoint const* other)
{
    return parent_->onConnect(other);
}

//-----------------------------------------------------------------------------
// Function: onDisonnect()
//-----------------------------------------------------------------------------
void SWOffPageConnectorItem::onDisconnect(ConnectionEndpoint const* other)
{
    parent_->onDisconnect(other);
}

//-----------------------------------------------------------------------------
// Function: isConnectionValid()
//-----------------------------------------------------------------------------
bool SWOffPageConnectorItem::isConnectionValid(ConnectionEndpoint const* other) const
{
    return parent_->isConnectionValid(other);
}

//-----------------------------------------------------------------------------
// Function: encompassingComp()
//-----------------------------------------------------------------------------
ComponentItem* SWOffPageConnectorItem::encompassingComp() const
{
    return parent_->encompassingComp();
}

//-----------------------------------------------------------------------------
// Function: getOwnerComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> SWOffPageConnectorItem::getOwnerComponent() const
{
    return parent_->getOwnerComponent();
}

//-----------------------------------------------------------------------------
// Function: itemChange()
//-----------------------------------------------------------------------------
QVariant SWOffPageConnectorItem::itemChange(GraphicsItemChange change,
                                             const QVariant &value)
{
    switch (change)
    {
    case ItemScenePositionHasChanged:
        {
            // Check if the updates are not disabled.
            if (encompassingComp() == 0 || !encompassingComp()->isConnectionUpdateDisabled())
            {
                // Update the connections.
                foreach (GraphicsConnection* interconnection, getConnections())
                {
                    interconnection->updatePosition();
                }
            }

            break;
        }

    default:
        {
            break;
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: isDirectionFixed()
//-----------------------------------------------------------------------------
bool SWOffPageConnectorItem::isDirectionFixed() const
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
// Function: setTypes()
//-----------------------------------------------------------------------------
void SWOffPageConnectorItem::setTypeDefinition(VLNV const& type)
{
    parent_->setTypeDefinition(type);
    setType(parent_->getType());
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void SWOffPageConnectorItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    SWConnectionEndpoint::mousePressEvent(event);
}

//-----------------------------------------------------------------------------
// Function: description()
//-----------------------------------------------------------------------------
QString SWOffPageConnectorItem::description() const
{
	return parent_->description();
}

void SWOffPageConnectorItem::setDescription(QString const& description)
{
	parent_->setDescription(description);
}

//-----------------------------------------------------------------------------
// Function: addConnection()
//-----------------------------------------------------------------------------
void SWOffPageConnectorItem::addConnection(GraphicsConnection* connection)
{
    SWConnectionEndpoint::addConnection(connection);
    connection->setRoutingMode(GraphicsConnection::ROUTING_MODE_OFFPAGE);

    setVisible(true);
}

//-----------------------------------------------------------------------------
// Function: removeConnection()
//-----------------------------------------------------------------------------
void SWOffPageConnectorItem::removeConnection(GraphicsConnection* connection)
{
    SWConnectionEndpoint::removeConnection(connection);

    if (getConnections().size() == 0)
    {
        setVisible(false);
    }
}

//-----------------------------------------------------------------------------
// Function: setDirection()
//-----------------------------------------------------------------------------
void SWOffPageConnectorItem::setDirection(QVector2D const& dir)
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
// Function: getDirection()
//-----------------------------------------------------------------------------
QVector2D const& SWOffPageConnectorItem::getDirection() const
{
    return parent_->getDirection();
}

//-----------------------------------------------------------------------------
// Function: SWOffPageConnectorItem::isExclusive()
//-----------------------------------------------------------------------------
bool SWOffPageConnectorItem::isExclusive() const
{
    return parent_->isExclusive();
}

//-----------------------------------------------------------------------------
// Function: SWOffPageConnectorItem::getTypeDefinition()
//-----------------------------------------------------------------------------
VLNV SWOffPageConnectorItem::getTypeDefinition() const
{
    return parent_->getTypeDefinition();
}

//-----------------------------------------------------------------------------
// Function: SWOffPageConnectorItem::onBeginConnect()
//-----------------------------------------------------------------------------
void SWOffPageConnectorItem::onBeginConnect()
{
    return parent_->onBeginConnect();
}

//-----------------------------------------------------------------------------
// Function: SWOffPageConnectorItem::onEndConnect()
//-----------------------------------------------------------------------------
void SWOffPageConnectorItem::onEndConnect()
{
    return parent_->onEndConnect();
}

//-----------------------------------------------------------------------------
// Function: SWOffPageConnectorItem::getComInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ComInterface> SWOffPageConnectorItem::getComInterface() const
{
    return parent_->getComInterface();
}

//-----------------------------------------------------------------------------
// Function: SWOffPageConnectorItem::getApiInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ApiInterface> SWOffPageConnectorItem::getApiInterface() const
{
    return parent_->getApiInterface();
}
