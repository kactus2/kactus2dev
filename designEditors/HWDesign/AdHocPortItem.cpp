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
#include "HWConnection.h"
#include "BusInterfaceDialog.h"
#include "HWMoveCommands.h"
#include "OffPageConnectorItem.h"

#include <common/IEditProvider.h>
#include <common/graphicsItems/GraphicsConnection.h>


#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/DesignDiagram.h>
#include <designEditors/common/NamelabelWidth.h>

#include <IPXACTmodels/common/DirectionTypes.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QColor>
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QVector2D>
#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::AdHocPortItem()
//-----------------------------------------------------------------------------
AdHocPortItem::AdHocPortItem(QSharedPointer<Port> port, HWComponentItem* parent) : 
HWConnectionEndpoint(parent),
    nameLabel_("", this),
    parentComponentItem_(parent),
    port_(port),
    oldPos_(), 
    oldPortPositions_(),
    offPageConnector_()
{
    Q_ASSERT_X(port, "AdHocPortItem constructor", "Null Port pointer given as parameter");

    setType(ENDPOINT_TYPE_ADHOC);

    int squareSize = GridSize - 4;
    int halfSquareSize = squareSize / 2;
    /*  /\
     *  ||
     */
    // Create the port shape based on the port direction.
    QPolygonF shape;
    if (port->getDirection() == DirectionTypes::IN)
    {
        shape << QPointF(-halfSquareSize, halfSquareSize)
            << QPointF(-halfSquareSize, -halfSquareSize)
            << QPointF(halfSquareSize, -halfSquareSize)
            << QPointF(halfSquareSize, halfSquareSize)
            << QPointF(0, squareSize);
    }
    else if (port->getDirection() == DirectionTypes::OUT)
    {
        shape << QPointF(-halfSquareSize, halfSquareSize)
            << QPointF(-halfSquareSize, -halfSquareSize)
            << QPointF(0, -squareSize)
            << QPointF(halfSquareSize, -halfSquareSize)
            << QPointF(halfSquareSize, halfSquareSize);
    }
    else
    {
        shape << QPointF(-halfSquareSize, halfSquareSize)
            << QPointF(-halfSquareSize, -halfSquareSize)
            << QPointF(0, -squareSize)
            << QPointF(halfSquareSize, -halfSquareSize)
            << QPointF(halfSquareSize, halfSquareSize)
            << QPointF(0, squareSize);
    }

    setPolygon(shape);

    QFont font = nameLabel_.font();
	font.setPointSize(8);
	nameLabel_.setFont(font);
	nameLabel_.setFlag(ItemStacksBehindParent);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setXOffset(0);
    shadow->setYOffset(0);
    shadow->setBlurRadius(5);
	nameLabel_.setGraphicsEffect(shadow);

    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemSendsScenePositionChanges);

    // Create the off-page connector.
    offPageConnector_ = new OffPageConnectorItem(this);
    offPageConnector_->setPos(0.0, -GridSize * 3);
    offPageConnector_->setFlag(ItemStacksBehindParent);
    offPageConnector_->setVisible(false);

    updateInterface();
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::~AdHocPortItem()
//-----------------------------------------------------------------------------
AdHocPortItem::~AdHocPortItem()
{
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::name()
//-----------------------------------------------------------------------------
QString AdHocPortItem::name() const
{
    return port_->name();
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::setName()
//-----------------------------------------------------------------------------
void AdHocPortItem::setName(QString const& name)
{
    beginUpdateConnectionNames();

	port_->setName(name);
	updateInterface();
    emit contentChanged();

    endUpdateConnectionNames();
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::getBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> AdHocPortItem::getBusInterface() const
{
    return QSharedPointer<BusInterface>();
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::updateInterface()
//-----------------------------------------------------------------------------
void AdHocPortItem::updateInterface()
{
    HWConnectionEndpoint::updateInterface();

    setBrush(QBrush(Qt::black));

	nameLabel_.setHtml("<div style=\"background-color:#eeeeee; padding:10px 10px;\">" +	port_->name() + "</div>");

	setLabelPosition();

    offPageConnector_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::isHierarchical()
//-----------------------------------------------------------------------------
bool AdHocPortItem::isHierarchical() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::onConnect()
//-----------------------------------------------------------------------------
bool AdHocPortItem::onConnect(ConnectionEndpoint const*)
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::onDisonnect()
//-----------------------------------------------------------------------------
void AdHocPortItem::onDisconnect(ConnectionEndpoint const*)
{
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

    if (other->isHierarchical())
    {
        return (port_->getDirection() == DirectionTypes::INOUT ||
                other->getPort()->getDirection() == DirectionTypes::INOUT ||
                port_->getDirection() == other->getPort()->getDirection());

    }
    else
    {
        return (port_->getDirection() == DirectionTypes::INOUT ||
            other->getPort()->getDirection() == DirectionTypes::INOUT ||
            (port_->getDirection() == DirectionTypes::IN && other->getPort()->getDirection() == DirectionTypes::OUT) ||
            (port_->getDirection() == DirectionTypes::OUT && other->getPort()->getDirection() == DirectionTypes::IN));
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
        nameLabel_.setRotation(-rotation());
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
// Function: AdHocPortItem::setTypes()
//-----------------------------------------------------------------------------
void AdHocPortItem::setTypes(VLNV const&, VLNV const&, General::InterfaceMode)
{
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
        port_->setDefaultPos(pos());

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
// Function: AdHocPortItem::setInterfaceMode()
//-----------------------------------------------------------------------------
void AdHocPortItem::setInterfaceMode(General::InterfaceMode)
{
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::description()
//-----------------------------------------------------------------------------
QString AdHocPortItem::description() const
{
	Q_ASSERT(port_);
    return port_->description();
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::setDescription()
//-----------------------------------------------------------------------------
void AdHocPortItem::setDescription(QString const& description)
{
	Q_ASSERT(port_);
	port_->setDescription(description);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::getOffPageConnector()
//-----------------------------------------------------------------------------
ConnectionEndpoint* AdHocPortItem::getOffPageConnector()
{
    return offPageConnector_;
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::isBus()
//-----------------------------------------------------------------------------
bool AdHocPortItem::isBus() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::getPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> AdHocPortItem::getPort() const
{
    return port_;
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::isExclusive()
//-----------------------------------------------------------------------------
bool AdHocPortItem::isExclusive() const
{
    // Ad-hoc ports are always non-exclusive.
    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::setLabelPosition()
//-----------------------------------------------------------------------------
void AdHocPortItem::setLabelPosition()
{
	qreal nameWidth = nameLabel_.boundingRect().width();
	qreal nameHeight = nameLabel_.boundingRect().height();

	// Check if the port is directed to the left.
	if (pos().x() < 0)
	{
		nameLabel_.setPos( nameHeight / 2, GridSize / 2 );
	}
	// Otherwise the port is directed to the right.
	else
	{
		nameLabel_.setPos( -nameHeight / 2, GridSize / 2 + nameWidth);
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
	QFont font = nameLabel_.font();

	return NamelabelWidth::getTextLength(name(), font);
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::shortenNameLabel()
//-----------------------------------------------------------------------------
void AdHocPortItem::shortenNameLabel( qreal width )
{
	QFont font = nameLabel_.font();
	QString nameLabelText = NamelabelWidth::setNameLabel(name(), font, width);

	nameLabel_.setHtml("<div style=\"background-color:#eeeeee; padding:10px 10px;\">" + nameLabelText + "</div>");

	setLabelPosition();
}