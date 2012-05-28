//-----------------------------------------------------------------------------
// File: DiagramAdHocInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.2.2012
//
// Description:
// Diagram graphics item for ad-hoc ports.
//-----------------------------------------------------------------------------

#include "DiagramAdHocInterface.h"

#include "diagramcomponent.h"
#include "diagraminterconnection.h"
#include "BusInterfaceDialog.h"
#include "DiagramMoveCommands.h"
#include "blockdiagram.h"
#include "DiagramOffPageConnector.h"

#include "columnview/DiagramColumn.h"
#include "columnview/DiagramColumnLayout.h"

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
// Function: DiagramAdHocInterface()
//-----------------------------------------------------------------------------
DiagramAdHocInterface::DiagramAdHocInterface(QSharedPointer<Component> component,
                                             Port* port, LibraryInterface* lh,
                                             QGraphicsItem *parent) : DiagramConnectionEndPoint(parent, QVector2D(1.0f, 0.0f)),
                                                                      lh_(lh),
                                                                      nameLabel_(0),
                                                                      port_(port),
                                                                      component_(component),
                                                                      oldColumn_(0),
                                                                      temp_(false), 
                                                                      oldPos_(),
                                                                      oldInterfacePositions_(),
                                                                      offPageConnector_(0)
{
    Q_ASSERT_X(port, "DiagramAdHocInterface constructor",
               "Null port pointer given as parameter");

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
    offPageConnector_ = new DiagramOffPageConnector(this);
    offPageConnector_->setPos(0.0, -GridSize * 3);
    offPageConnector_->setFlag(ItemStacksBehindParent);
    offPageConnector_->setVisible(false);

    updateInterface();
}

//-----------------------------------------------------------------------------
// Function: ~DiagramAdHocInterface()
//-----------------------------------------------------------------------------
DiagramAdHocInterface::~DiagramAdHocInterface()
{
    DiagramColumn* column = dynamic_cast<DiagramColumn*>(parentItem());

    if (column != 0)
    {
        column->removeItem(this);
    }
}

//-----------------------------------------------------------------------------
// Function: setTemporary()
//-----------------------------------------------------------------------------
void DiagramAdHocInterface::setTemporary(bool temp)
{
    temp_ = temp;
}

//-----------------------------------------------------------------------------
// Function: name()
//-----------------------------------------------------------------------------
QString DiagramAdHocInterface::name() const
{
    return port_->getName();
}

//-----------------------------------------------------------------------------
// Function: setName()
//-----------------------------------------------------------------------------
void DiagramAdHocInterface::setName(QString const& name)
{
	port_->setName(name);
	updateInterface();
}

//-----------------------------------------------------------------------------
// Function: getBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> DiagramAdHocInterface::getBusInterface() const
{
    return QSharedPointer<BusInterface>();
}

