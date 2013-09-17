/* 
 *
 *         filename: diagramport.cpp
 */

#include "BusPortItem.h"
#include "HWComponentItem.h"
#include "HWConnection.h"
#include "BusInterfaceDialog.h"
#include "HWMoveCommands.h"
#include "HWDesignDiagram.h"
#include "OffPageConnectorItem.h"

#include <common/graphicsItems/GraphicsConnection.h>
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

BusPortItem::BusPortItem(QSharedPointer<BusInterface> busIf, LibraryInterface* lh,
                         bool packetized, QGraphicsItem *parent)
    : HWConnectionEndpoint(parent, !packetized),
      lh_(lh),
      oldPos_(), oldPortPositions_(),
      offPageConnector_(0), oldName_()
{
    Q_ASSERT_X(busIf, "BusPortItem constructor",
        "Null BusInterface pointer given as parameter");

    setType(ENDPOINT_TYPE_BUS);
    setTypeLocked(packetized);
    busInterface_ = busIf;

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

BusPortItem::~BusPortItem() {
}

QString BusPortItem::name() const
{
    return busInterface_->getName();
}

//-----------------------------------------------------------------------------
// Function: setName()
//-----------------------------------------------------------------------------
void BusPortItem::setName( const QString& name )
{
    beginUpdateConnectionNames();

    busInterface_->setName(name);

	updateInterface();
    emit contentChanged();

    endUpdateConnectionNames();
}

QSharedPointer<BusInterface> BusPortItem::getBusInterface() const
{
    return busInterface_;
}

void BusPortItem::updateInterface()
{
    HWConnectionEndpoint::updateInterface();

    if (isInvalid())
    {
        setBrush(QBrush(Qt::red));
    }
    else if (!busInterface_->getBusType().isValid() ||
             busInterface_->getInterfaceMode() == General::INTERFACE_MODE_COUNT)
    {
        setBrush(QBrush(Qt::black));
    }
    else
    {
        // Otherwise set the color based on the interface mode.
        switch (busInterface_->getInterfaceMode()) {
        case General::MASTER:
            setBrush(QBrush(QColor(0x32,0xcb,0xcb)));
            break;
        case General::SLAVE:
            setBrush(QBrush(QColor(0x32,0x99,0x64)));
            break;
        case General::MIRROREDMASTER:
            setBrush(QBrush(QColor(0xcb,0xfd,0xfd)));
            break;
        case General::MIRROREDSLAVE:
            setBrush(QBrush(QColor(0x00,0xfd,00)));
            break;
        case General::SYSTEM:
            setBrush(QBrush(QColor(0xf9,0x11,0x11)));
            break;
        case General::MIRROREDSYSTEM:
            setBrush(QBrush(QColor(0xf9,0x9d,0xcb)));
            break;
        case General::MONITOR:
            setBrush(QBrush(QColor(0xfd,0xfd,0xfd)));
            break;
        }
    }

    // Determine the bus direction.
    General::Direction dir = General::DIRECTION_INVALID;

    if (busInterface_->getPhysicalPortNames().size() > 0)
    {
        foreach (QString const& portName, busInterface_->getPhysicalPortNames())
        {
            QSharedPointer<Port> port = getOwnerComponent()->getPort(portName);

            if (port != 0)
            {
                if (dir == General::DIRECTION_INVALID)
                {
                    dir = port->getDirection();
                }
                else if (dir != port->getDirection())
                {
                    dir = General::INOUT;
                    break;
                }
            }
        }
    }
    else
    {
        dir = General::INOUT;
    }

    // Update the polygon shape based on the direction.
    int squareSize = GridSize;
    QPolygonF shape;

    switch (dir)
    {
    case General::IN:
        {
            /*  ||
            *  \/
            */
            shape << QPointF(-squareSize/2, 0)
                << QPointF(-squareSize/2, -squareSize/2)
                << QPointF(squareSize/2, -squareSize/2)
                << QPointF(squareSize/2, 0)
                << QPointF(0, squareSize/2);
            break;
        }

    case General::OUT:
        {
            /*  /\
            *  ||
            */
            shape << QPointF(-squareSize/2, squareSize/2)
                << QPointF(-squareSize/2, 0)
                << QPointF(0, -squareSize/2)
                << QPointF(squareSize/2, 0)
                << QPointF(squareSize/2, squareSize/2);
            break;
        }

    case General::INOUT:
    default:
        {
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
            break;
        }
    }

    setPolygon(shape);

    nameLabel_->setHtml("<div style=\"background-color:#eeeeee; padding:10px 10px;\">"
                        + busInterface_->getName() + "</div>");

    qreal nameWidth = nameLabel_->boundingRect().width();
    qreal nameHeight = nameLabel_->boundingRect().height();

    if (pos().x() < 0)
    {
        nameLabel_->setPos(nameHeight / 2, GridSize);
    }
    else
    {
        nameLabel_->setPos(-nameHeight / 2, GridSize + nameWidth);
    }

    offPageConnector_->updateInterface();
}

bool BusPortItem::isHierarchical() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: onConnect()
//-----------------------------------------------------------------------------
bool BusPortItem::onConnect(ConnectionEndpoint const* other)
{
    QSharedPointer<BusInterface> otherBusIf = other->getBusInterface();

    // If the port is a non-typed one, try to copy the configuration from the other end point.
    if (getConnections().empty() && !isTypeLocked())
    {
        oldName_ = busInterface_->getName();

        if (otherBusIf != 0 && otherBusIf->getBusType().isValid())
        {
            GenericEditProvider& editProvider = static_cast<DesignDiagram*>(scene())->getEditProvider();
            editProvider.setState("portsCopied", false);

            if (!static_cast<HWDesignDiagram*>(scene())->getEditProvider().isPerformingUndoRedo() &&
                !static_cast<HWDesignDiagram*>(scene())->isLoading())
            {                            
                QList< QSharedPointer<Port> > newPorts;
                QList< QSharedPointer<General::PortMap> > newPortMaps;

                General::InterfaceMode mode = otherBusIf->getInterfaceMode();
                // Set a compatible interface mode. If the other end point is a hierarchical one,
                // the same interface mode injects automatically. Otherwise the proper interface mode must
                // be determined/asked based on the other bus interface.                
                if (!getModeAndPorts(other, mode, newPorts, newPortMaps))
                {
                    return false;
                }

                busInterface_->setInterfaceMode(mode);

                editProvider.setState("portsCopied", true);

                foreach ( QSharedPointer<Port> port, newPorts)
                {
                    encompassingComp()->componentModel()->addPort(port);
                } 

                busInterface_->setPortMaps(newPortMaps);
            }
    
            QString ifName = other->getBusInterface()->getName();
            int count = 0;
            while( getOwnerComponent()->hasInterface(ifName) )
            {
                count++;
                ifName = other->getBusInterface()->getName() + "_" + QString::number(count);
            }
            busInterface_->setName(ifName);

            // Copy the bus and abstraction definitions.
            busInterface_->setBusType(otherBusIf->getBusType());
            busInterface_->setAbstractionType(otherBusIf->getAbstractionType());
            updateInterface();
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: onDisonnect()
//-----------------------------------------------------------------------------
void BusPortItem::onDisconnect(ConnectionEndpoint const*)
{
    // If the port is a non-typed one, set the bus and abstraction definitions undefined.
    if (getConnections().empty() && !isTypeLocked())
    {
        busInterface_->setBusType(VLNV());
        busInterface_->setAbstractionType(VLNV());      
        busInterface_->setPortMaps(QList< QSharedPointer<General::PortMap> >());
        busInterface_->setName(oldName_);
        updateInterface();
    }
}

//-----------------------------------------------------------------------------
// Function: isConnectionValid()
//-----------------------------------------------------------------------------
bool BusPortItem::isConnectionValid(ConnectionEndpoint const* other) const
{
    if (!HWConnectionEndpoint::isConnectionValid(other))
    {
        return false;
    }

    // This end point requires a bus interface connection.
    if (!other->isBus())
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
                other->getBusInterface()->getInterfaceMode() == busInterface_->getInterfaceMode());
    }
    // If only one port has a bus definition defined at most, then the end points can be connected.
    else if (!(busInterface_->getBusType().isValid() && otherBusIf->getBusType().isValid()))
    {
        return true;
    }
    // Otherwise make sure that the bus and abstraction definitions are of the same type.
    else if (otherBusIf->getBusType() == busInterface_->getBusType() &&
             otherBusIf->getAbstractionType() == busInterface_->getAbstractionType())
    {
        General::InterfaceMode myMode = busInterface_->getInterfaceMode();
        General::InterfaceMode otherMode = otherBusIf->getInterfaceMode();

        // Retrieve the bus definition to get access to the direct connection flag.
        QSharedPointer<LibraryComponent const> libComp = lh_->getModelReadOnly(busInterface_->getBusType());
        QSharedPointer<BusDefinition const> busDef = libComp.dynamicCast<BusDefinition const>();

        bool directConnection = busDef == 0 || busDef->getDirectConnection();

        if (myMode == General::MASTER)
        {
            if ((directConnection && otherMode == General::SLAVE) || otherMode == General::MIRROREDMASTER)
            {
                return true;
            }
        }
        else if (myMode == General::SLAVE)
        {
            if ((directConnection && otherMode == General::MASTER) || otherMode == General::MIRROREDSLAVE)
            {
                return true;
            }
        }
        else if (myMode == General::MIRROREDMASTER && otherMode == General::MASTER)
        {
            return true;
        }
        else if (myMode == General::MIRROREDSLAVE && otherMode == General::SLAVE)
        {
            return true;
        }
        else if (myMode == General::SYSTEM || otherMode == General::SYSTEM)
        {
            return true;
        }
    }

    return false;
}

ComponentItem* BusPortItem::encompassingComp() const
{
    return static_cast<ComponentItem*>(parentItem());
}

QSharedPointer<Component> BusPortItem::getOwnerComponent() const {
	ComponentItem* comp = encompassingComp();
	Q_ASSERT(comp);
	QSharedPointer<Component> compModel = comp->componentModel();
	Q_ASSERT(compModel);
	return compModel;
}

QVariant BusPortItem::itemChange(GraphicsItemChange change,
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
            QRectF parentRect = qgraphicsitem_cast<HWComponentItem *>(parentItem())->rect();

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

            QRectF parentRect = qgraphicsitem_cast<HWComponentItem *>(parentItem())->rect();

            // Check if the port is directed to the left.
            if (pos().x() < 0)
            {
                setDirection(QVector2D(-1.0f, 0.0f));
                nameLabel_->setPos(nameHeight/2, GridSize);
            }
            // Otherwise the port is directed to the right.
            else
            {
                setDirection(QVector2D(1.0f, 0.0f));
                nameLabel_->setPos(-nameHeight/2, GridSize + nameWidth);
            }

            break;
        }

    case ItemScenePositionHasChanged:
        // Check if the updates are not disabled.
        if (!static_cast<HWComponentItem*>(parentItem())->isConnectionUpdateDisabled())
        {
            // Update the connections.
            foreach (GraphicsConnection* interconnection, getConnections())
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
bool BusPortItem::isDirectionFixed() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
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
    static_cast<HWComponentItem*>(parentItem())->onMovePort(this);
}

//-----------------------------------------------------------------------------
// Function: setTypes()
//-----------------------------------------------------------------------------
void BusPortItem::setTypes(VLNV const& busType, VLNV const& absType, General::InterfaceMode mode)
{
    Q_ASSERT(busInterface_ != 0);

    // If the bus interface was already defined, disconnect the connections before making any updates.
    if (busInterface_->getBusType().isValid())
    {
        foreach(GraphicsConnection* conn, getConnections())
        {
            if (conn->endpoint1() != this)
            {
                conn->endpoint1()->removeConnection(conn);
                conn->endpoint1()->onDisconnect(this);
                conn->endpoint1()->addConnection(conn);
            }
            else
            {
                conn->endpoint2()->removeConnection(conn);
                conn->endpoint2()->onDisconnect(this);
                conn->endpoint2()->addConnection(conn);
            }
        }
    }

    // Initialize the bus interface.
    busInterface_->setBusType(busType);
    busInterface_->setAbstractionType(absType);
    busInterface_->setInterfaceMode(mode);

    // Update the interface visuals.
    setTypeLocked(busType.isValid());
    updateInterface();

    if (busType.isValid())
    {
        // Undefined end points of the connections can now be defined.
        foreach(GraphicsConnection* conn, getConnections())
        {
            if (conn->endpoint1() != this)
            {
                conn->endpoint1()->onConnect(this);
                conn->endpoint2()->onConnect(conn->endpoint1());
            }
            else
            {
                conn->endpoint2()->onConnect(this);
                conn->endpoint1()->onConnect(conn->endpoint2());
            }
        }
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void BusPortItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
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
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void BusPortItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    HWConnectionEndpoint::mouseReleaseEvent(event);

    DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(scene());

    if (diagram == 0)
    {
        // Update the default position in case the graphics are located in other scene than the designer.
        busInterface_->setDefaultPos(pos());

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

        ++cur;
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
        static_cast<HWDesignDiagram*>(scene())->getEditProvider().addCommand(cmd, false);
    }
}

//-----------------------------------------------------------------------------
// Function: getModeAndPorts()
//-----------------------------------------------------------------------------
bool BusPortItem::getModeAndPorts(ConnectionEndpoint const* other,
                                    General::InterfaceMode& mode, QList< QSharedPointer<Port> >& ports,
                                    QList< QSharedPointer<General::PortMap> >& portMaps)
{
    // Get the possible interface modes for this item.
    QList<General::InterfaceMode> modes;
    if (other->isHierarchical())
    {
        modes.append(other->getBusInterface()->getInterfaceMode());        
    }
    else
    {
        modes = getOpposingModes(other->getBusInterface());
        if (modes.empty())
        {
            return false;
        }     
    }

    // If only one possible mode, select it.
    if (modes.size() == 1)
    {
        mode = modes.first();
    }

    // If there are ports to copy from the other interface, ask for their names and interface mode.
    if (!other->getBusInterface()->getPortMaps().empty() || modes.size() > 1)
    {
        BusInterfaceDialog dialog(false, (QWidget*)scene()->parent());
        foreach (General::InterfaceMode ifMode, modes)
        {
            dialog.addMode(ifMode);
        }        
        if (!other->getBusInterface()->getPortMaps().empty())
        {
            dialog.setBusInterfaces(other->getOwnerComponent(), other->getBusInterface(), getOwnerComponent(), lh_);
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

void BusPortItem::setInterfaceMode( General::InterfaceMode mode ) {
    Q_ASSERT(busInterface_);
    busInterface_->setInterfaceMode(mode);
    updateInterface();
}

QString BusPortItem::description() const {
    Q_ASSERT(busInterface_);
    return busInterface_->getDescription();
}

void BusPortItem::setDescription( const QString& description ) {
    Q_ASSERT(busInterface_);
    busInterface_->setDescription(description);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: getOffPageConnector()
//-----------------------------------------------------------------------------
ConnectionEndpoint* BusPortItem::getOffPageConnector()
{
    return offPageConnector_;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::isBus()
//-----------------------------------------------------------------------------
bool BusPortItem::isBus() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::getPort()
//-----------------------------------------------------------------------------
Port* BusPortItem::getPort() const
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::isExclusive()
//-----------------------------------------------------------------------------
bool BusPortItem::isExclusive() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: BusPortItem::getOpposingModes()
//-----------------------------------------------------------------------------
QList<General::InterfaceMode> BusPortItem::getOpposingModes(QSharedPointer<BusInterface> busIf)
{
    QList<General::InterfaceMode> modes;
    switch (busIf->getInterfaceMode())
    {
    case General::MIRROREDMASTER:
        {
            modes.append(General::MASTER);
            break;
        }

    case General::MIRROREDSLAVE:
        {
            modes.append(General::SLAVE);
            break;
        }

    case General::MIRROREDSYSTEM:
        {
            modes.append(General::SYSTEM);        
            break;
        }

    case General::SYSTEM:
        {
            modes.append(General::MIRROREDSYSTEM);     
            break;
        }

    case General::MASTER:
        {
            // Try to determine whether this port should be slave or mirrored-master.
            QSharedPointer<LibraryComponent> libComp = lh_->getModel(busIf->getBusType());
            QSharedPointer<BusDefinition> busDef = libComp.dynamicCast<BusDefinition>();

            if (busDef == 0)
            {
                break;
            }

            if (!busDef->getDirectConnection())
            {
                // This must be a mirrored-master.
                modes.append(General::MIRROREDMASTER);     
            }
            else
            {
                // Ask the user for the correct type.
                modes.append(General::MIRROREDMASTER);    
                modes.append(General::SLAVE);    
            }
            break;
        }

    case General::SLAVE:
        {
            // Try to determine whether this port should be slave or mirrored-master.
            QSharedPointer<LibraryComponent> libComp = lh_->getModel(busIf->getBusType());
            QSharedPointer<BusDefinition> busDef = libComp.dynamicCast<BusDefinition>();

            if (busDef == 0)
            {
                break;
            }

            if (!busDef->getDirectConnection())
            {
                // This must be a mirrored-slave.
                modes.append(General::MIRROREDSLAVE);    
            }
            else
            {
                // Ask the user for the correct type.
                modes.append(General::MIRROREDSLAVE);    
                modes.append(General::MASTER);    
            }
            break;
        }

    case General::MONITOR:
        {
            modes.append(General::MONITOR);    
            break;
        }
    }

    return modes;
}

