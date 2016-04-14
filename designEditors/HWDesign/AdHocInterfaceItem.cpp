//-----------------------------------------------------------------------------
// File: AdHocInterfaceItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.2.2012
//
// Description:
// Diagram graphics item for ad-hoc ports.
//-----------------------------------------------------------------------------

#include "AdHocInterfaceItem.h"

#include "HWMoveCommands.h"
#include "OffPageConnectorItem.h"

#include <common/IEditProvider.h>

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsColumn.h>
#include <common/graphicsItems/CommonGraphicsUndoCommands.h>

#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/DesignDiagram.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>

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
// Function: AdHocInterfaceItem::AdHocInterfaceItem()
//-----------------------------------------------------------------------------
AdHocInterfaceItem::AdHocInterfaceItem(QSharedPointer<Component> component,
    QSharedPointer<Port> port, QSharedPointer<Kactus2Placeholder> dataGroup,
    QGraphicsItem *parent):
HWConnectionEndpoint(parent, QVector2D(1.0f, 0.0f)),
nameLabel_("", this),
component_(component),
port_(port),
dataGroup_(dataGroup),
oldColumn_(0),
oldPos_(),
oldInterfacePositions_(),
offPageConnector_()
{
    Q_ASSERT_X(port, "AdHocInterfaceItem constructor", "Null port pointer given as parameter");

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
            << QPointF(0, -squareSize)
            << QPointF(halfSquareSize, -halfSquareSize)
            << QPointF(halfSquareSize, halfSquareSize);
    }
    else if (port->getDirection() == DirectionTypes::OUT)
    {
        shape << QPointF(-halfSquareSize, halfSquareSize)
            << QPointF(-halfSquareSize, -halfSquareSize)
            << QPointF(halfSquareSize, -halfSquareSize)
            << QPointF(halfSquareSize, halfSquareSize)
            << QPointF(0, squareSize);
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

    if (dataGroup_->getAttributeValue("portName").isEmpty())
    {
        dataGroup_->setAttribute("portName", port_->name());
    }

    if (!dataGroup_->getAttributeValue("x").isEmpty())
    {
        QPointF position(dataGroup_->getAttributeValue("x").toInt(), dataGroup_->getAttributeValue("y").toInt());
        setPos(position);
    }

	QFont font = nameLabel_.font();
    font.setPointSize(8);
	nameLabel_.setFont(font);
	nameLabel_.setFlag(ItemStacksBehindParent);
    nameLabel_.setRotation(-rotation());

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setXOffset(0);
    shadow->setYOffset(0);
    shadow->setBlurRadius(5);
	nameLabel_.setGraphicsEffect(shadow);

    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemSendsScenePositionChanges);

    offPageConnector_ = new OffPageConnectorItem(this);
    offPageConnector_->setPos(0.0, -GridSize * 3);
    offPageConnector_->setFlag(ItemStacksBehindParent);
    offPageConnector_->setVisible(false);

    updateInterface();
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::~AdHocInterfaceItem()
//-----------------------------------------------------------------------------
AdHocInterfaceItem::~AdHocInterfaceItem()
{

}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::setTypes()
//-----------------------------------------------------------------------------
void AdHocInterfaceItem::setTypes(VLNV const&, VLNV const&, General::InterfaceMode)
{
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::updateInterface()
//-----------------------------------------------------------------------------
void AdHocInterfaceItem::updateInterface()
{
    HWConnectionEndpoint::updateInterface();

    if (adhocPortIsValid())
    {
        setBrush(QBrush(Qt::black));
    }
    else
    {
        setBrush(QBrush(Qt::red));
    }

    nameLabel_.setHtml("<div style=\"background-color:#eeeeee; padding:10px 10px;\">" + port_->name() + "</div>");

    setLabelPosition();

    offPageConnector_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::adhocPortIsValid()
//-----------------------------------------------------------------------------
bool AdHocInterfaceItem::adhocPortIsValid() const
{
    if (component_->getPort(port_->name()))
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::name()
//-----------------------------------------------------------------------------
QString AdHocInterfaceItem::name() const
{
    return port_->name();
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::setName()
//-----------------------------------------------------------------------------
void AdHocInterfaceItem::setName(QString const& name)
{
    beginUpdateConnectionNames();

	port_->setName(name);
    dataGroup_->setAttribute("portName", name);

	updateInterface();
    emit contentChanged();

    endUpdateConnectionNames();
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::description()
//-----------------------------------------------------------------------------
QString AdHocInterfaceItem::description() const
{
    Q_ASSERT(port_);
    return port_->description();
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::setDescription()
//-----------------------------------------------------------------------------
void AdHocInterfaceItem::setDescription(QString const& description)
{
    Q_ASSERT(port_);
    port_->setDescription(description);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::onConnect()
//-----------------------------------------------------------------------------
bool AdHocInterfaceItem::onConnect(ConnectionEndpoint const*)
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::onDisonnect()
//-----------------------------------------------------------------------------
void AdHocInterfaceItem::onDisconnect(ConnectionEndpoint const*)
{
}

//-----------------------------------------------------------------------------
// Function: isConnectionValid()
//-----------------------------------------------------------------------------
bool AdHocInterfaceItem::isConnectionValid(ConnectionEndpoint const* other) const
{
    // Ad-hoc connection is not possible to a bus end point.
    return HWConnectionEndpoint::isConnectionValid(other) && !other->isBus();
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::isExclusive()
//-----------------------------------------------------------------------------
bool AdHocInterfaceItem::isExclusive() const
{
    // Ad-hoc interfaces are always non-exclusive.
    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::encompassingComp()
//-----------------------------------------------------------------------------
ComponentItem* AdHocInterfaceItem::encompassingComp() const
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::getOwnerComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> AdHocInterfaceItem::getOwnerComponent() const
{
    return component_;
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::getBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> AdHocInterfaceItem::getBusInterface() const
{
    return QSharedPointer<BusInterface>();
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::getPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> AdHocInterfaceItem::getPort() const
{
    return port_;
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::isHierarchical()
//-----------------------------------------------------------------------------
bool AdHocInterfaceItem::isHierarchical() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::isBus()
//-----------------------------------------------------------------------------
bool AdHocInterfaceItem::isBus() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::setDirection()
//-----------------------------------------------------------------------------
void AdHocInterfaceItem::setDirection(QVector2D const& dir)
{
    HWConnectionEndpoint::setDirection(dir);
    setLabelPosition();
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::isDirectionFixed()
//-----------------------------------------------------------------------------
bool AdHocInterfaceItem::isDirectionFixed() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::setInterfaceMode()
//-----------------------------------------------------------------------------
void AdHocInterfaceItem::setInterfaceMode(General::InterfaceMode)
{
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::getOffPageConnector()
//-----------------------------------------------------------------------------
ConnectionEndpoint* AdHocInterfaceItem::getOffPageConnector()
{
    return offPageConnector_;
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::setLabelPosition()
//-----------------------------------------------------------------------------
void AdHocInterfaceItem::setLabelPosition()
{
    qreal nameWidth = nameLabel_.boundingRect().width();

    // Check if the port is directed to the left.
    if (getDirection().x() < 0)
    {
        nameLabel_.setPos(0, GridSize * 3.0 / 4.0 - nameWidth / 2.0);
    }
    // Otherwise the port is directed to the right.
    else
    {
        nameLabel_.setPos(0, GridSize * 3.0 / 4.0 + nameWidth / 2.0);
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::itemChange()
//-----------------------------------------------------------------------------
QVariant AdHocInterfaceItem::itemChange(GraphicsItemChange change, QVariant const& value)
{
    if (change == ItemPositionChange)
    {
        return snapPointToGrid(value.toPointF());
    }
    else if (change == ItemRotationHasChanged)
    {
        nameLabel_.setRotation(-rotation());
    }
    else if (change == ItemScenePositionHasChanged)
    {
        dataGroup_->setAttribute("x", QString::number(value.toPointF().x()));
        dataGroup_->setAttribute("y", QString::number(value.toPointF().y()));

        foreach (GraphicsConnection* interconnection, getConnections())
        {
            interconnection->updatePosition();
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::mousePressEvent()
//-----------------------------------------------------------------------------
void AdHocInterfaceItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    HWConnectionEndpoint::mousePressEvent(event);
    setZValue(1001.0);

    oldColumn_ = dynamic_cast<GraphicsColumn*>(parentItem());
    oldPos_ = scenePos();
    Q_ASSERT(oldColumn_ != 0);

    foreach (QGraphicsItem* item, scene()->items()) //column->childItems())
    {
        if (item->type() == GFX_TYPE_DIAGRAM_INTERFACE || item->type() == AdHocInterfaceItem::Type)
        {
            oldInterfacePositions_.insert(item, item->scenePos());
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
// Function: AdHocInterfaceItem::mouseMoveEvent()
//-----------------------------------------------------------------------------
void AdHocInterfaceItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // Discard mouse move if the diagram is protected.
    if (static_cast<DesignDiagram*>(scene())->isProtected())
    {
        return;
    }

    HWConnectionEndpoint::mouseMoveEvent(event);

    setPos(parentItem()->mapFromScene(oldColumn_->mapToScene(pos())));

    GraphicsColumn* column = dynamic_cast<GraphicsColumn*>(parentItem());
    Q_ASSERT(column != 0);
    column->onMoveItem(this);
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void AdHocInterfaceItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    HWConnectionEndpoint::mouseReleaseEvent(event);
    setZValue(0.0);

    if (oldColumn_ != 0)
    {
        GraphicsColumn* column = dynamic_cast<GraphicsColumn*>(parentItem());
        Q_ASSERT(column != 0);
        column->onReleaseItem(this);

        QSharedPointer<QUndoCommand> cmd;

        // Check if the interface position was really changed.
        if (oldPos_ != scenePos())
        {
            cmd = QSharedPointer<QUndoCommand>(new ItemMoveCommand(this, oldPos_, oldColumn_));
        }
        else
        {
            cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
        }

        // Determine if the other interfaces changed their position and create undo commands for them.
        QMap<QGraphicsItem*, QPointF>::iterator cur = oldInterfacePositions_.begin();
        while (cur != oldInterfacePositions_.end())
        {
            if (cur.key()->scenePos() != cur.value())
            {
                new ItemMoveCommand(cur.key(), cur.value(), oldColumn_, cmd.data());
            }

            cur++;
        }

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
        if (cmd->childCount() > 0 || oldPos_ != scenePos())
        {
            static_cast<DesignDiagram*>(scene())->getEditProvider()->addCommand(cmd);
        }

        oldColumn_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::getDataGroup()
//-----------------------------------------------------------------------------
QSharedPointer<Kactus2Placeholder> AdHocInterfaceItem::getDataGroup() const
{
    return dataGroup_;
}
