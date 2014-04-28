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

#include <IPXACTmodels/businterface.h>

#include <QBrush>
#include <QPen>

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem()
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
// Function: OffPageConnectorItem()
//-----------------------------------------------------------------------------
OffPageConnectorItem::~OffPageConnectorItem()
{
}

//-----------------------------------------------------------------------------
// Function: name()
//-----------------------------------------------------------------------------
QString OffPageConnectorItem::name() const
{
    return parent_->name();
}

//-----------------------------------------------------------------------------
// Function: setName()
//-----------------------------------------------------------------------------
void OffPageConnectorItem::setName(QString const& name)
{
	parent_->setName(name);
}

//-----------------------------------------------------------------------------
// Function: getBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> OffPageConnectorItem::getBusInterface() const
{
    return parent_->getBusInterface();
}

//-----------------------------------------------------------------------------
// Function: updateInterface()
//-----------------------------------------------------------------------------
void OffPageConnectorItem::updateInterface()
{
    // Retrieve the correct brush from the parent diagram port.
    // Set the port black if it is temporary.
    if (getBusInterface() == 0 || !getBusInterface()->getBusType().isValid())
    {
        setBrush(QBrush(Qt::black));
    }
    else
    {
        // Otherwise set the color based on the interface mode.
        switch (getBusInterface()->getInterfaceMode()) {
        case General::MASTER:
            setBrush(QBrush(QColor(0x32,0xcb,0xcb)));
            break;
        case General::SLAVE:
            setBrush(QBrush(QColor(0x32,0x99,0x64)));
            break;
        case General::MIRROREDMASTER:
            setBrush(QBrush(QColor(0xcb,0xfd,0xfd)));
            break;
        case General::MIRROREDSLAVE:
            setBrush(QBrush(QColor(0x00,0xfd,00)));
            break;
        case General::SYSTEM:
            setBrush(QBrush(QColor(0xf9,0x11,0x11)));
            break;
        case General::MIRROREDSYSTEM:
            setBrush(QBrush(QColor(0xf9,0x9d,0xcb)));
            break;
        case General::MONITOR:
            setBrush(QBrush(QColor(0xfd,0xfd,0xfd)));
            break;
        default:
            setBrush(QBrush(Qt::red));
            break;
        }
    }
}

bool OffPageConnectorItem::isHierarchical() const
{
    return parent_->isHierarchical();
}

//-----------------------------------------------------------------------------
// Function: onConnect()
//-----------------------------------------------------------------------------
bool OffPageConnectorItem::onConnect(ConnectionEndpoint const* other)
{
    return parent_->onConnect(other);
}

//-----------------------------------------------------------------------------
// Function: onDisonnect()
//-----------------------------------------------------------------------------
void OffPageConnectorItem::onDisconnect(ConnectionEndpoint const* other)
{
    parent_->onDisconnect(other);
}

//-----------------------------------------------------------------------------
// Function: isConnectionValid()
//-----------------------------------------------------------------------------
bool OffPageConnectorItem::isConnectionValid(ConnectionEndpoint const* other) const
{
    return parent_->isConnectionValid(other);
}

//-----------------------------------------------------------------------------
// Function: encompassingComp()
//-----------------------------------------------------------------------------
ComponentItem* OffPageConnectorItem::encompassingComp() const
{
    return parent_->encompassingComp();
}

//-----------------------------------------------------------------------------
// Function: getOwnerComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> OffPageConnectorItem::getOwnerComponent() const
{
    return parent_->getOwnerComponent();
}

//-----------------------------------------------------------------------------
// Function: itemChange()
//-----------------------------------------------------------------------------
QVariant OffPageConnectorItem::itemChange(GraphicsItemChange change,
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
// Function: setTypes()
//-----------------------------------------------------------------------------
void OffPageConnectorItem::setTypes(VLNV const& busType, VLNV const& absType, General::InterfaceMode mode)
{
    parent_->setTypes(busType, absType, mode);
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void OffPageConnectorItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    HWConnectionEndpoint::mousePressEvent(event);
}

//-----------------------------------------------------------------------------
// Function: setInterfaceMode()
//-----------------------------------------------------------------------------
void OffPageConnectorItem::setInterfaceMode(General::InterfaceMode mode)
{
    parent_->setInterfaceMode(mode);
}

//-----------------------------------------------------------------------------
// Function: description()
//-----------------------------------------------------------------------------
QString OffPageConnectorItem::description() const
{
	return parent_->description();
}

void OffPageConnectorItem::setDescription(QString const& description)
{
	parent_->setDescription(description);
}

//-----------------------------------------------------------------------------
// Function: addConnection()
//-----------------------------------------------------------------------------
void OffPageConnectorItem::addConnection(GraphicsConnection* connection)
{
    HWConnectionEndpoint::addConnection(connection);
    connection->setRoutingMode(GraphicsConnection::ROUTING_MODE_OFFPAGE);

    setVisible(true);
}

//-----------------------------------------------------------------------------
// Function: removeConnection()
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
// Function: setDirection()
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
// Function: getDirection()
//-----------------------------------------------------------------------------
QVector2D const& OffPageConnectorItem::getDirection() const
{
    return parent_->getDirection();
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::getPort()
//-----------------------------------------------------------------------------
Port* OffPageConnectorItem::getPort() const
{
    return parent_->getPort();
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::isExclusive()
//-----------------------------------------------------------------------------
bool OffPageConnectorItem::isExclusive() const
{
    return parent_->isExclusive();
}
