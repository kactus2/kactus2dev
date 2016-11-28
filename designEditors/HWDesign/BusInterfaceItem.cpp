//-----------------------------------------------------------------------------
// File: diagraminterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.3.2011
//
// Description:
// Diagram interface graphics item.
//-----------------------------------------------------------------------------

#include "BusInterfaceItem.h"

#include "InterfaceGraphics.h"

#include "OffPageConnectorItem.h"
#include "AdHocInterfaceItem.h"
#include "HWMoveCommands.h"
#include "HWDesignDiagram.h"

#include "columnview/HWColumn.h"

#include <IPXACTmodels/common/DirectionTypes.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <IPXACTmodels/kactusExtensions/InterfaceGraphicsData.h>

#include <common/IEditProvider.h>

#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/graphicsItems/CommonGraphicsUndoCommands.h>

#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/DesignDiagram.h>

#include <QBrush>
#include <QPen>
#include <QColor>
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>
#include <QCoreApplication>
#include <QGraphicsScene>
#include <QPushButton>

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::BusInterfaceItem()
//-----------------------------------------------------------------------------
BusInterfaceItem::BusInterfaceItem(QSharedPointer<Component> component,
                                   QSharedPointer<BusInterface> busIf,
                                   QSharedPointer<InterfaceGraphicsData> dataGroup, QGraphicsItem *parent):
