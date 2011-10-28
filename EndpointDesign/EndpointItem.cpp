//-----------------------------------------------------------------------------
// File: EndpointItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 26.2.2011
//
// Description:
// Endpoint UI item.
//-----------------------------------------------------------------------------

#include "EndpointItem.h"

#include "MappingComponentItem.h"
#include "ProgramEntityItem.h"
#include "EndpointEditDialog.h"
#include "EndpointStack.h"
#include "EndpointConnection.h"
#include "EndpointDesignDiagram.h"
#include "SystemMoveCommands.h"

#include <models/component.h>
#include <models/businterface.h>

#include "common/GenericEditProvider.h"
#include <common/diagramgrid.h>

#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QDebug>

//-----------------------------------------------------------------------------
// Function: EndpointItem()
//-----------------------------------------------------------------------------
EndpointItem::EndpointItem(ProgramEntityItem* parentNode, QString const& name,
                           MCAPIEndpointDirection type, MCAPIDataType connType,
                           QGraphicsItem* parent) : QAbstractGraphicsShapeItem(parent),
                                                    parentProgEntity_(parentNode), name_(name),
                                                    type_(type), connType_(connType), portID_(-1),
                                                    textLabel_(0), dir_(DIR_LEFT), oldPos_()
{
    Q_ASSERT(parentNode != 0);

    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemSendsScenePositionChanges);
    setFlag(ItemIsSelectable);
    setPen(QPen(Qt::black, 0));
    
    textLabel_ = new QGraphicsTextItem(this);

    textLabel_->setPos(-WIDTH / 2 + ARROW_WIDTH + 5, 2 - HEIGHT / 2);
    updateText();
    updateVisuals();
}

//-----------------------------------------------------------------------------
// Function: ~EndpointItem()
//-----------------------------------------------------------------------------
EndpointItem::~EndpointItem()
{
    // Remove all connections.
    foreach (EndpointConnection* conn, getConnections())
    {
        delete conn;
    }
        
    // Remove the endpoint from the stack where it resides.
    EndpointStack* stack = dynamic_cast<EndpointStack*>(parentItem());

    if (stack != 0)
    {
        stack->removeEndpoint(this);
    }
}

//-----------------------------------------------------------------------------
// Function: setName()
//-----------------------------------------------------------------------------
void EndpointItem::setName(QString const& name)
{
    name_ = name;
    updateText();
}

//-----------------------------------------------------------------------------
// Function: setType()
//-----------------------------------------------------------------------------
void EndpointItem::setType(MCAPIEndpointDirection type)
{
    type_ = type;
    updateText();
}

//-----------------------------------------------------------------------------
// Function: setConnectionType()
//-----------------------------------------------------------------------------
void EndpointItem::setConnectionType(MCAPIDataType connType)
{
    connType_ = connType;
    updateVisuals();
}


//-----------------------------------------------------------------------------
// Function: setPortID()
//-----------------------------------------------------------------------------
void EndpointItem::setPortID(unsigned int portID)
{
    // Release the old id if valid.
    if (portID_ >= 0)
    {
        parentProgEntity_->getMappingComponent()->getPortIDFactory().freeID(portID_);
    }

    portID_ = portID;

    if (portID_ >= 0)
    {
        parentProgEntity_->getMappingComponent()->getPortIDFactory().usedID(portID_);
    }

    updateText();
}

