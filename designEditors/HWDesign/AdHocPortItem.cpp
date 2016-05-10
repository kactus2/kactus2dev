//-----------------------------------------------------------------------------
// File: AdHocPortItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.2.2012
//
// Description:
// Diagram graphics item for ad-hoc ports.
//-----------------------------------------------------------------------------

#include "AdHocPortItem.h"

#include "HWComponentItem.h"
#include "HWMoveCommands.h"
#include "OffPageConnectorItem.h"

#include <common/IEditProvider.h>
#include <common/graphicsItems/GraphicsConnection.h>

#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/DesignDiagram.h>
#include <designEditors/common/NamelabelWidth.h>

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>

#include <QBrush>
#include <QFont>
#include <QVector2D>
#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::AdHocPortItem()
//-----------------------------------------------------------------------------
AdHocPortItem::AdHocPortItem(QSharedPointer<Port> port, HWComponentItem* parent):
AdHocItem(port, parent),
parentComponentItem_(parent),
oldPos_(),
oldPortPositions_()
{
    setPolygon(getPortShape());

    updateInterface();
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::getInPortShape()
//-----------------------------------------------------------------------------
QPolygonF AdHocPortItem::getInPortShape(const int squareSize) const
{
    QPolygonF shape;

    int halfSquare = squareSize / 2;

    shape << QPointF(-halfSquare, halfSquare)
        << QPointF(-halfSquare, -halfSquare)
        << QPointF(halfSquare, -halfSquare)
        << QPointF(halfSquare, halfSquare)
        << QPointF(0, squareSize);

    return shape;
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::getOutPortShape()
//-----------------------------------------------------------------------------
QPolygonF AdHocPortItem::getOutPortShape(const int squareSize) const
{
    QPolygonF shape;

    int halfSquare = squareSize / 2;

    shape << QPointF(-halfSquare, halfSquare)
        << QPointF(-halfSquare, -halfSquare)
        << QPointF(0, -squareSize)
        << QPointF(halfSquare, -halfSquare)
        << QPointF(halfSquare, halfSquare);

    return shape;
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::~AdHocPortItem()
//-----------------------------------------------------------------------------
AdHocPortItem::~AdHocPortItem()
{

}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::updateInterface()
//-----------------------------------------------------------------------------
void AdHocPortItem::updateInterface()
{
    HWConnectionEndpoint::updateInterface();

    if (adHocPortExists())
    {
        setBrush(QBrush(Qt::black));
    }
    else
    {
        setBrush(QBrush(Qt::red));
    }

    getNameLabel().setHtml(
        "<div style=\"background-color:#eeeeee; padding:10px 10px;\">" + getPort()->name() + "</div>");

	setLabelPosition();

    getOffPageConnector()->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::adHocPortExists()
//-----------------------------------------------------------------------------
bool AdHocPortItem::adHocPortExists() const
{
    if (parentComponentItem_->componentModel()->getPort(getPort()->name()))
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::isHierarchical()
//-----------------------------------------------------------------------------
bool AdHocPortItem::isHierarchical() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::isConnectionValid()
//-----------------------------------------------------------------------------
bool AdHocPortItem::isConnectionValid(ConnectionEndpoint const* other) const
{
    if (!HWConnectionEndpoint::isConnectionValid(other) || !other->isAdHoc())
    {
        return false;
    }

    QSharedPointer<Port> thisPort = getPort();
    QSharedPointer<Port> otherPort = other->getPort();

    if (other->isHierarchical())
    {
        return (thisPort->getDirection() == DirectionTypes::INOUT ||
            otherPort->getDirection() == DirectionTypes::INOUT ||
            thisPort->getDirection() == other->getPort()->getDirection());
    }
    else
    {
        return (thisPort->getDirection() == DirectionTypes::INOUT ||
            otherPort->getDirection() == DirectionTypes::INOUT ||
            (thisPort->getDirection() == DirectionTypes::IN && otherPort->getDirection() == DirectionTypes::OUT) ||
            (thisPort->getDirection() == DirectionTypes::OUT && otherPort->getDirection() == DirectionTypes::IN));
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::encompassingComp()
//-----------------------------------------------------------------------------
ComponentItem* AdHocPortItem::encompassingComp() const
{
    return static_cast<ComponentItem*>(parentItem());
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::getOwnerComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> AdHocPortItem::getOwnerComponent() const
{
	ComponentItem* comp = encompassingComp();
	Q_ASSERT(comp);

	QSharedPointer<Component> compModel = comp->componentModel();
	Q_ASSERT(compModel);

	return compModel;
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::itemChange()
//-----------------------------------------------------------------------------
QVariant AdHocPortItem::itemChange(GraphicsItemChange change, QVariant const& value)
{
    if (change == ItemPositionChange)
    {
        QPointF pos = value.toPointF();

        if (parentItem())
        {
            QRectF parentRect = parentComponentItem_->rect();
            if (pos.x() < 0)
            {
                pos.setX(parentRect.left());
            }
            else
            {
                pos.setX(parentRect.right());
            }
        }

        return snapPointToGrid(pos);
    }
    else if (change == ItemPositionHasChanged && parentItem())
    {
        checkDirection();
        setLabelPosition();
    }
    else if (change == ItemRotationHasChanged)
    {
        getNameLabel().setRotation(-rotation());
    }
    else if (change == ItemScenePositionHasChanged)
    {
        // Update the connections.
        foreach (GraphicsConnection *interconnection, getConnections())
        {
            interconnection->updatePosition();
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::isDirectionFixed()
//-----------------------------------------------------------------------------
bool AdHocPortItem::isDirectionFixed() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::mouseMoveEvent()
//-----------------------------------------------------------------------------
void AdHocPortItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // Discard mouse move if the diagram is protected.
    DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(scene());
    if (diagram != 0 && diagram->isProtected())
    {
        return;
    }

    HWConnectionEndpoint::mouseMoveEvent(event);
    parentComponentItem_->onMovePort(this);
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::mousePressEvent()
//-----------------------------------------------------------------------------
void AdHocPortItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    HWConnectionEndpoint::mousePressEvent(event);    
    oldPos_ = pos();

    // Save old port positions for all ports in the parent component.
    foreach (QGraphicsItem* item, parentItem()->childItems())
    {
        if (dynamic_cast<ConnectionEndpoint*>(item) != 0 && item != this)
        {
            ConnectionEndpoint* port = static_cast<ConnectionEndpoint*>(item);
            oldPortPositions_.insert(port, port->pos());
        }
    }

    // Begin the position update for all connections.
    foreach (QGraphicsItem *item, scene()->items())
    {
        GraphicsConnection* conn = dynamic_cast<GraphicsConnection*>(item);
        if (conn != 0)
        {
            conn->beginUpdatePosition();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void AdHocPortItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    HWConnectionEndpoint::mouseReleaseEvent(event);

    DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(scene());
    if (diagram == 0)
    {
        // Update the default position in case the graphics are located in other scene than the designer.
        getPort()->setDefaultPos(pos());

        if (oldPos_ != pos())
        {
            emit moved(this);
        }

        return;
    }

    QSharedPointer<QUndoCommand> cmd;

    // Check if the port position was really changed.
    if (oldPos_ != pos())
    {
        cmd = QSharedPointer<QUndoCommand>(new PortMoveCommand(this, oldPos_));
    }
    else
    {
        cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
    }

    // Determine if the other ports changed their position and create undo commands for them.
    QMap<ConnectionEndpoint*, QPointF>::iterator cur = oldPortPositions_.begin();
    while (cur != oldPortPositions_.end())
    {
        if (cur.key()->pos() != cur.value())
        {
            new PortMoveCommand(static_cast<HWConnectionEndpoint*>(cur.key()), cur.value(), cmd.data());
        }

        cur++;
    }

    oldPortPositions_.clear();
    
    // End the position update for all connections.
    foreach (QGraphicsItem *item, scene()->items())
    {
        GraphicsConnection* conn = dynamic_cast<GraphicsConnection*>(item);
        if (conn != 0)
        {
            conn->endUpdatePosition(cmd.data());
        }
    }

    // Add the undo command to the edit stack only if it has changes.
    if (cmd->childCount() > 0 || oldPos_ != pos())
    {
        static_cast<DesignDiagram*>(scene())->getEditProvider()->addCommand(cmd);
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::setLabelPosition()
//-----------------------------------------------------------------------------
void AdHocPortItem::setLabelPosition()
{
    qreal nameWidth = getNameLabel().boundingRect().width();
    qreal nameHeight = getNameLabel().boundingRect().height();

	// Check if the port is directed to the left.
	if (pos().x() < 0)
	{
        getNameLabel().setPos( nameHeight / 2, GridSize / 2 );
	}
	// Otherwise the port is directed to the right.
	else
	{
        getNameLabel().setPos( -nameHeight / 2, GridSize / 2 + nameWidth);
	}
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::checkDirection()
//-----------------------------------------------------------------------------
void AdHocPortItem::checkDirection()
{
	// Check if the port is directed to the left.
	if (pos().x() < 0)
	{
		setDirection(QVector2D(-1.0f, 0.0f));
	}
	// Otherwise the port is directed to the right.
	else
	{
		setDirection(QVector2D(1.0f, 0.0f));
	}
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::getNameLength()
//-----------------------------------------------------------------------------
qreal AdHocPortItem::getNameLength()
{
    QFont font = getNameLabel().font();

	return NamelabelWidth::getTextLength(name(), font);
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::shortenNameLabel()
//-----------------------------------------------------------------------------
void AdHocPortItem::shortenNameLabel( qreal width )
{
    QFont font = getNameLabel().font();
	QString nameLabelText = NamelabelWidth::setNameLabel(name(), font, width);

    getNameLabel().setHtml(
        "<div style=\"background-color:#eeeeee; padding:10px 10px;\">" + nameLabelText + "</div>");

	setLabelPosition();
}
