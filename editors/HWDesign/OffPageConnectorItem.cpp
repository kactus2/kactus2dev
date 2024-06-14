//-----------------------------------------------------------------------------
// File: OffPageConnectorItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 9.1.2012
//
// Description:
// Off-page connector for the block diagram.
//-----------------------------------------------------------------------------

#include "OffPageConnectorItem.h"

#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsConnection.h>

#include <editors/common/diagramgrid.h>

#include <IPXACTmodels/Component/BusInterface.h>

#include <QBrush>
#include <QPen>

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::OffPageConnectorItem()
//-----------------------------------------------------------------------------
OffPageConnectorItem::OffPageConnectorItem(ConnectionEndpoint* parent):
ConnectionEndpoint(parent),
parent_(parent)
{
    Q_ASSERT(parent != 0);

    int squareSize = GridSize;
    QPolygonF shape;
    shape << QPointF(-squareSize / 2,  squareSize / 2)
          << QPointF(-squareSize / 2, -squareSize / 2)
          << QPointF( squareSize / 2, -squareSize / 2)
          << QPointF( squareSize / 2,  squareSize / 2);
    setPolygon(shape);

    // Add a line as a child graphics item.
    auto line = new QGraphicsLineItem(0.0, 0.0, 0.0, 3*GridSize, this);
    line->setFlag(ItemStacksBehindParent);
    
    QPen newPen = line->pen();
    newPen.setWidth(3);
    line->setPen(newPen);

    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemSendsScenePositionChanges);

    OffPageConnectorItem::updateInterface();
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
    ConnectionEndpoint::addConnection(connection);
    connection->setRoutingMode(GraphicsConnection::ROUTING_MODE_OFFPAGE);

    setVisible(true);
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::removeConnection()
//-----------------------------------------------------------------------------
void OffPageConnectorItem::removeConnection(GraphicsConnection* connection)
{
    ConnectionEndpoint::removeConnection(connection);
    connection->setRoutingMode(GraphicsConnection::ROUTING_MODE_NORMAL);

    if (getConnections().isEmpty())
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
void OffPageConnectorItem::onDisconnect()
{
    parent_->onDisconnect();
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
bool OffPageConnectorItem::isDirectionFixed() const noexcept
{
    if (getConnections().isEmpty() == false)
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
// Function: OffPageConnectorItem::getComInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ComInterface> OffPageConnectorItem::getComInterface() const
{
    return parent_->getComInterface();
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::getApiInterface()
//-----------------------------------------------------------------------------
QSharedPointer<ApiInterface> OffPageConnectorItem::getApiInterface() const
{
    return parent_->getApiInterface();
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::isHierarchical()
//-----------------------------------------------------------------------------
bool OffPageConnectorItem::isHierarchical() const noexcept
{
    return parent_->isHierarchical();
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::getInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode OffPageConnectorItem::getInterfaceMode() const
{
    return parent_->getInterfaceMode();
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::itemChange()
//-----------------------------------------------------------------------------
QVariant OffPageConnectorItem::itemChange(GraphicsItemChange change, QVariant const& value)
{
    if (change == ItemScenePositionHasChanged)
    {
        updateConnectionPositions();
    }

    return QGraphicsItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::getType()
//-----------------------------------------------------------------------------
ConnectionEndpoint::EndpointType OffPageConnectorItem::getType() const noexcept
{
    return parent_->getType();
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::isBus()
//-----------------------------------------------------------------------------
bool OffPageConnectorItem::isBus() const noexcept
{
    return parent_->isBus();
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::isAdHoc()
//-----------------------------------------------------------------------------
bool OffPageConnectorItem::isAdHoc() const noexcept
{
    return parent_->isAdHoc();
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::isCom()
//-----------------------------------------------------------------------------
bool OffPageConnectorItem::isCom() const noexcept
{
    return parent_->isCom();
}

//-----------------------------------------------------------------------------
// Function: OffPageConnectorItem::isApi()
//-----------------------------------------------------------------------------
bool OffPageConnectorItem::isApi() const noexcept
{
    return parent_->isApi();
}

ConnectionEndpoint* OffPageConnectorItem::getParentConnector() const
{
    return parent_;
}
