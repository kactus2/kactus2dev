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
                         QGraphicsItem *parent) : HWConnectionEndpoint(parent),
lh_(lh),
                                                  temp_(!busIf->getBusType().isValid()),
                                                  oldPos_(), oldPortPositions_(),
                                                  offPageConnector_(0)
{
    Q_ASSERT_X(busIf, "BusPortItem constructor",
        "Null BusInterface pointer given as parameter");

    setType(ENDPOINT_TYPE_BUS);
    busInterface_ = busIf;

    int squareSize = GridSize;
    /*  /\
     *  ||
     */
    QPolygonF shape;
    shape << QPointF(-squareSize/2, squareSize/2)
          << QPointF(-squareSize/2, .0)
          << QPointF(0, -squareSize/2)
          << QPointF(squareSize/2, 0)
          << QPointF(squareSize/2, squareSize/2);
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

BusPortItem::~BusPortItem() {
}

//-----------------------------------------------------------------------------
// Function: setTemporary()
//-----------------------------------------------------------------------------
void BusPortItem::setTemporary(bool temp)
{
    temp_ = temp;
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
    busInterface_->setName(name);
    encompassingComp()->componentModel()->updateBusInterface(busInterface_.data());

	updateInterface();
    emit contentChanged();
}

QSharedPointer<BusInterface> BusPortItem::getBusInterface() const
{
    return busInterface_;
}

void BusPortItem::updateInterface()
{
    // Set the port black if it is temporary.
    if (!busInterface_->getBusType().isValid())
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

    nameLabel_->setHtml("<div style=\"background-color:#eeeeee; padding:10px 10px;\">"
                        + busInterface_->getName() + "</div>");

    qreal nameWidth = nameLabel_->boundingRect().width();
    qreal nameHeight = nameLabel_->boundingRect().height();

    if (pos().x() < 0)
    {
        nameLabel_->setPos(nameHeight / 2, GridSize / 2);
    }
    else
    {
        nameLabel_->setPos(-nameHeight / 2, GridSize / 2 + nameWidth);
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

    // If the port is a temporary one, try to copy the configuration from the other end point.
    if (temp_ && otherBusIf != 0 && otherBusIf->getBusType().isValid())
    {
        if (!static_cast<HWDesignDiagram*>(scene())->getEditProvider().isPerformingUndoRedo())
        {
            // Set a compatible interface mode. If the other end point is a hierarchical one,
            // the same interface mode injects automatically. Otherwise the proper interface mode must
            // be determined/asked based on the other bus interface.
            General::InterfaceMode mode = otherBusIf->getInterfaceMode();
           
            if (!other->isHierarchical())
            {
                if (!askCompatibleMode(otherBusIf, mode))
                {
                    return false;
                }
            }

            busInterface_->setInterfaceMode(mode);
        }

        // Copy the bus and abstraction definitions.
        busInterface_->setBusType(otherBusIf->getBusType());
        busInterface_->setAbstractionType(otherBusIf->getAbstractionType());
        updateInterface();
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: onDisonnect()
//-----------------------------------------------------------------------------
void BusPortItem::onDisconnect(ConnectionEndpoint const*)
{
    // If the port is a temporary one, set the bus and abstraction definitions undefined.
    if (temp_)
    {
        busInterface_->setBusType(VLNV());
        busInterface_->setAbstractionType(VLNV());
        updateInterface();
    }
}

//-----------------------------------------------------------------------------
// Function: canConnect()
//-----------------------------------------------------------------------------
bool BusPortItem::canConnect(ConnectionEndpoint const* other) const
{
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

        if (myMode == General::MASTER) {
            if (otherMode == General::SLAVE || otherMode == General::MIRROREDMASTER) {
                return true;
            }
        } else if (myMode == General::SLAVE) {

            if (otherMode == General::MASTER || otherMode == General::MIRROREDSLAVE) {
                return true;
            }
        } else if (myMode == General::MIRROREDMASTER && otherMode == General::MASTER) {
            return true;
        } else if (myMode == General::MIRROREDSLAVE && otherMode == General::SLAVE) {
            return true;
        } else if (myMode == General::SYSTEM || otherMode == General::SYSTEM) {
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
                nameLabel_->setPos(nameHeight/2, GridSize/2);
            }
            // Otherwise the port is directed to the right.
            else
            {
                setDirection(QVector2D(1.0f, 0.0f));
                nameLabel_->setPos(-nameHeight/2, GridSize/2 + nameWidth);
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
    if (static_cast<HWDesignDiagram*>(scene())->isProtected())
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
    setTemporary(!busType.isValid());
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

    // Begin the position update for the connections.
    foreach (GraphicsConnection* conn, getConnections())
    {
        conn->beginUpdatePosition();
    }
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void BusPortItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    HWConnectionEndpoint::mouseReleaseEvent(event);

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
            QUndoCommand* childCmd =
                new PortMoveCommand(static_cast<HWConnectionEndpoint*>(cur.key()), cur.value(), cmd.data());
        }

        ++cur;
    }

    oldPortPositions_.clear();
    
    // End the position update of the connections.
    foreach (GraphicsConnection* conn, getConnections())
    {
        conn->endUpdatePosition(cmd.data());
    }

    // Add the undo command to the edit stack only if it has changes.
    if (cmd->childCount() > 0 || oldPos_ != pos())
    {
        static_cast<HWDesignDiagram*>(scene())->getEditProvider().addCommand(cmd, false);
    }
}

//-----------------------------------------------------------------------------
// Function: askCompatibleMode()
//-----------------------------------------------------------------------------
bool BusPortItem::askCompatibleMode(QSharedPointer<BusInterface> otherBusIf,
                                    General::InterfaceMode& mode)
{
    switch (otherBusIf->getInterfaceMode())
    {
    case General::MIRROREDMASTER:
        {
            mode = General::MASTER;
            break;
        }

    case General::MIRROREDSLAVE:
        {
            mode = General::SLAVE;
            break;
        }

    case General::MIRROREDSYSTEM:
        {
            mode = General::SYSTEM;
            break;
        }

    case General::SYSTEM:
        {
            mode = General::MIRROREDSYSTEM;
            break;
        }

    case General::MASTER:
        {
            // Try to determine whether this port should be slave or mirrored-master.
            QSharedPointer<LibraryComponent> libComp = lh_->getModel(otherBusIf->getBusType());
            QSharedPointer<BusDefinition> busDef = libComp.dynamicCast<BusDefinition>();

            if (busDef == 0)
            {
                return false;
            }

            if (!busDef->getDirectConnection())
            {
                // This must be a mirrored-master.
                mode = General::MIRROREDMASTER;
            }
            else
            {
                // Ask the user for the correct type.
                setHighlight(HIGHLIGHT_HOVER);

                BusInterfaceDialog dialog(false, (QWidget*)scene()->parent());
                dialog.addMode(General::MIRROREDMASTER);
                dialog.addMode(General::SLAVE);

                if (dialog.exec() == QDialog::Rejected)
                {
                    setHighlight(HIGHLIGHT_OFF);
                    return false;
                }

                setHighlight(HIGHLIGHT_OFF);

                mode = dialog.getSelectedMode();
            }
            break;
        }

    case General::SLAVE:
        {
            // Try to determine whether this port should be slave or mirrored-master.
            QSharedPointer<LibraryComponent> libComp = lh_->getModel(otherBusIf->getBusType());
            QSharedPointer<BusDefinition> busDef = libComp.dynamicCast<BusDefinition>();

            if (busDef == 0)
            {
                return false;
            }

            if (!busDef->getDirectConnection())
            {
                // This must be a mirrored-master.
                mode = General::MIRROREDMASTER;
            }
            else
            {
                // Ask the user for the correct type.
                setHighlight(HIGHLIGHT_HOVER);

                BusInterfaceDialog dialog(false, (QWidget*)scene()->parent());
                dialog.addMode(General::MIRROREDSLAVE);
                dialog.addMode(General::MASTER);

                if (dialog.exec() == QDialog::Rejected)
                {
                    setHighlight(HIGHLIGHT_OFF);
                    return false;
                }

                setHighlight(HIGHLIGHT_OFF);

                mode = dialog.getSelectedMode();

            }
            break;
        }

    case General::MONITOR:
        {
            mode = General::MONITOR;
            break;
        }
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
