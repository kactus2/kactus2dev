//-----------------------------------------------------------------------------
// File: BusPortItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 
//
// Description:
// HWConnection represents graphically an IP-XACT bus interface in a component instance.
//-----------------------------------------------------------------------------

#include "BusPortItem.h"

#include "InterfaceGraphics.h"

#include "HWComponentItem.h"
#include "HWMoveCommands.h"
#include "HWDesignDiagram.h"
#include "OffPageConnectorItem.h"

#include <common/IEditProvider.h>

#include <common/graphicsItems/GraphicsConnection.h>

#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/NamelabelWidth.h>

#include <IPXACTmodels/common/DirectionTypes.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/Design/ComponentInstance.h>

#include <library/LibraryManager/libraryinterface.h>

#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QVector2D>
#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: BusPortItem::BusPortItem()
//-----------------------------------------------------------------------------
BusPortItem::BusPortItem(QSharedPointer<BusInterface> busIf, LibraryInterface* library, HWComponentItem* parent):
HWConnectionEndpoint(parent),
    busInterface_(busIf),
    parentComponentItem_(parent),
    library_(library),
    nameLabel_("",this),
    oldPos_(), 
    oldPortPositions_(),
    offPageConnector_()
{
    Q_ASSERT_X(busIf, "BusPortItem constructor", "Null BusInterface pointer given as parameter");

    setType(ENDPOINT_TYPE_BUS);

	QFont font = nameLabel_.font();
    font.setPointSize(8);
	nameLabel_.setFont(font);
	nameLabel_.setFlag(ItemStacksBehindParent);

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect;
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
// Function: BusPortItem::~BusPortItem()
//-----------------------------------------------------------------------------
BusPortItem::~BusPortItem()
{
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::updateInterface()
//-----------------------------------------------------------------------------
void BusPortItem::updateInterface()
{
    HWConnectionEndpoint::updateInterface(); 

    // Determine the bus direction.
    DirectionTypes::Direction dir = InterfaceGraphics::getInterfaceDirection(busInterface_, getOwnerComponent());

    // Update the polygon shape based on the direction.
    QPolygonF shape;
    if (dir == DirectionTypes::IN)
    {
        shape = arrowDown();
    }
    else if (dir == DirectionTypes::OUT)
    {
        shape = arrowUp();
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
// Function: BusPortItem::name()
//-----------------------------------------------------------------------------
QString BusPortItem::name() const
{
    return busInterface_->name();
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::setName()
//-----------------------------------------------------------------------------
void BusPortItem::setName(QString const& name)
{
    beginUpdateConnectionNames();

    QString previousName = busInterface_->name();

    busInterface_->setName(name);

    if (previousName.compare(name) != 0)
    {
        encompassingComp()->getComponentInstance()->removeBusInterfacePosition(previousName);
        encompassingComp()->getComponentInstance()->updateBusInterfacePosition(name, pos());
    }

	updateInterface();
    emit contentChanged();

    endUpdateConnectionNames();
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::description()
//-----------------------------------------------------------------------------
QString BusPortItem::description() const
{
    Q_ASSERT(busInterface_);
    return busInterface_->description();
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::setDescription()
//-----------------------------------------------------------------------------
void BusPortItem::setDescription( QString const& description )
{
    Q_ASSERT(busInterface_);
    busInterface_->setDescription(description);
    emit contentChanged();
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
        return (otherBusIf->getInterfaceMode() == busInterface_->getInterfaceMode() ||
            other->getBusInterface()->getInterfaceMode() == General::INTERFACE_MODE_COUNT ||
            !busInterface_->getBusType().isValid());
    }

    // If only one port has a bus definition defined at most, then the end points can be connected.
    else if (!(busInterface_->getBusType().isValid() && otherBusIf->getBusType().isValid()))
    {
        return true;
    }

    // Otherwise make sure that the bus and abstraction definitions are of the same type.
    else if (otherBusIf->getBusType() == busInterface_->getBusType() &&
        *otherBusIf->getAbstractionTypes()->first()->getAbstractionRef() ==
        *busInterface_->getAbstractionTypes()->first()->getAbstractionRef())
    {
        QList<General::InterfaceMode> compatibleModes = getOpposingModes(busInterface_);
        compatibleModes.append(General::SYSTEM);

        General::InterfaceMode otherMode = otherBusIf->getInterfaceMode();

        return busInterface_->getInterfaceMode() != General::MONITOR && compatibleModes.contains(otherMode);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::isExclusive()
//-----------------------------------------------------------------------------
bool BusPortItem::isExclusive() const
{
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
    Q_ASSERT(comp);

    QSharedPointer<Component> compModel = comp->componentModel();
    Q_ASSERT(compModel);

    return compModel;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::getBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> BusPortItem::getBusInterface() const
{
    return busInterface_;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::getPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> BusPortItem::getPort() const
{
    return QSharedPointer<Port>();
}

//-----------------------------------------------------------------------------
// Function: isHierarchical()
//-----------------------------------------------------------------------------
bool BusPortItem::isHierarchical() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::isBus()
//-----------------------------------------------------------------------------
bool BusPortItem::isBus() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::setInterfaceMode()
//-----------------------------------------------------------------------------
void BusPortItem::setInterfaceMode(General::InterfaceMode mode)
{
    Q_ASSERT(busInterface_);
    busInterface_->setInterfaceMode(mode);
    updateInterface();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::getInterfaceMode()
//-----------------------------------------------------------------------------
General::InterfaceMode BusPortItem::getInterfaceMode() const
{
    return busInterface_->getInterfaceMode();
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::getOffPageConnector()
//-----------------------------------------------------------------------------
ConnectionEndpoint* BusPortItem::getOffPageConnector()
{
    return offPageConnector_;
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
    qreal nameWidth = nameLabel_.boundingRect().width();
    qreal nameHeight = nameLabel_.boundingRect().height();

    // Check if the port is directed to the left.
    if (pos().x() < 0)
    {
        nameLabel_.setPos(nameHeight/2, GridSize);
    }
    else // The port is directed to the right.
    {
        nameLabel_.setPos(-nameHeight/2, GridSize + nameWidth);
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
    QFont font = nameLabel_.font();
    return NamelabelWidth::getTextLength(name(), font);
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::shortenNameLabel()
//-----------------------------------------------------------------------------
void BusPortItem::shortenNameLabel(qreal width)
{
    QString nameLabelText = NamelabelWidth::setNameLabel(name(), nameLabel_.font(), width);

    nameLabel_.setHtml("<div style=\"background-color:#eeeeee; padding:10px 10px;\">" + nameLabelText + "</div>");
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
        nameLabel_.setRotation(-rotation());
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
// Function: BusPortItem::mouseMoveEvent()
//-----------------------------------------------------------------------------
void BusPortItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
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
    if (oldPos_ != pos())
    {
        moveUndoCommand = QSharedPointer<QUndoCommand>(new PortMoveCommand(this, oldPos_));
    }
    else
    {
        moveUndoCommand = QSharedPointer<QUndoCommand>(new QUndoCommand());
    }

    // Determine if the other ports changed their position and create undo commands for them.
    QMap<ConnectionEndpoint*, QPointF>::iterator cur = oldPortPositions_.begin();
    while (cur != oldPortPositions_.end())
    {
        if (cur.key()->pos() != cur.value())
        {
            new PortMoveCommand(static_cast<HWConnectionEndpoint*>(cur.key()), cur.value(), moveUndoCommand.data());
        }

        cur++;
    }
    oldPortPositions_.clear();
    
    foreach (QGraphicsItem *item, scene()->items())
    {
        GraphicsConnection* conn = dynamic_cast<GraphicsConnection*>(item);
        if (conn != 0)
        {
            conn->endUpdatePosition(moveUndoCommand.data());
        }
    }

    // Add the undo command to the edit stack only if it has changes.
    if (moveUndoCommand->childCount() > 0 || oldPos_ != pos())
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
    General::InterfaceMode sourceMode = busIf->getInterfaceMode();

    QList<General::InterfaceMode> possibleModes;

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
// Function: BusPortItem::arrowUp()
//-----------------------------------------------------------------------------
QPolygonF BusPortItem::arrowUp() const
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
// Function: BusPortItem::arrowDown()
//-----------------------------------------------------------------------------
QPolygonF BusPortItem::arrowDown() const
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
// Function: BusPortItem::doubleArrow()
//-----------------------------------------------------------------------------
QPolygonF BusPortItem::doubleArrow() const
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