HWConnectionEndpoint(parent, QVector2D(1.0f, 0.0f)),
    nameLabel_("", this),
    busInterface_(busIf),
    component_(component),
    dataGroup_(dataGroup),
    oldColumn_(0),
    oldPos_(),
    oldInterfacePositions_(),
    offPageConnector_()
{
    setTemporary(busIf == 0);

    setType(ENDPOINT_TYPE_BUS);
    setTypeLocked(busIf != 0 && busIf->getInterfaceMode() != General::INTERFACE_MODE_COUNT);

    setPolygon(arrowDown());
    
    dataGroup_->setName(busInterface_->name());

    if (dataGroup_->hasPosition())
    {
        setPos(dataGroup_->getPosition());
    }

    if (dataGroup_->hasDirection())
    {
        setDirection(dataGroup_->getDirection());
    }
    else
    {
        dataGroup_->setDirection(getDirection());
    }

	QFont font = nameLabel_.font();
    font.setPointSize(8);
    nameLabel_.setFont(font);
    nameLabel_.setRotation(-rotation());
    nameLabel_.setFlag(ItemStacksBehindParent);
	
	QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
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
// Function: BusInterfaceItem::~BusInterfaceItem()
//-----------------------------------------------------------------------------
BusInterfaceItem::~BusInterfaceItem()
{

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::updateInterface()
//-----------------------------------------------------------------------------
void BusInterfaceItem::updateInterface()
{
    Q_ASSERT(busInterface_);

    HWConnectionEndpoint::updateInterface();

    // Determine the bus direction.
    DirectionTypes::Direction direction = InterfaceGraphics::getInterfaceDirection(busInterface_, 
        getOwnerComponent());

    // Update the polygon shape based on the direction.
    QPolygonF shape;
    if (direction == DirectionTypes::IN)
    {
        shape = arrowUp();
    }
    else if (direction == DirectionTypes::OUT)
    {
        shape = arrowDown();
    }    
    else
    {
        shape = doubleArrow();
    }
    setPolygon(shape);

    nameLabel_.setHtml("<div style=\"background-color:#eeeeee; padding:10px 10px;\">" + name() + "</div>");
    setLabelPosition();

    offPageConnector_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::define()
//-----------------------------------------------------------------------------
void BusInterfaceItem::define(QSharedPointer<BusInterface> busIf)
{
    // Add the bus interface to the component.
    component_->getBusInterfaces()->append(busIf);
    busInterface_ = busIf;

    // Update the interface visuals.
    updateInterface();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::getPorts()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Port> > BusInterfaceItem::getPorts() const
{
    Q_ASSERT(busInterface_ != 0);
    QList<QSharedPointer<Port> > ports;

    if (busInterface_->getPortMaps())
    {
        foreach (QSharedPointer<PortMap> portMap, *busInterface_->getPortMaps())
        {
            QSharedPointer<Port> port = component_->getPort(portMap->getPhysicalPort()->name_);
            if (port)
            {
                ports.append(port);
            }
        }
    }

    return ports;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::name()
//-----------------------------------------------------------------------------
QString BusInterfaceItem::name() const
{
    return busInterface_->name();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::setName()
//-----------------------------------------------------------------------------
void BusInterfaceItem::setName(QString const& name)
{
    beginUpdateConnectionNames();

	busInterface_->setName(name);
    
    dataGroup_->setName(name);

    updateInterface();
    emit contentChanged();

    endUpdateConnectionNames();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::description()
//-----------------------------------------------------------------------------
QString BusInterfaceItem::description() const
{
    Q_ASSERT(busInterface_);
    return busInterface_->description();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::setDescription()
//-----------------------------------------------------------------------------
void BusInterfaceItem::setDescription(QString const& description)
{
    Q_ASSERT(busInterface_);
    busInterface_->setDescription(description);

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::onConnect()
//-----------------------------------------------------------------------------
bool BusInterfaceItem::onConnect(ConnectionEndpoint const* other)
{
    if (static_cast<HWDesignDiagram*>(scene())->isLoading())
    {
        return true;
    }

    updateInterface();

    return other != 0;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::onDisonnect()
//-----------------------------------------------------------------------------
void BusInterfaceItem::onDisconnect(ConnectionEndpoint const*)
{
    // Check if there is still some connections left, the bus interface is not defined
    // or the interface is typed.
    if (!getConnections().empty() ||
        busInterface_->getInterfaceMode() == General::INTERFACE_MODE_COUNT || isTypeLocked())
    {
        // Don't do anything.
        return;
    }

    // Update the interface visuals.
    updateInterface();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::isConnectionValid()
//-----------------------------------------------------------------------------
bool BusInterfaceItem::isConnectionValid(ConnectionEndpoint const* other) const
{
    if (!HWConnectionEndpoint::isConnectionValid(other) || !other->isBus() || other->getBusInterface() == 0)
    {
        return false;
    }

    QSharedPointer<BusInterface> otherBusIf = other->getBusInterface();

    // Connection is allowed if this interface's bus interface is not defined,
    // the other end point is unpackaged, or the abstraction definitions of the bus interfaces
    // in each end point are equal.
    return (busInterface_->getInterfaceMode() == General::INTERFACE_MODE_COUNT ||
        !otherBusIf->getBusType().isValid() ||
        (otherBusIf->getBusType() == busInterface_->getBusType() &&
        *otherBusIf->getAbstractionTypes()->first()->getAbstractionRef() == 
        *busInterface_->getAbstractionTypes()->first()->getAbstractionRef()));
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::isExclusive()
//-----------------------------------------------------------------------------
bool BusInterfaceItem::isExclusive() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::encompassingComp()
//-----------------------------------------------------------------------------
ComponentItem* BusInterfaceItem::encompassingComp() const
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::getOwnerComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> BusInterfaceItem::getOwnerComponent() const
{
    Q_ASSERT(component_);
    return component_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::getBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> BusInterfaceItem::getBusInterface() const
{
    return busInterface_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::getPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> BusInterfaceItem::getPort() const
{
    return QSharedPointer<Port>();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::isHierarchical()
//-----------------------------------------------------------------------------
bool BusInterfaceItem::isHierarchical() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::isBus()
//-----------------------------------------------------------------------------
bool BusInterfaceItem::isBus() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::setDirection()
//-----------------------------------------------------------------------------
void BusInterfaceItem::setDirection(QVector2D const& dir)
{
    HWConnectionEndpoint::setDirection(dir);

    dataGroup_->setDirection(dir);

    setLabelPosition();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::setInterfaceMode()
//-----------------------------------------------------------------------------
void BusInterfaceItem::setInterfaceMode(General::InterfaceMode mode)
{
    Q_ASSERT(busInterface_);
    busInterface_->setInterfaceMode(mode);
    emit contentChanged();
    updateInterface();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::getInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode BusInterfaceItem::getInterfaceMode() const
{
    return busInterface_->getInterfaceMode();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::getOffPageConnector()
//-----------------------------------------------------------------------------
ConnectionEndpoint* BusInterfaceItem::getOffPageConnector()
{
    return offPageConnector_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::setLabelPosition()
//-----------------------------------------------------------------------------
void BusInterfaceItem::setLabelPosition()
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
// Function: BusInterfaceItem::getDataExtension()
//-----------------------------------------------------------------------------
QSharedPointer<VendorExtension> BusInterfaceItem::getDataExtension() const
{
    return dataGroup_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::itemChange()
//-----------------------------------------------------------------------------
QVariant BusInterfaceItem::itemChange(GraphicsItemChange change, QVariant const& value)
{
    if (change == ItemPositionChange)
    {
        return snapPointToGrid(value.toPointF());
    }
    else if (change == ItemRotationHasChanged)
    {
        nameLabel_.setRotation(-rotation());

        dataGroup_->setDirection(getDirection());
    }
    else if (change == ItemScenePositionHasChanged)
    {   
        dataGroup_->setPosition(value.toPointF());

        foreach (GraphicsConnection* interconnection, getConnections())
        {
            interconnection->updatePosition();
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::mousePressEvent()
//-----------------------------------------------------------------------------
void BusInterfaceItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    HWConnectionEndpoint::mousePressEvent(event);
    setZValue(1001.0);

    oldColumn_ = dynamic_cast<HWColumn*>(parentItem());
    oldPos_ = scenePos();
    Q_ASSERT(oldColumn_ != 0);

    foreach (QGraphicsItem* item, scene()->items()) //column->childItems())
    {
        if (item->type() == BusInterfaceItem::Type || item->type() == AdHocInterfaceItem::Type)
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
// Function: BusInterfaceItem::mouseMoveEvent()
//-----------------------------------------------------------------------------
void BusInterfaceItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Discard mouse move if the diagram is protected.
    if (static_cast<DesignDiagram*>(scene())->isProtected())
    {
        return;
    }

    HWConnectionEndpoint::mouseMoveEvent(event);

    setPos(parentItem()->mapFromScene(oldColumn_->mapToScene(pos())));

    HWColumn* column = dynamic_cast<HWColumn*>(parentItem());
    Q_ASSERT(column != 0);
    column->onMoveItem(this);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void BusInterfaceItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    HWConnectionEndpoint::mouseReleaseEvent(event);
    setZValue(0.0);

    if (oldColumn_ != 0)
    {
        HWColumn* column = dynamic_cast<HWColumn*>(parentItem());
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

            ++cur;
        }

        oldInterfacePositions_.clear();

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
// Function: BusInterfaceItem::arrowUp()
//-----------------------------------------------------------------------------
QPolygonF BusInterfaceItem::arrowUp() const
{
    int squareSize = GridSize;
    
    /*  /\
     *  ||
     */
    QPolygonF shape;
    shape << QPointF(-squareSize/2, squareSize)
        << QPointF(-squareSize/2, -squareSize / 2)
        << QPointF(0, -squareSize)
        << QPointF(squareSize/2, -squareSize / 2)
        << QPointF(squareSize/2, squareSize);

    return shape;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::arrowDown()
//-----------------------------------------------------------------------------
QPolygonF BusInterfaceItem::arrowDown() const
{
    int squareSize = GridSize;

    /*  ||
     *  \/
     */
    QPolygonF shape;
    shape << QPointF(-squareSize/2, squareSize / 2)
        << QPointF(-squareSize/2, -squareSize)
        << QPointF(squareSize/2, -squareSize)
        << QPointF(squareSize/2, squareSize / 2)
        << QPointF(0, squareSize);

    return shape;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::doubleArrow()
//-----------------------------------------------------------------------------
QPolygonF BusInterfaceItem::doubleArrow() const
{
    int squareSize = GridSize;

    QPolygonF shape;
    /*  /\
     *  ||
     *  \/
     */
    shape << QPointF(-squareSize/2, squareSize / 2)
        << QPointF(-squareSize/2, -squareSize / 2)
        << QPointF(0, -squareSize)
        << QPointF(squareSize/2, -squareSize / 2)
        << QPointF(squareSize/2, squareSize / 2)
        << QPointF(0, squareSize);

    return shape;
}