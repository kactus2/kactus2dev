//-----------------------------------------------------------------------------
// File: DiagramAdHocPort.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.2.2012
//
// Description:
// Diagram graphics item for ad-hoc ports.
//-----------------------------------------------------------------------------

#include "DiagramAdHocPort.h"

#include "diagramcomponent.h"
#include "diagraminterconnection.h"
#include "BusInterfaceDialog.h"
#include "DiagramMoveCommands.h"
#include "blockdiagram.h"
#include "DiagramOffPageConnector.h"

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
// Function: DiagramAdHocPort()
//-----------------------------------------------------------------------------
DiagramAdHocPort::DiagramAdHocPort(QSharedPointer<Port> port, LibraryInterface* lh,
                                   QGraphicsItem *parent) : DiagramConnectionEndPoint(parent),
                                                            port_(port),
                                                            temp_(false), lh_(lh),
                                                            oldPos_(), oldPortPositions_(),
                                                            offPageConnector_(0)
{

    Q_ASSERT_X(port, "DiagramAdHocPort constructor",
        "Null BusInterface pointer given as parameter");

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
                  << QPointF(halfSquareSize, -halfSquareSize)
                  << QPointF(halfSquareSize, halfSquareSize)
                  << QPointF(0, squareSize);
            break;
        }

    case General::OUT:
        {
            shape << QPointF(-halfSquareSize, halfSquareSize)
                  << QPointF(-halfSquareSize, -halfSquareSize)
                  << QPointF(0, -squareSize)
                  << QPointF(halfSquareSize, -halfSquareSize)
                  << QPointF(halfSquareSize, halfSquareSize);
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
    offPageConnector_ = new DiagramOffPageConnector(this);
    offPageConnector_->setPos(0.0, -GridSize * 3);
    offPageConnector_->setFlag(ItemStacksBehindParent);
    offPageConnector_->setVisible(false);

    updateInterface();
}

//-----------------------------------------------------------------------------
// Function: ~DiagramAdHocPort()
//-----------------------------------------------------------------------------
DiagramAdHocPort::~DiagramAdHocPort()
{
}

//-----------------------------------------------------------------------------
// Function: setTemporary()
//-----------------------------------------------------------------------------
void DiagramAdHocPort::setTemporary(bool temp)
{
    temp_ = temp;
}

//-----------------------------------------------------------------------------
// Function: name()
//-----------------------------------------------------------------------------
QString DiagramAdHocPort::name() const
{
    return port_->getName();
}

//-----------------------------------------------------------------------------
// Function: setName()
//-----------------------------------------------------------------------------
void DiagramAdHocPort::setName(QString const& name)
{
	port_->setName(name);
	updateInterface();
}

//-----------------------------------------------------------------------------
// Function: getBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> DiagramAdHocPort::getBusInterface() const
{
    return QSharedPointer<BusInterface>();
}

