//-----------------------------------------------------------------------------
// File: DiagramOffPageConnector.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.1.2012
//
// Description:
// Off-page connector for the block diagram.
//-----------------------------------------------------------------------------

#include "DiagramOffPageConnector.h"

#include "diagramcomponent.h"
#include "diagraminterconnection.h"

#include <common/diagramgrid.h>

#include <models/businterface.h>

#include <QBrush>
#include <QPen>

//-----------------------------------------------------------------------------
// Function: DiagramOffPageConnector()
//-----------------------------------------------------------------------------
DiagramOffPageConnector::DiagramOffPageConnector(DiagramConnectionEndpoint* parent)
    : DiagramConnectionEndpoint(parent),
      parent_(parent)        
{
    Q_ASSERT(parent != 0);

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
// Function: DiagramOffPageConnector()
//-----------------------------------------------------------------------------
DiagramOffPageConnector::~DiagramOffPageConnector()
{
}

//-----------------------------------------------------------------------------
// Function: name()
//-----------------------------------------------------------------------------
QString DiagramOffPageConnector::name() const
{
    return parent_->name();
}

//-----------------------------------------------------------------------------
// Function: setName()
//-----------------------------------------------------------------------------
void DiagramOffPageConnector::setName(QString const& name)
{
	parent_->setName(name);
}

//-----------------------------------------------------------------------------
// Function: getBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> DiagramOffPageConnector::getBusInterface() const
{
    return parent_->getBusInterface();
}

//-----------------------------------------------------------------------------
// Function: updateInterface()
//-----------------------------------------------------------------------------
void DiagramOffPageConnector::updateInterface()
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
        }
    }

    emit contentChanged();
}

bool DiagramOffPageConnector::isHierarchical() const
{
    return parent_->isHierarchical();
}

//-----------------------------------------------------------------------------
// Function: onConnect()
//-----------------------------------------------------------------------------
bool DiagramOffPageConnector::onConnect(DiagramConnectionEndpoint const* other)
{
    return parent_->onConnect(other);
}

//-----------------------------------------------------------------------------
// Function: onDisonnect()
//-----------------------------------------------------------------------------
void DiagramOffPageConnector::onDisconnect(DiagramConnectionEndpoint const* other)
{
    parent_->onDisconnect(other);
}

//-----------------------------------------------------------------------------
// Function: canConnect()
//-----------------------------------------------------------------------------
bool DiagramOffPageConnector::canConnect(DiagramConnectionEndpoint const* other) const
{
    return parent_->canConnect(other);
}

//-----------------------------------------------------------------------------
// Function: encompassingComp()
//-----------------------------------------------------------------------------
ComponentItem* DiagramOffPageConnector::encompassingComp() const
{
    return parent_->encompassingComp();
}

//-----------------------------------------------------------------------------
// Function: getOwnerComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> DiagramOffPageConnector::getOwnerComponent() const
{
    return parent_->getOwnerComponent();
}

//-----------------------------------------------------------------------------
// Function: itemChange()
//-----------------------------------------------------------------------------
QVariant DiagramOffPageConnector::itemChange(GraphicsItemChange change,
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
                foreach (DiagramInterconnection* interconnection, getInterconnections())
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
bool DiagramOffPageConnector::isDirectionFixed() const
{
    if (getInterconnections().size() > 0)
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
void DiagramOffPageConnector::setTypes(VLNV const& busType, VLNV const& absType, General::InterfaceMode mode)
{
    parent_->setTypes(busType, absType, mode);
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void DiagramOffPageConnector::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    DiagramConnectionEndpoint::mousePressEvent(event);
}

//-----------------------------------------------------------------------------
// Function: setInterfaceMode()
//-----------------------------------------------------------------------------
void DiagramOffPageConnector::setInterfaceMode(General::InterfaceMode mode)
{
    parent_->setInterfaceMode(mode);
}

//-----------------------------------------------------------------------------
// Function: description()
//-----------------------------------------------------------------------------
QString DiagramOffPageConnector::description() const
{
	return parent_->description();
}

void DiagramOffPageConnector::setDescription(QString const& description)
{
	parent_->setDescription(description);
}

//-----------------------------------------------------------------------------
// Function: addInterconnection()
//-----------------------------------------------------------------------------
void DiagramOffPageConnector::addInterconnection(DiagramInterconnection* connection)
{
    DiagramConnectionEndpoint::addInterconnection(connection);
    connection->setRoutingMode(DiagramInterconnection::ROUTING_MODE_OFFPAGE);

    setVisible(true);
}

//-----------------------------------------------------------------------------
// Function: removeInterconnection()
//-----------------------------------------------------------------------------
void DiagramOffPageConnector::removeInterconnection(DiagramInterconnection* connection)
{
    DiagramConnectionEndpoint::removeInterconnection(connection);

    if (getInterconnections().size() == 0)
    {
        setVisible(false);
    }
}

//-----------------------------------------------------------------------------
// Function: setDirection()
//-----------------------------------------------------------------------------
void DiagramOffPageConnector::setDirection(QVector2D const& dir)
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
QVector2D const& DiagramOffPageConnector::getDirection() const
{
    return parent_->getDirection();
}

//-----------------------------------------------------------------------------
// Function: DiagramOffPageConnector::isBus()
//-----------------------------------------------------------------------------
bool DiagramOffPageConnector::isBus() const
{
    return parent_->isBus();
}

//-----------------------------------------------------------------------------
// Function: DiagramOffPageConnector::getPort()
//-----------------------------------------------------------------------------
Port* DiagramOffPageConnector::getPort() const
{
    return parent_->getPort();
}
