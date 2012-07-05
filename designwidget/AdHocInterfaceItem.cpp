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

#include "HWComponentItem.h"
#include "HWConnection.h"
#include "BusInterfaceItem.h"
#include "BusInterfaceDialog.h"
#include "HWMoveCommands.h"
#include "HWDesignDiagram.h"
#include "OffPageConnectorItem.h"

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsColumn.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/GenericEditProvider.h>
#include <common/diagramgrid.h>

#include <models/businterface.h>
#include <models/component.h>
#include <models/busdefinition.h>

#include <LibraryManager/libraryinterface.h>

#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QColor>
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QVector2D>
#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem()
//-----------------------------------------------------------------------------
AdHocInterfaceItem::AdHocInterfaceItem(QSharedPointer<Component> component,
                                             Port* port, LibraryInterface* lh,
                                             QGraphicsItem *parent)
    : HWConnectionEndpoint(parent, false, QVector2D(1.0f, 0.0f)),
      lh_(lh),
      nameLabel_(0),
      port_(port),
      component_(component),
      oldColumn_(0),
      oldPos_(),
      oldInterfacePositions_(),
      offPageConnector_(0)
{
    Q_ASSERT_X(port, "AdHocInterfaceItem constructor",
               "Null port pointer given as parameter");

    setType(ENDPOINT_TYPE_ADHOC);

    int squareSize = GridSize - 4;
    int halfSquareSize = squareSize / 2;
    /*  /\
     *  ||
     */
    // Create the port shape based on the port direction.
    QPolygonF shape;

    switch (port->getDirection())
    {
    case General::IN:
        {
            shape << QPointF(-halfSquareSize, halfSquareSize)
                << QPointF(-halfSquareSize, -halfSquareSize)
                << QPointF(0, -squareSize)
                << QPointF(halfSquareSize, -halfSquareSize)
                << QPointF(halfSquareSize, halfSquareSize);
            break;
        }

    case General::OUT:
        {
            shape << QPointF(-halfSquareSize, halfSquareSize)
                << QPointF(-halfSquareSize, -halfSquareSize)
                << QPointF(halfSquareSize, -halfSquareSize)
                << QPointF(halfSquareSize, halfSquareSize)
                << QPointF(0, squareSize);
            break;
        }

    case General::INOUT:
        {
            shape << QPointF(-halfSquareSize, halfSquareSize)
                  << QPointF(-halfSquareSize, -halfSquareSize)
                  << QPointF(0, -squareSize)
                  << QPointF(halfSquareSize, -halfSquareSize)
                  << QPointF(halfSquareSize, halfSquareSize)
                  << QPointF(0, squareSize);
            break;
        }
    }

    setPolygon(shape);

    nameLabel_ = new QGraphicsTextItem("", this);
    QFont font = nameLabel_->font();
    font.setPointSize(8);
    nameLabel_->setFont(font);
    nameLabel_->setFlag(ItemIgnoresTransformations);
    nameLabel_->setFlag(ItemStacksBehindParent);
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setXOffset(0);
    shadow->setYOffset(0);
    shadow->setBlurRadius(5);
    nameLabel_->setGraphicsEffect(shadow);

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
// Function: ~AdHocInterfaceItem()
//-----------------------------------------------------------------------------
AdHocInterfaceItem::~AdHocInterfaceItem()
{
    GraphicsColumn* column = dynamic_cast<GraphicsColumn*>(parentItem());

    if (column != 0)
    {
        column->removeItem(this);
    }
}

//-----------------------------------------------------------------------------
// Function: name()
//-----------------------------------------------------------------------------
QString AdHocInterfaceItem::name() const
{
    return port_->getName();
}

//-----------------------------------------------------------------------------
// Function: setName()
//-----------------------------------------------------------------------------
void AdHocInterfaceItem::setName(QString const& name)
{
	port_->setName(name);
	updateInterface();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: getBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> AdHocInterfaceItem::getBusInterface() const
{
    return QSharedPointer<BusInterface>();
}

//-----------------------------------------------------------------------------
// Function: updateInterface()
//-----------------------------------------------------------------------------
void AdHocInterfaceItem::updateInterface()
{
    HWConnectionEndpoint::updateInterface();

    setBrush(QBrush(Qt::black));

    nameLabel_->setHtml("<div style=\"background-color:#eeeeee; padding:10px 10px;\">" +
                        port_->getName() + "</div>");

    qreal nameWidth = nameLabel_->boundingRect().width();

    // Check if the port is directed to the left.
    if (getDirection().x() < 0)
    {
        nameLabel_->setPos(0, GridSize * 3.0 / 4.0 - nameWidth / 2.0);
    }
    // Otherwise the port is directed to the right.
    else
    {
        nameLabel_->setPos(0, GridSize * 3.0 / 4.0 + nameWidth / 2.0);
    }

    offPageConnector_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: isHierarchical()
//-----------------------------------------------------------------------------
bool AdHocInterfaceItem::isHierarchical() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: onConnect()
//-----------------------------------------------------------------------------
bool AdHocInterfaceItem::onConnect(ConnectionEndpoint const*)
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: onDisonnect()
//-----------------------------------------------------------------------------
void AdHocInterfaceItem::onDisconnect(ConnectionEndpoint const*)
{
}

//-----------------------------------------------------------------------------
// Function: isConnectionValid()
//-----------------------------------------------------------------------------
bool AdHocInterfaceItem::isConnectionValid(ConnectionEndpoint const* other) const
{
    if (!HWConnectionEndpoint::isConnectionValid(other))
    {
        return false;
    }

    // Ad-hoc connection is not possible to a bus end point.
    if (other->isBus())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: encompassingComp()
//-----------------------------------------------------------------------------
ComponentItem* AdHocInterfaceItem::encompassingComp() const
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: getOwnerComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> AdHocInterfaceItem::getOwnerComponent() const
{
    return component_;
}

//-----------------------------------------------------------------------------
// Function: itemChange()
//-----------------------------------------------------------------------------
QVariant AdHocInterfaceItem::itemChange(GraphicsItemChange change,
                                 const QVariant &value)
{
    switch (change) {

    case ItemPositionChange:
        {
            return snapPointToGrid(value.toPointF());
        }

    case ItemPositionHasChanged:
        {
            if (!parentItem())
                break;

            break;
        }

    case ItemScenePositionHasChanged:

        foreach (GraphicsConnection *interconnection, getConnections()) {
            interconnection->updatePosition();
        }

        break;

    default:
        break;
    }

    return QGraphicsItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: isDirectionFixed()
//-----------------------------------------------------------------------------
bool AdHocInterfaceItem::isDirectionFixed() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void AdHocInterfaceItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // Discard mouse move if the diagram is protected.
    if (static_cast<HWDesignDiagram*>(scene())->isProtected())
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
// Function: setTypes()
//-----------------------------------------------------------------------------
void AdHocInterfaceItem::setTypes(VLNV const&, VLNV const&, General::InterfaceMode)
{
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void AdHocInterfaceItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    HWConnectionEndpoint::mousePressEvent(event);
    setZValue(1001.0);

    oldColumn_ = dynamic_cast<GraphicsColumn*>(parentItem());
    oldPos_ = scenePos();
    Q_ASSERT(oldColumn_ != 0);

    // Save the positions of the other diagram/ad-hoc interfaces.
    foreach (GraphicsColumn* column, oldColumn_->getLayout().getColumns())
    {
        if (column->getContentType() == COLUMN_CONTENT_IO)
        {
            foreach (QGraphicsItem* item, column->childItems())
            {
                if (item->type() == BusInterfaceItem::Type ||
                    item->type() == AdHocInterfaceItem::Type)
                {
                    oldInterfacePositions_.insert(item, item->scenePos());
                }
            }
        }
    }

    // Begin the position update of the connections.
    foreach (GraphicsConnection* conn, getConnections())
    {
        conn->beginUpdatePosition();
    }
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
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

        oldColumn_ = 0;

        QSharedPointer<QUndoCommand> cmd;

        // Check if the interface position was really changed.
        if (oldPos_ != scenePos())
        {
            cmd = QSharedPointer<QUndoCommand>(new ItemMoveCommand(this, oldPos_));
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
                QUndoCommand* childCmd = new ItemMoveCommand(cur.key(), cur.value(), cmd.data());
            }

            ++cur;
        }

        // End the position update of the connections.
        foreach (GraphicsConnection* conn, getConnections())
        {
            conn->endUpdatePosition(cmd.data());
        }

        // Add the undo command to the edit stack only if it has changes.
        if (cmd->childCount() > 0 || oldPos_ != scenePos())
        {
            static_cast<DesignDiagram*>(scene())->getEditProvider().addCommand(cmd, false);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: setDirection()
//-----------------------------------------------------------------------------
void AdHocInterfaceItem::setDirection(QVector2D const& dir)
{
    HWConnectionEndpoint::setDirection(dir);

    // Update the position of the name label based on the direction.
    qreal nameWidth = nameLabel_->boundingRect().width();

    // Check if the interface is directed to the left.
    if (dir.x() < 0)
    {
        nameLabel_->setPos(0, GridSize * 3.0 / 4.0 - nameWidth / 2.0);
    }
    // Otherwise the interface is directed to the right.
    else
    {
        nameLabel_->setPos(0, GridSize * 3.0 / 4.0 + nameWidth / 2.0);
    }
}

//-----------------------------------------------------------------------------
// Function: setInterfaceMode()
//-----------------------------------------------------------------------------
void AdHocInterfaceItem::setInterfaceMode(General::InterfaceMode)
{
}

//-----------------------------------------------------------------------------
// Function: description()
//-----------------------------------------------------------------------------
QString AdHocInterfaceItem::description() const
{
	Q_ASSERT(port_);
    return port_->getDescription();
}

//-----------------------------------------------------------------------------
// Function: setDescription()
//-----------------------------------------------------------------------------
void AdHocInterfaceItem::setDescription(QString const& description)
{
	Q_ASSERT(port_);
	port_->setDescription(description);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: getOffPageConnector()
//-----------------------------------------------------------------------------
ConnectionEndpoint* AdHocInterfaceItem::getOffPageConnector()
{
    return offPageConnector_;
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::isBus()
//-----------------------------------------------------------------------------
bool AdHocInterfaceItem::isBus() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::getPort()
//-----------------------------------------------------------------------------
Port* AdHocInterfaceItem::getPort() const
{
    return port_;
}

//-----------------------------------------------------------------------------
// Function: AdHocInterfaceItem::isExclusive()
//-----------------------------------------------------------------------------
bool AdHocInterfaceItem::isExclusive() const
{
    // Ad-hoc interfaces are always non-exclusive.
    return false;
}