//-----------------------------------------------------------------------------
// Function: updateInterface()
//-----------------------------------------------------------------------------
void DiagramAdHocInterface::updateInterface()
{
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
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: isHierarchical()
//-----------------------------------------------------------------------------
bool DiagramAdHocInterface::isHierarchical() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: onConnect()
//-----------------------------------------------------------------------------
bool DiagramAdHocInterface::onConnect(DiagramConnectionEndPoint const*)
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: onDisonnect()
//-----------------------------------------------------------------------------
void DiagramAdHocInterface::onDisconnect(DiagramConnectionEndPoint const*)
{
}

//-----------------------------------------------------------------------------
// Function: canConnect()
//-----------------------------------------------------------------------------
bool DiagramAdHocInterface::canConnect(DiagramConnectionEndPoint const* other) const
{
    // Ad-hoc connection is not possible to a bus end point.
    if (other->isBus())
    {
        return false;
    }

    // Two hierarchical end points cannot be connected together.
    if (other->isHierarchical())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: encompassingComp()
//-----------------------------------------------------------------------------
DiagramComponent* DiagramAdHocInterface::encompassingComp() const
{
    return qgraphicsitem_cast<DiagramComponent*>(parentItem());
}

//-----------------------------------------------------------------------------
// Function: ownerComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> DiagramAdHocInterface::ownerComponent() const
{
    return component_;
}

//-----------------------------------------------------------------------------
// Function: itemChange()
//-----------------------------------------------------------------------------
QVariant DiagramAdHocInterface::itemChange(GraphicsItemChange change,
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

            emit contentChanged();
            break;
        }

    case ItemScenePositionHasChanged:

        foreach (DiagramInterconnection *interconnection, getInterconnections()) {
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
bool DiagramAdHocInterface::isDirectionFixed() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void DiagramAdHocInterface::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // Discard mouse move if the diagram is protected.
    if (static_cast<BlockDiagram*>(scene())->isProtected())
    {
        return;
    }

    DiagramConnectionEndPoint::mouseMoveEvent(event);

    setPos(parentItem()->mapFromScene(oldColumn_->mapToScene(pos())));

    DiagramColumn* column = dynamic_cast<DiagramColumn*>(parentItem());
    Q_ASSERT(column != 0);
    column->onMoveItem(this, oldColumn_);
}

//-----------------------------------------------------------------------------
// Function: setTypes()
//-----------------------------------------------------------------------------
void DiagramAdHocInterface::setTypes(VLNV const&, VLNV const&, General::InterfaceMode)
{
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void DiagramAdHocInterface::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    DiagramConnectionEndPoint::mousePressEvent(event);
    setZValue(1001.0);

    oldColumn_ = dynamic_cast<DiagramColumn*>(parentItem());
    oldPos_ = scenePos();
    Q_ASSERT(oldColumn_ != 0);

    // Save the positions of the other diagram interfaces.
    foreach (DiagramColumn* column, oldColumn_->getLayout().getColumns())
    {
        if (column->getContentType() == COLUMN_CONTENT_IO)
        {
            foreach (QGraphicsItem* item, column->childItems())
            {
                if (item->type() == DiagramAdHocInterface::Type)
                {
                    DiagramAdHocInterface* interface = static_cast<DiagramAdHocInterface*>(item);
                    oldInterfacePositions_.insert(interface, interface->scenePos());
                }
            }
        }
    }

    // Begin the position update of the connections.
    foreach (DiagramInterconnection* conn, getInterconnections())
    {
        conn->beginUpdatePosition();
    }
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void DiagramAdHocInterface::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    DiagramConnectionEndPoint::mouseReleaseEvent(event);
    setZValue(0.0);

    if (oldColumn_ != 0)
    {
        DiagramColumn* column = dynamic_cast<DiagramColumn*>(parentItem());
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
        QMap<DiagramConnectionEndPoint*, QPointF>::iterator cur = oldInterfacePositions_.begin();

        while (cur != oldInterfacePositions_.end())
        {
            if (cur.key()->scenePos() != cur.value())
            {
                QUndoCommand* childCmd = new ItemMoveCommand(cur.key(), cur.value(), cmd.data());
            }

            ++cur;
        }

        oldInterfacePositions_.clear();

        // End the position update of the connections.
        foreach (DiagramInterconnection* conn, getInterconnections())
        {
            conn->endUpdatePosition(cmd.data());
        }

        // Add the undo command to the edit stack only if it has changes.
        if (cmd->childCount() > 0 || oldPos_ != scenePos())
        {
            column->getEditProvider().addCommand(cmd, false);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: setDirection()
//-----------------------------------------------------------------------------
void DiagramAdHocInterface::setDirection(QVector2D const& dir)
{
    DiagramConnectionEndPoint::setDirection(dir);

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
void DiagramAdHocInterface::setInterfaceMode(General::InterfaceMode)
{
}

//-----------------------------------------------------------------------------
// Function: description()
//-----------------------------------------------------------------------------
QString DiagramAdHocInterface::description() const
{
	Q_ASSERT(port_);
    return port_->getDescription();
}

//-----------------------------------------------------------------------------
// Function: setDescription()
//-----------------------------------------------------------------------------
void DiagramAdHocInterface::setDescription(QString const& description)
{
	Q_ASSERT(port_);
	port_->setDescription(description);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: getOffPageConnector()
//-----------------------------------------------------------------------------
DiagramConnectionEndPoint* DiagramAdHocInterface::getOffPageConnector()
{
    return offPageConnector_;
}

//-----------------------------------------------------------------------------
// Function: DiagramAdHocInterface::isBus()
//-----------------------------------------------------------------------------
bool DiagramAdHocInterface::isBus() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: DiagramAdHocInterface::getPort()
//-----------------------------------------------------------------------------
Port* DiagramAdHocInterface::getPort() const
{
    return port_;
}
