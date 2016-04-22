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
#include "HWConnection.h"
#include "BusInterfaceDialog.h"
#include "HWMoveCommands.h"
#include "HWDesignDiagram.h"
#include "OffPageConnectorItem.h"

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/GenericEditProvider.h>
#include <common/KactusColors.h>

#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/NamelabelWidth.h>

#include <IPXACTmodels/common/DirectionTypes.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <library/LibraryManager/libraryinterface.h>

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
    offPageConnector_(), 
    oldName_()
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
// Function: BusPortItem::setTypes()
//-----------------------------------------------------------------------------
void BusPortItem::setTypes(VLNV const& busType, VLNV const& absType, General::InterfaceMode mode)
{
    Q_ASSERT(busInterface_ != 0);

    // If the bus interface was already defined, disconnect the connections before making any updates.
    if (busInterface_->getBusType().isValid())
    {
        foreach(GraphicsConnection* conn, getConnections())
        {
            ConnectionEndpoint* opposingEnd = conn->endpoint1();
            if (opposingEnd == this)
            {
                opposingEnd = conn->endpoint2();
            }

            opposingEnd->removeConnection(conn);
            opposingEnd->onDisconnect(this);
            opposingEnd->addConnection(conn);
        }
    }

    // Initialize the bus interface.
    busInterface_->setBusType(busType);
    
    QSharedPointer<AbstractionType> abstraction(new AbstractionType());
    abstraction->setAbstractionRef(QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(absType)));

    busInterface_->getAbstractionTypes()->clear();
    busInterface_->getAbstractionTypes()->append(abstraction);

    busInterface_->setInterfaceMode(mode);

    // Update the interface visuals.
    setTypeLocked(busType.isValid());
    updateInterface();

    if (busType.isValid())
    {
        // Undefined end points of the connections can now be defined.
        foreach(GraphicsConnection* conn, getConnections())
        {
            ConnectionEndpoint* opposingEnd = conn->endpoint1();
            if (opposingEnd == this)
            {
                opposingEnd = conn->endpoint2();
            }

            opposingEnd->onConnect(this);
            opposingEnd->onConnect(conn->endpoint1());
        }
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::updateInterface()
//-----------------------------------------------------------------------------
void BusPortItem::updateInterface()
{
    HWConnectionEndpoint::updateInterface();

    if (isInvalid())
    {
        setBrush(QBrush(Qt::red));
    }
    else if (busInterface_->getInterfaceMode() == General::MASTER)
    {
        setBrush(QBrush(KactusColors::MASTER_INTERFACE));
    }
    else if (busInterface_->getInterfaceMode() == General::SLAVE)
    {
        setBrush(QBrush(KactusColors::SLAVE_INTERFACE));
    }
    else if (busInterface_->getInterfaceMode() == General::MIRROREDMASTER)
    {
        setBrush(QBrush(KactusColors::MIRROREDMASTER_INTERFACE));
    }
    else if (busInterface_->getInterfaceMode() == General::MIRROREDSLAVE)
    {
        setBrush(QBrush(KactusColors::MIRROREDSLAVE_INTERFACE));
    }
    else if (busInterface_->getInterfaceMode() == General::SYSTEM)
    {
        setBrush(QBrush(KactusColors::SYSTEM_INTERFACE));
    }
    else if (busInterface_->getInterfaceMode() == General::MIRROREDSYSTEM)
    {
        setBrush(QBrush(KactusColors::MIRROREDSYSTEM_INTERFACE));
    }
    else if (busInterface_->getInterfaceMode() == General::MONITOR)
    {
        setBrush(QBrush(KactusColors::MONITOR_INTERFACE));
    }
    else
    {
        setBrush(QBrush(Qt::black));
    }

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

    busInterface_->setName(name);

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
    if (!other)
    {
        return false;
    }

    QSharedPointer<BusInterface> otherBusIf = other->getBusInterface();

    // If the port is a non-typed one, try to copy the configuration from the other end point.
    if (getConnections().empty() && !isTypeLocked())
    {
        oldName_ = busInterface_->name();

        if (otherBusIf != 0 && otherBusIf->getBusType().isValid())
        {
            if (busInterface_->getAbstractionTypes()->isEmpty())
            {
                busInterface_->getAbstractionTypes()->append(QSharedPointer<AbstractionType>(new AbstractionType()));
            }

            QSharedPointer<GenericEditProvider> editProvider = 
                static_cast<DesignDiagram*>(scene())->getEditProvider().dynamicCast<GenericEditProvider>();
            editProvider->setState("portsCopied", false);

            if (!editProvider->isPerformingUndoRedo() && !static_cast<HWDesignDiagram*>(scene())->isLoading())
            {                            
                QList< QSharedPointer<Port> > newPorts;
                QList< QSharedPointer<PortMap> > newPortMaps;

                General::InterfaceMode mode = otherBusIf->getInterfaceMode();
                // Set a compatible interface mode. If the other end point is a hierarchical one,
                // the same interface mode injects automatically. Otherwise the proper interface mode must
                // be determined/asked based on the other bus interface.
                if (!getModeAndPorts(other, mode, newPorts, newPortMaps))
                {
                    return false;
                }

                busInterface_->setInterfaceMode(mode);

                editProvider->setState("portsCopied", true);

                encompassingComp()->componentModel()->getPorts()->append(newPorts);

                busInterface_->getPortMaps()->clear();
                busInterface_->getPortMaps()->append(newPortMaps);
            }

            QString ifName = other->name();
            int count = 0;
            while (getOwnerComponent()->hasInterface(ifName))
            {
                count++;
                ifName = other->name() + "_" + QString::number(count);
            }
            busInterface_->setName(ifName);

            // Copy the bus and abstraction definitions.
            busInterface_->setBusType(otherBusIf->getBusType());

            if (otherBusIf->getAbstractionTypes()->first()->getAbstractionRef())
            {
                QSharedPointer<ConfigurableVLNVReference> abstractionReference(new ConfigurableVLNVReference());
                abstractionReference->setVLNV(*otherBusIf->getAbstractionTypes()->first()->getAbstractionRef());

                busInterface_->getAbstractionTypes()->first()->setAbstractionRef(abstractionReference);
            }

            updateInterface();
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::onDisconnect()
//-----------------------------------------------------------------------------
void BusPortItem::onDisconnect(ConnectionEndpoint const*)
{
    // If the port is a non-typed one, set the bus and abstraction definitions undefined.
    if (getConnections().empty() && !isTypeLocked())
    {
        busInterface_->setBusType(VLNV());
        busInterface_->getAbstractionTypes()->clear();
        busInterface_->getAbstractionTypes()->append(QSharedPointer<AbstractionType>(new AbstractionType())); 
        busInterface_->getPortMaps()->clear();
        busInterface_->setInterfaceMode(General::INTERFACE_MODE_COUNT);

        if (!oldName_.isEmpty())
        {
            busInterface_->setName(oldName_);
        }        
        updateInterface();
    }
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
        return (other->getBusInterface()->getInterfaceMode() == General::INTERFACE_MODE_COUNT ||
            !busInterface_->getBusType().isValid() ||
            otherBusIf->getInterfaceMode() == busInterface_->getInterfaceMode());
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
void BusPortItem::shortenNameLabel( qreal width )
{
    QFont font = nameLabel_.font();
    QString nameLabelText = NamelabelWidth::setNameLabel(name(), font, width);

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
        static_cast<HWDesignDiagram*>(scene())->getEditProvider()->addCommand(moveUndoCommand);
        moveUndoCommand->redo();
    }
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::getModeAndPorts()
//-----------------------------------------------------------------------------
bool BusPortItem::getModeAndPorts(ConnectionEndpoint const* other, General::InterfaceMode& mode, 
    QList< QSharedPointer<Port> >& ports, QList< QSharedPointer<PortMap> >& portMaps)
{
    // Get the possible interface modes for this item.
    QList<General::InterfaceMode> possibleModes;
    if (other->isHierarchical())
    {
        possibleModes.append(other->getBusInterface()->getInterfaceMode());        
    }
    else
    {
        possibleModes = getOpposingModes(other->getBusInterface());
        if (possibleModes.empty())
        {
            return false;
        }     
    }

    // If only one possible mode, select it.
    if (possibleModes.size() == 1)
    {
        mode = possibleModes.first();
    }

    // If there are ports to copy from the other interface, ask for their names and interface mode.
    if (!other->getBusInterface()->getPortMaps()->isEmpty() || possibleModes.size() > 1)
    {
        BusInterfaceDialog dialog(false, (QWidget*)scene()->parent());
        foreach (General::InterfaceMode ifMode, possibleModes)
        {
            dialog.addMode(ifMode);
        }

        if (!other->getBusInterface()->getPortMaps()->isEmpty())
        {
            dialog.setBusInterfaces(other->getOwnerComponent(), other->getBusInterface(), getOwnerComponent(), library_);
        }
        setHighlight(HIGHLIGHT_HOVER);

        if (dialog.exec() == QDialog::Rejected)
        {
            setHighlight(HIGHLIGHT_OFF);
            return false;
        }

        setHighlight(HIGHLIGHT_OFF);

        mode = dialog.getSelectedMode();                
        ports = dialog.getPorts();
        portMaps = dialog.getPortMaps();
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::getOpposingModes()
//-----------------------------------------------------------------------------
QList<General::InterfaceMode> BusPortItem::getOpposingModes(QSharedPointer<BusInterface> busIf) const
{
    QList<General::InterfaceMode> possibleModes;
    
    General::InterfaceMode mode = busIf->getInterfaceMode();

    if (mode == General::MIRROREDMASTER)
    {
        possibleModes.append(General::MASTER);
    }
    else if (mode == General::MIRROREDSLAVE)
    {
        possibleModes.append(General::SLAVE);
    }
    else if (mode == General::MIRROREDSYSTEM)
    {
        possibleModes.append(General::SYSTEM);        
    }
    else if (mode == General::SYSTEM)
    {
        possibleModes.append(General::MIRROREDSYSTEM);     
    }
    else if (mode == General::MASTER)
    {
        // Try to determine whether this port should be slave or mirrored-master.
        possibleModes.append(General::MIRROREDMASTER);    

        QSharedPointer<BusDefinition> busDef = library_->getModel(busIf->getBusType()).dynamicCast<BusDefinition>();
        if (busDef != 0 && busDef->getDirectConnection())
        {
            possibleModes.append(General::SLAVE);      
        }
    }
    else if (mode == General::SLAVE)
    {
        possibleModes.append(General::MIRROREDSLAVE);    

        // Try to determine whether this port should be slave or mirrored-master.
        QSharedPointer<BusDefinition> busDef = library_->getModel(busIf->getBusType()).dynamicCast<BusDefinition>();

        if (busDef != 0 && busDef->getDirectConnection())
        {
            possibleModes.append(General::MASTER); 
        }
    }
    else if (mode == General::MONITOR)
    {
        possibleModes.append(General::MONITOR);    
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
