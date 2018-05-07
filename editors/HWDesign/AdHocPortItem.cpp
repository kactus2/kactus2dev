//-----------------------------------------------------------------------------
// File: AdHocPortItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.2.2012
//
// Description:
// Diagram graphics item for ad-hoc ports.
//-----------------------------------------------------------------------------

#include "AdHocPortItem.h"

#include "HWMoveCommands.h"

#include <common/IEditProvider.h>
#include <common/graphicsItems/GraphicsConnection.h>

#include <editors/common/diagramgrid.h>
#include <editors/common/DesignDiagram.h>
#include <editors/common/NamelabelWidth.h>
#include <editors/common/GraphicsItemLabel.h>
#include <editors/HWDesign/HWComponentItem.h>

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
AdHocItem(port, parent->componentModel(), parent),
oldPos_(),
oldPortPositions_()
{
    setPolygon(getPortShape());

    updateInterface();
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::~AdHocPortItem()
//-----------------------------------------------------------------------------
AdHocPortItem::~AdHocPortItem()
{

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
    if (!AdHocItem::isConnectionValid(other))
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
// Function: AdHocPortItem::itemChange()
//-----------------------------------------------------------------------------
QVariant AdHocPortItem::itemChange(GraphicsItemChange change, QVariant const& value)
{
    if (change == ItemPositionChange)
    {
        QPointF pos = value.toPointF();

        if (parentItem())
        {
            QRectF parentRect = encompassingComp()->rect();
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
        setTieOffLabelPosition();
    }
    else if (change == ItemRotationHasChanged)
    {
        getNameLabel()->setRotation(-rotation());
        if (getTieOffLabel() != 0)
        {
            getTieOffLabel()->setRotation(-rotation());
        }
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
// Function: AdHocPortItem::moveItemByMouse()
//-----------------------------------------------------------------------------
void AdHocPortItem::moveItemByMouse()
{
    encompassingComp()->onMovePort(this);
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::saveOldPortPositions()
//-----------------------------------------------------------------------------
void AdHocPortItem::saveOldPortPositions()
{
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
        cmd = QSharedPointer<QUndoCommand>(new PortMoveCommand(this, oldPos_, diagram));
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
            new PortMoveCommand(static_cast<HWConnectionEndpoint*>(cur.key()), cur.value(), diagram, cmd.data());
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
        diagram->getEditProvider()->addCommand(cmd);
        cmd->redo();
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::setLabelPosition()
//-----------------------------------------------------------------------------
void AdHocPortItem::setLabelPosition()
{
    qreal nameWidth = getNameLabel()->boundingRect().width();
    qreal nameHeight = getNameLabel()->boundingRect().height();

	// Check if the port is directed to the left.
	if (pos().x() < 0)
	{
        getNameLabel()->setPos( nameHeight / 2, GridSize / 2 );
	}
	// Otherwise the port is directed to the right.
	else
	{
        getNameLabel()->setPos( -nameHeight / 2, GridSize / 2 + nameWidth);
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
    QFont font = getNameLabel()->font();

	return NamelabelWidth::getTextLength(name(), font);
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::shortenNameLabel()
//-----------------------------------------------------------------------------
void AdHocPortItem::shortenNameLabel( qreal width )
{
    QFont font = getNameLabel()->font();
	QString nameLabelText = NamelabelWidth::setNameLabel(name(), font, width);

	setLabelPosition();
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::tieOffLabelShouldBeDrawnLeft()
//-----------------------------------------------------------------------------
bool AdHocPortItem::labelShouldBeDrawnLeft() const
{
    return pos().x() < 0;
}