//-----------------------------------------------------------------------------
// Function: setHighlight()
//-----------------------------------------------------------------------------
void EndpointItem::setHighlight(HighlightMode mode)
{
    switch (mode)
    {
    case HIGHLIGHT_OFF:
        {
            setPen(QPen(Qt::black, 0));
            setZValue(0.0);
            break;
        }

    case HIGHLIGHT_ALLOWED:
        {
            setPen(QPen(Qt::blue, 2));
            setZValue(1000.0);
            break;
        }

    case HIGHLIGHT_HOVER:
        {
            setPen(QPen(Qt::red, 2));
            setZValue(1000.0);
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: addConnection()
//-----------------------------------------------------------------------------
void EndpointItem::addConnection(EndpointConnection* connection)
{
    connections_.append(connection);
}

//-----------------------------------------------------------------------------
// Function: removeConnection()
//-----------------------------------------------------------------------------
void EndpointItem::removeConnection(EndpointConnection* connection)
{
    connections_.removeAll(connection);
}

//-----------------------------------------------------------------------------
// Function: getInterconnections()
//-----------------------------------------------------------------------------
QList<EndpointConnection*> const& EndpointItem::getConnections() const
{
    return connections_;
}

//-----------------------------------------------------------------------------
// Function: isConnected()
//-----------------------------------------------------------------------------
bool EndpointItem::isConnected() const
{
    return (!connections_.empty());
}

//-----------------------------------------------------------------------------
// Function: setDirection()
//-----------------------------------------------------------------------------
void EndpointItem::setDirection(DrawDirection dir)
{
    // Update the visuals based on the new direction.
    if (dir == DIR_RIGHT)
    {
        textLabel_->setPos(-WIDTH / 2 + 5, 2 - HEIGHT / 2);
    }
    else
    {
        textLabel_->setPos(-WIDTH / 2 + ARROW_WIDTH + 5, 2 - HEIGHT / 2);
    }

    dir_ = dir;
    update();
}

//-----------------------------------------------------------------------------
// Function: getDirectionVector()
//-----------------------------------------------------------------------------
QVector2D EndpointItem::getDirectionVector() const
{
    switch (dir_)
    {
    case DIR_LEFT:
        {
            return QVector2D(-1.0f, 0.0f);
        }

    case DIR_RIGHT:
        {
            return QVector2D(1.0f, 0.0f);
        }

    default:
        {
            // Should not be possible.
            Q_ASSERT(false);
            return QVector2D();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: getDirection()
//-----------------------------------------------------------------------------
EndpointItem::DrawDirection EndpointItem::getDirection() const
{
    return dir_;
}

//-----------------------------------------------------------------------------
// Function: getName()
//-----------------------------------------------------------------------------
QString const& EndpointItem::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: getMCAPIDirection()
//-----------------------------------------------------------------------------
MCAPIEndpointDirection EndpointItem::getMCAPIDirection() const
{
    return type_;
}

//-----------------------------------------------------------------------------
// Function: getPortID()
//-----------------------------------------------------------------------------
int EndpointItem::getPortID() const
{
    return portID_;
}

//-----------------------------------------------------------------------------
// Function: boundingRect()
//-----------------------------------------------------------------------------
QRectF EndpointItem::boundingRect() const
{
    return QRectF(-WIDTH / 2, -HEIGHT / 2, WIDTH, HEIGHT);
}

//-----------------------------------------------------------------------------
// Function: shape()
//-----------------------------------------------------------------------------
QPainterPath EndpointItem::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

//-----------------------------------------------------------------------------
// Function: paint()
//-----------------------------------------------------------------------------
void EndpointItem::paint(QPainter* painter, QStyleOptionGraphicsItem const* option, QWidget*)
{
    // Determine the arrow offset.
    int arrowOffsetX = -WIDTH / 2;

    if (dir_ == DIR_RIGHT)
    {
        arrowOffsetX = WIDTH / 2 - ARROW_WIDTH;
    }

    // Draw the base rectangle.
    painter->setBrush(brush());
    painter->setPen(pen());
    painter->drawRect(boundingRect());

    // Draw the endpoint arrow background.
    painter->setBrush(QBrush(QColor(222, 222, 222)));
    painter->drawRect(QRectF(arrowOffsetX, -HEIGHT / 2, ARROW_WIDTH, HEIGHT));

    // Draw the actual arrow.
    painter->setBrush(QBrush(QColor(164, 164, 164)));
    QPolygon arrowPoly(4);

    if ((dir_ == DIR_LEFT && type_ == MCAPI_ENDPOINT_IN) ||
        (dir_ == DIR_RIGHT && type_ == MCAPI_ENDPOINT_OUT))
    {
        arrowPoly.putPoints(0, 4, arrowOffsetX, -HEIGHT / 2, arrowOffsetX + ARROW_WIDTH,
                            0, arrowOffsetX, HEIGHT / 2, arrowOffsetX, -HEIGHT / 2);
    }
    else
    {
        arrowPoly.putPoints(0, 4, arrowOffsetX + ARROW_WIDTH, -HEIGHT / 2, arrowOffsetX, 0,
                            arrowOffsetX + ARROW_WIDTH, HEIGHT / 2, arrowOffsetX + ARROW_WIDTH, -HEIGHT / 2);
    }

    painter->drawConvexPolygon(arrowPoly);

    // Draw a selection rectangle if the item is selected.
    if (option->state & QStyle::State_Selected)
    {
        painter->setBrush(Qt::NoBrush);

        painter->setPen(QPen(Qt::white, 0, Qt::SolidLine));
        painter->drawRect(boundingRect().adjusted(0.5, 0.5, -0.5, -0.5));

        painter->setPen(QPen(Qt::black, 0, Qt::DashLine));
        painter->drawRect(boundingRect().adjusted(0.5, 0.5, -0.5, -0.5));
    }
}

//-----------------------------------------------------------------------------
// Function: editEndpoint()
//-----------------------------------------------------------------------------
void EndpointItem::editEndpoint()
{
    EndpointEditDialog dialog(0, this);

    if (dialog.exec() == QDialog::Accepted)
    {
        setName(dialog.getName());
        setType(dialog.getType());
        setConnectionType(dialog.getConnectionType());
    }
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void EndpointItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseMoveEvent(event);

    setZValue(100);

    EndpointStack* stack = dynamic_cast<EndpointStack*>(parentItem());

    if (stack != 0)
    {
        stack->onMoveEndpoint(this);
    }
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void EndpointItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);

    EndpointStack* stack = dynamic_cast<EndpointStack*>(parentItem());

    if (stack != 0)
    {
        stack->onReleaseEndpoint(this);

        QSharedPointer<QUndoCommand> cmd;

        // Check if the port position was really changed.
        if (oldPos_ != pos())
        {
            cmd = QSharedPointer<QUndoCommand>(new EndpointMoveCommand(this, oldPos_));
        }
        else
        {
            cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
        }

        // End the position update of the connections.
        foreach (EndpointConnection* conn, getConnections())
        {
            conn->endUpdatePosition(cmd.data());
        }

        // Add the undo command to the edit stack only if it has changes.
        if (cmd->childCount() > 0 || oldPos_ != pos())
        {
            static_cast<EndpointDesignDiagram*>(scene())->getEditProvider().addCommand(cmd, false);
        }
    }

    setZValue(0);
}

//-----------------------------------------------------------------------------
// Function: mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void EndpointItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseDoubleClickEvent(event);
    
    if (event->button() == Qt::LeftButton)
    {
        editEndpoint();
    }
}

//-----------------------------------------------------------------------------
// Function: updateText()
//-----------------------------------------------------------------------------
void EndpointItem::updateText()
{
    if (parentProgEntity_->getMappingComponent() != 0)
    {
        textLabel_->setHtml("<b>Name: </b>" + name_ +
                            "<br><b>Endpoint:</b> &lt;HW, " +
                            QString::number(parentProgEntity_->getMappingComponent()->getID()) +
                             ", " + QString::number(portID_) + "&gt;");
        update();
    }
}

//-----------------------------------------------------------------------------
// Function: onConnect()
//-----------------------------------------------------------------------------
bool EndpointItem::onConnect(EndpointItem const*)
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: onDisconnect()
//-----------------------------------------------------------------------------
void EndpointItem::onDisconnect(EndpointItem const*)
{
}

//-----------------------------------------------------------------------------
// Function: canConnect()
//-----------------------------------------------------------------------------
bool EndpointItem::canConnect(EndpointItem const* other) const
{
    return (getConnections().empty() && type_ != other->type_ &&
            connType_ == other->connType_);
}

//-----------------------------------------------------------------------------
// Function: itemChange()
//-----------------------------------------------------------------------------
QVariant EndpointItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
    case ItemPositionHasChanged:
        {
            emit contentChanged();
            break;
        }

    case ItemScenePositionHasChanged:
        {
            foreach (EndpointConnection* connection, getConnections())
            {
                connection->updatePosition();
            }
            
            break;
        }
    }

    return QAbstractGraphicsShapeItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: getConnectionType()
//-----------------------------------------------------------------------------
MCAPIDataType EndpointItem::getConnectionType() const
{
    return connType_;
}

//-----------------------------------------------------------------------------
// Function: updateVisuals()
//-----------------------------------------------------------------------------
void EndpointItem::updateVisuals()
{
    switch (connType_)
    {
    case MCAPI_TYPE_MESSAGE:
        {
            setBrush(QBrush(QColor(215, 249, 217)));
            break;
        }

    case MCAPI_TYPE_SCALAR:
        {
            setBrush(QBrush(QColor(229, 215, 249)));
            break;
        }

    case MCAPI_TYPE_PACKET:
        {
            setBrush(QBrush(QColor(215, 241, 249)));
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: getParentProgramEntity()
//-----------------------------------------------------------------------------
ProgramEntityItem* EndpointItem::getParentProgramEntity() const
{
    return parentProgEntity_;
}

//-----------------------------------------------------------------------------
// Function: getFullName()
//-----------------------------------------------------------------------------
QString EndpointItem::getFullName() const
{
    return parentProgEntity_->name() + "_mcapiBus_" + name_;
}

//-----------------------------------------------------------------------------
// Function: getParentMappingComp()
//-----------------------------------------------------------------------------
MappingComponentItem* EndpointItem::getParentMappingComp() const
{
    return parentProgEntity_->getMappingComponent();
}

//-----------------------------------------------------------------------------
// Function: createBusInterface()
//-----------------------------------------------------------------------------
void EndpointItem::createBusInterface(MappingComponentItem* mappingComp)
{
    if (mappingComp == 0)
    {
        return;
    }
    
    // If the bus interface exists, just retrieve the port ID from it.
    BusInterface* oldBusIf = mappingComp->componentModel()->getBusInterface(getFullName());

    if (oldBusIf != 0)
    {
        setPortID(oldBusIf->getMCAPIPortID());
        return;
    }

    // Otherwise generate a new port ID.
    setPortID(mappingComp->getPortIDFactory().getID());

    // Add a bus interface to the mapping component.
    QSharedPointer<BusInterface> busIf(new BusInterface());
    busIf->setName(getFullName());
    busIf->setMCAPIPortID(portID_);

    if (type_ == MCAPI_ENDPOINT_OUT)
    {
        busIf->setInterfaceMode(General::MASTER);
    }
    else
    {
        busIf->setInterfaceMode(General::SLAVE);
    }

    switch (connType_)
    {
    case MCAPI_TYPE_MESSAGE:
        {
            busIf->setBusType(VLNV(VLNV::BUSDEFINITION, "Kactus", "internal", "mcapi_message", "1.0"));
            break;
        }

    case MCAPI_TYPE_PACKET:
        {
            busIf->setBusType(VLNV(VLNV::BUSDEFINITION, "Kactus", "internal", "mcapi_packet", "1.0"));
            break;
        }

    case MCAPI_TYPE_SCALAR:
        {
            busIf->setBusType(VLNV(VLNV::BUSDEFINITION, "Kactus", "internal", "mcapi_scalar", "1.0"));
            break;
        }
    }

    mappingComp->componentModel()->addBusInterface(busIf);
}

//-----------------------------------------------------------------------------
// Function: removeBusInterface()
//-----------------------------------------------------------------------------
void EndpointItem::removeBusInterface(MappingComponentItem* mappingComp)
{
    if (mappingComp != 0)
    {
        mappingComp->componentModel()->removeBusInterface(getFullName());

        // Reset the port id.
        setPortID(-1);
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void EndpointItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);

    oldPos_ = pos();

    // Begin position update for the connections.
    foreach (EndpointConnection* conn, getConnections())
    {
        conn->beginUpdatePosition();
    }
}