//-----------------------------------------------------------------------------
// Function: updateInterface()
//-----------------------------------------------------------------------------
void DiagramAdHocPort::updateInterface()
{
    setBrush(QBrush(Qt::black));

    nameLabel_->setHtml("<div style=\"background-color:#eeeeee; padding:10px 10px;\">" +
                        port_->getName().left(15) + "</div>");

    qreal nameWidth = nameLabel_->boundingRect().width();
    qreal nameHeight = nameLabel_->boundingRect().height();

    if (pos().x() < 0)
    {
        nameLabel_->setPos(nameHeight/2, GridSize/2);
    }
    else
    {
        nameLabel_->setPos(-nameHeight/2, GridSize/2 + nameWidth);
    }

    offPageConnector_->updateInterface();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: isHierarchical()
//-----------------------------------------------------------------------------
bool DiagramAdHocPort::isHierarchical() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: onConnect()
//-----------------------------------------------------------------------------
bool DiagramAdHocPort::onConnect(DiagramConnectionEndPoint const* other)
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: onDisonnect()
//-----------------------------------------------------------------------------
void DiagramAdHocPort::onDisconnect(DiagramConnectionEndPoint const*)
{
}

//-----------------------------------------------------------------------------
// Function: canConnect()
//-----------------------------------------------------------------------------
bool DiagramAdHocPort::canConnect(DiagramConnectionEndPoint const* other) const
{
    // Ad-hoc connection is not possible to a bus end point.
    if (other->isBus())
    {
        return false;
    }

    return (port_->getDirection() == General::INOUT ||
            other->getPort()->getDirection() == General::INOUT ||
            port_->getDirection() == General::IN && other->getPort()->getDirection() == General::OUT ||
            port_->getDirection() == General::OUT && other->getPort()->getDirection() == General::IN);
}

//-----------------------------------------------------------------------------
// Function: encompassingComp()
//-----------------------------------------------------------------------------
DiagramComponent* DiagramAdHocPort::encompassingComp() const
{
    return qgraphicsitem_cast<DiagramComponent*>(parentItem());
}

//-----------------------------------------------------------------------------
// Function: ownerComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> DiagramAdHocPort::ownerComponent() const
{
	DiagramComponent* comp = encompassingComp();
	Q_ASSERT(comp);
	QSharedPointer<Component> compModel = comp->componentModel();
	Q_ASSERT(compModel);
	return compModel;
}

//-----------------------------------------------------------------------------
// Function: itemChange()
//-----------------------------------------------------------------------------
QVariant DiagramAdHocPort::itemChange(GraphicsItemChange change,
                                 const QVariant &value)
{
    switch (change) {

    case ItemPositionChange:
        {
            if (!parentItem())
            {
                return snapPointToGrid(value.toPointF());
            }

            QPointF pos = value.toPointF();
            QRectF parentRect = qgraphicsitem_cast<DiagramComponent *>(parentItem())->rect();

            if (pos.x() < 0)
            {
                pos.setX(parentRect.left());
            }
            else
            {
                pos.setX(parentRect.right());
            }

            return snapPointToGrid(pos);
        }

    case ItemPositionHasChanged:
        {
            if (!parentItem())
                break;

            qreal nameWidth = nameLabel_->boundingRect().width();
            qreal nameHeight = nameLabel_->boundingRect().height();

            QRectF parentRect = qgraphicsitem_cast<DiagramComponent*>(parentItem())->rect();

            // Check if the port is directed to the left.
            if (pos().x() < 0)
            {
                setDirection(QVector2D(-1.0f, 0.0f));
                nameLabel_->setPos(nameHeight/2, GridSize/2);
            }
            // Otherwise the port is directed to the right.
            else
            {
                setDirection(QVector2D(1.0f, 0.0f));
                nameLabel_->setPos(-nameHeight/2, GridSize/2 + nameWidth);
            }

            emit contentChanged();
            break;
        }

    case ItemScenePositionHasChanged:
        // Check if the updates are not disabled.
        if (!static_cast<DiagramComponent*>(parentItem())->isConnectionUpdateDisabled())
        {
            // Update the connections.
            foreach (DiagramInterconnection *interconnection, getInterconnections())
            {
                interconnection->updatePosition();
            }
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
bool DiagramAdHocPort::isDirectionFixed() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void DiagramAdHocPort::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // Discard mouse move if the diagram is protected.
    if (static_cast<BlockDiagram*>(scene())->isProtected())
    {
        return;
    }

    DiagramConnectionEndPoint::mouseMoveEvent(event);
    encompassingComp()->onMovePort(this);
}

//-----------------------------------------------------------------------------
// Function: setTypes()
//-----------------------------------------------------------------------------
void DiagramAdHocPort::setTypes(VLNV const& busType, VLNV const& absType, General::InterfaceMode mode)
{
    // TODO: Not useful for ad-hoc ports at all (?)
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void DiagramAdHocPort::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    DiagramConnectionEndPoint::mousePressEvent(event);    
    oldPos_ = pos();

    // Save old port positions for all ports in the parent component.
    foreach (QGraphicsItem* item, parentItem()->childItems())
    {
        if (dynamic_cast<DiagramConnectionEndPoint*>(item) != 0 && item != this)
        {
            DiagramConnectionEndPoint* port = static_cast<DiagramConnectionEndPoint*>(item);
            oldPortPositions_.insert(port, port->pos());
        }
    }

    // Begin the position update for the connections.
    foreach (DiagramInterconnection* conn, getInterconnections())
    {
        conn->beginUpdatePosition();
    }
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void DiagramAdHocPort::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    DiagramConnectionEndPoint::mouseReleaseEvent(event);

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
    QMap<DiagramConnectionEndPoint*, QPointF>::iterator cur = oldPortPositions_.begin();

    while (cur != oldPortPositions_.end())
    {
        if (cur.key()->pos() != cur.value())
        {
            QUndoCommand* childCmd = new PortMoveCommand(cur.key(), cur.value(), cmd.data());
        }

        ++cur;
    }

    oldPortPositions_.clear();
    
    // End the position update of the connections.
    foreach (DiagramInterconnection* conn, getInterconnections())
    {
        conn->endUpdatePosition(cmd.data());
    }

    // Add the undo command to the edit stack only if it has changes.
    if (cmd->childCount() > 0 || oldPos_ != pos())
    {
        static_cast<BlockDiagram*>(scene())->getEditProvider().addCommand(cmd, false);
    }
}

//-----------------------------------------------------------------------------
// Function: setInterfaceMode()
//-----------------------------------------------------------------------------
void DiagramAdHocPort::setInterfaceMode(General::InterfaceMode mode)
{
	Q_ASSERT(port_);
    // TODO:
	updateInterface();
}

//-----------------------------------------------------------------------------
// Function: description()
//-----------------------------------------------------------------------------
QString DiagramAdHocPort::description() const
{
	Q_ASSERT(port_);
    return port_->getDescription();
}

//-----------------------------------------------------------------------------
// Function: setDescription()
//-----------------------------------------------------------------------------
void DiagramAdHocPort::setDescription(QString const& description)
{
	Q_ASSERT(port_);
	port_->setDescription(description);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: getOffPageConnector()
//-----------------------------------------------------------------------------
DiagramConnectionEndPoint* DiagramAdHocPort::getOffPageConnector()
{
    return offPageConnector_;
}

//-----------------------------------------------------------------------------
// Function: DiagramAdHocPort::isBus()
//-----------------------------------------------------------------------------
bool DiagramAdHocPort::isBus() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: DiagramAdHocPort::getPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> DiagramAdHocPort::getPort() const
{
    return port_;
}
