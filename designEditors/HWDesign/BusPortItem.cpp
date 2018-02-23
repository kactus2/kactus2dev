//-----------------------------------------------------------------------------
// File: BusPortItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: 
// Date: 
//
// Description:
// HWConnection represents graphically an IP-XACT bus interface in a component instance.
//-----------------------------------------------------------------------------

#include "BusPortItem.h"

#include "HWComponentItem.h"
#include "HWMoveCommands.h"
#include "HWDesignDiagram.h"

#include <common/IEditProvider.h>
#include <common/graphicsItems/GraphicsConnection.h>

#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/NamelabelWidth.h>

#include <library/LibraryInterface.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/ComponentInstance.h>
#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <QFont>
#include <QVector2D>
#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: BusPortItem::BusPortItem()
//-----------------------------------------------------------------------------
BusPortItem::BusPortItem(QSharedPointer<BusInterface> busIf, LibraryInterface* library, HWComponentItem* parent):
BusInterfaceEndPoint(busIf, parent),
parentComponentItem_(parent),
library_(library)
{
    Q_ASSERT_X(busIf, "BusPortItem constructor", "Null BusInterface pointer given as parameter");

    updateInterface();
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::~BusPortItem()
//-----------------------------------------------------------------------------
BusPortItem::~BusPortItem()
{
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::updateName()
//-----------------------------------------------------------------------------
void BusPortItem::updateName(QString const& previousName, QString const& newName)
{
    if (previousName.compare(newName) != 0)
    {
        encompassingComp()->getComponentInstance()->removeBusInterfacePosition(previousName);
        encompassingComp()->getComponentInstance()->updateBusInterfacePosition(newName, pos());
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::onConnect()
//-----------------------------------------------------------------------------
bool BusPortItem::onConnect(ConnectionEndpoint const* other)
{
    return other != 0;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::onDisconnect()
//-----------------------------------------------------------------------------
void BusPortItem::onDisconnect(ConnectionEndpoint const*)
{

}

//-----------------------------------------------------------------------------
// Function: isConnectionValid()
//-----------------------------------------------------------------------------
bool BusPortItem::isConnectionValid(ConnectionEndpoint const* other) const
{
    if (!HWConnectionEndpoint::isConnectionValid(other) || !other->isBus())
    {
        return false;
    }

    QSharedPointer<BusInterface> otherBusIf = other->getBusInterface();

    // If the other end point is hierarchical, we can connect to it if the interface mode
    // is the same or either of the end point is undefined.
    if (other->isHierarchical())
    {
        return (otherBusIf->getInterfaceMode() == getBusInterface()->getInterfaceMode() ||
            other->getBusInterface()->getInterfaceMode() == General::INTERFACE_MODE_COUNT ||
            !getBusInterface()->getBusType().isValid());
    }

    // If only one port has a bus definition defined at most, then the end points can be connected.
    else if (!(getBusInterface()->getBusType().isValid() && otherBusIf->getBusType().isValid()))
    {
        return true;
    }

    // Otherwise make sure that the bus and abstraction definitions are of the same type.
    else if (otherBusIf->getBusType() == getBusInterface()->getBusType())
    {
        QList<General::InterfaceMode> compatibleModes = getOpposingModes(getBusInterface());
        compatibleModes.append(General::SYSTEM);

        General::InterfaceMode otherMode = otherBusIf->getInterfaceMode();

        return getBusInterface()->getInterfaceMode() != General::MONITOR && compatibleModes.contains(otherMode);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::encompassingComp()
//-----------------------------------------------------------------------------
ComponentItem* BusPortItem::encompassingComp() const
{
    return parentComponentItem_;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::getOwnerComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> BusPortItem::getOwnerComponent() const
{
    ComponentItem* comp = encompassingComp();

    QSharedPointer<Component> compModel;
    if (comp)
    {
        compModel = comp->componentModel();
    }

    Q_ASSERT(compModel);
    return compModel;
}

//-----------------------------------------------------------------------------
// Function: isHierarchical()
//-----------------------------------------------------------------------------
bool BusPortItem::isHierarchical() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::setTemporary()
//-----------------------------------------------------------------------------
void BusPortItem::setTemporary(bool temp)
{
    setTypeLocked(!temp);
    HWConnectionEndpoint::setTemporary(temp);
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::isDirectionFixed()
//-----------------------------------------------------------------------------
bool BusPortItem::isDirectionFixed() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::setLabelPosition()
//-----------------------------------------------------------------------------
void BusPortItem::setLabelPosition()
{
    qreal nameWidth = getNameLabel()->boundingRect().width();
    qreal nameHeight = getNameLabel()->boundingRect().height();

    // Check if the port is directed to the left.
    if (pos().x() < 0)
    {
        getNameLabel()->setPos(nameHeight/2, GridSize);
    }
    else // The port is directed to the right.
    {
        getNameLabel()->setPos(-nameHeight/2, GridSize + nameWidth);
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::checkDirection()
//-----------------------------------------------------------------------------
void BusPortItem::checkDirection()
{
    // Check if the port is directed to the left
    if (pos().x() < 0)
    {
        setDirection(QVector2D(-1.0f, 0.0f));
    }
    else // The port is directed to the right.
    {
        setDirection(QVector2D(1.0f, 0.0f));
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::getNameLength()
//-----------------------------------------------------------------------------
qreal BusPortItem::getNameLength()
{
    QFont font = getNameLabel()->font();
    return NamelabelWidth::getTextLength(name(), font);
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::shortenNameLabel()
//-----------------------------------------------------------------------------
void BusPortItem::shortenNameLabel(qreal width)
{
    QString nameLabelText = NamelabelWidth::setNameLabel(name(), getNameLabel()->font(), width);

    getNameLabel()->setHtml("<div style=\"background-color:#eeeeee; padding:10px 10px;\">" + nameLabelText + "</div>");
    setLabelPosition();
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::itemChange()
//-----------------------------------------------------------------------------
QVariant BusPortItem::itemChange(GraphicsItemChange change, QVariant const& value)
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
        getNameLabel()->setRotation(-rotation());
    }
    else if (change == ItemScenePositionHasChanged)
    {
        foreach (GraphicsConnection* interconnection, getConnections())
        {
            interconnection->updatePosition();
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::mousePressEvent()
//-----------------------------------------------------------------------------
void BusPortItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    HWConnectionEndpoint::mousePressEvent(event);  

    DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(scene());
    if (diagram != 0 && diagram->isProtected())
    {
        return;
    }

    setOldPosition(pos());

    saveOldPortPositions(parentItem()->childItems());

    beginUpdateConnectionPositions();
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::mouseMoveEvent()
//-----------------------------------------------------------------------------
void BusPortItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!sceneIsLocked())
    {
        BusInterfaceEndPoint::mouseMoveEvent(event);

        parentComponentItem_->onMovePort(this);
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void BusPortItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    HWConnectionEndpoint::mouseReleaseEvent(event);

    DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(scene());
    if (diagram == 0 || diagram->isProtected())
    {
        return;
    }

    QSharedPointer<QUndoCommand> moveUndoCommand;

    // Check if the port position was really changed.
    if (getOldPosition() != pos())
    {
        moveUndoCommand = QSharedPointer<QUndoCommand>(new PortMoveCommand(this, getOldPosition(), diagram));
    }
    else
    {
        moveUndoCommand = QSharedPointer<QUndoCommand>(new QUndoCommand());
    }

    // Determine if the other ports changed their position and create undo commands for them.
    QMap<ConnectionEndpoint*, QPointF> oldPortPositions = getOldPortPositions();

    QMap<ConnectionEndpoint*, QPointF>::iterator cur = oldPortPositions.begin();
    while (cur != oldPortPositions.end())
    {
        if (cur.key()->pos() != cur.value())
        {
            new PortMoveCommand(
                static_cast<HWConnectionEndpoint*>(cur.key()), cur.value(), diagram, moveUndoCommand.data());
        }

        cur++;
    }
    clearOldPortPositions();

    foreach (QGraphicsItem *item, scene()->items())
    {
        GraphicsConnection* conn = dynamic_cast<GraphicsConnection*>(item);
        if (conn != 0)
        {
            conn->endUpdatePosition(moveUndoCommand.data());
        }
    }

    // Add the undo command to the edit stack only if it has changes.
    if (moveUndoCommand->childCount() > 0 || getOldPosition() != pos())
    {
        static_cast<DesignDiagram*>(scene())->getEditProvider()->addCommand(moveUndoCommand);
        moveUndoCommand->redo();
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::getOpposingModes()
//-----------------------------------------------------------------------------
QList<General::InterfaceMode> BusPortItem::getOpposingModes(QSharedPointer<BusInterface> busIf) const
{
    QList<General::InterfaceMode> possibleModes;

    General::InterfaceMode sourceMode = busIf->getInterfaceMode();

    if (sourceMode == General::INTERFACE_MODE_COUNT)
    {
        return possibleModes;
    }

    possibleModes.append(General::getCompatibleInterfaceMode(sourceMode));

    if (sourceMode == General::MASTER)
    {
        QSharedPointer<BusDefinition const> busDef = 
            library_->getModelReadOnly(busIf->getBusType()).dynamicCast<BusDefinition const>();
        if (busDef != 0 && busDef->getDirectConnection())
        {
            possibleModes.append(General::SLAVE);      
        }
    }
    else if (sourceMode == General::SLAVE)
    {
        QSharedPointer<BusDefinition const> busDef = 
            library_->getModelReadOnly(busIf->getBusType()).dynamicCast<BusDefinition const>();
        if (busDef != 0 && busDef->getDirectConnection())
        {
            possibleModes.append(General::MASTER);      
        }
    }

    return possibleModes;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::getDirectionOutShape()
//-----------------------------------------------------------------------------
QPolygonF BusPortItem::getDirectionOutShape() const
{
    int squareSize = GridSize;
    
    /*  /\
     *  ||
     */
    QPolygonF shape;
    shape << QPointF(-squareSize/2, squareSize/2)
        << QPointF(-squareSize/2, 0)
        << QPointF(0, -squareSize/2)
        << QPointF(squareSize/2, 0)
        << QPointF(squareSize/2, squareSize/2);

    return shape;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::getDirectionInShape()
//-----------------------------------------------------------------------------
QPolygonF BusPortItem::getDirectionInShape() const
{
    int squareSize = GridSize;

    /*  ||
     *  \/
     */
    QPolygonF shape;
    shape << QPointF(-squareSize/2, 0)
        << QPointF(-squareSize/2, -squareSize/2)
        << QPointF(squareSize/2, -squareSize/2)
        << QPointF(squareSize/2, 0)
        << QPointF(0, squareSize/2);

    return shape;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::getDirectionInOutShape()
//-----------------------------------------------------------------------------
QPolygonF BusPortItem::getDirectionInOutShape() const
{
    int squareSize = GridSize;

    QPolygonF shape;
    /*  /\
     *  ||
     *  \/
     */
    shape << QPointF(-squareSize/2, squareSize/2)
        << QPointF(-squareSize/2, 0)
        << QPointF(0, -squareSize/2)
        << QPointF(squareSize/2, 0)
        << QPointF(squareSize/2, squareSize/2)
        << QPointF(0, squareSize);

    return shape;
}
