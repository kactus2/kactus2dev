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
#include "PortmapDialog.h"
#include "HWMoveCommands.h"
#include "HWDesignDiagram.h"

#include "columnview/HWColumn.h"

#include <common/KactusColors.h>
#include <IPXACTmodels/common/DirectionTypes.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <IPXACTmodels/kactusExtensions/InterfaceGraphicsData.h>

#include <common/GenericEditProvider.h>
#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/graphicsItems/CommonGraphicsUndoCommands.h>

#include <designEditors/common/diagramgrid.h>

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
BusInterfaceItem::BusInterfaceItem(LibraryInterface* lh, QSharedPointer<Component> component,
                                   QSharedPointer<BusInterface> busIf,
                                   QSharedPointer<InterfaceGraphicsData> dataGroup, QGraphicsItem *parent):
HWConnectionEndpoint(parent, QVector2D(1.0f, 0.0f)),
library_(lh),
nameLabel_("", this),
busInterface_(busIf),
component_(component),
dataGroup_(dataGroup),
oldColumn_(0),
oldPos_(),
oldInterfacePositions_(),
offPageConnector_(),
portsCopied_(false),
oldName_()
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
// Function: BusInterfaceItem::setTypes()
//-----------------------------------------------------------------------------
void BusInterfaceItem::setTypes(VLNV const& busType, VLNV const& absType, General::InterfaceMode mode)
{
    // Destroy the old bus interface if it exists.
    if (busInterface_->getInterfaceMode() != General::INTERFACE_MODE_COUNT)
    {
        // Disconnect the connections.
        foreach(GraphicsConnection* conn, getConnections())
        {
            if (conn->endpoint1() != this)
            {
                conn->endpoint1()->onDisconnect(this);
            }
            else
            {
                conn->endpoint2()->onDisconnect(this);
            }
        }

        busInterface_->setInterfaceMode(General::INTERFACE_MODE_COUNT);
    }

    busInterface_->setBusType(busType);

    QSharedPointer<AbstractionType> abstraction(new AbstractionType());
    abstraction->setAbstractionRef(QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(absType)));

    busInterface_->getAbstractionTypes()->clear();
    busInterface_->getAbstractionTypes()->append(abstraction);

    busInterface_->setInterfaceMode(mode);

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
// Function: BusInterfaceItem::updateInterface()
//-----------------------------------------------------------------------------
void BusInterfaceItem::updateInterface()
{
    HWConnectionEndpoint::updateInterface();

    Q_ASSERT(busInterface_);

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
    else // if undefined
    {
        setBrush(QBrush(Qt::black));
    }

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
void BusInterfaceItem::define(QSharedPointer<BusInterface> busIf, bool addPorts,
    QList< QSharedPointer<Port> > ports)
{
    if (addPorts)
    {
        component_->getPorts()->append(ports);
    }

    // Add the bus interface to the component.
    component_->getBusInterfaces()->append(busIf);
    busInterface_ = busIf;

    // Update the interface visuals.
    updateInterface();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceItem::undefine()
//-----------------------------------------------------------------------------
void BusInterfaceItem::undefine(bool removePorts)
{
    if (removePorts)
    {
        // Delete all ports related to the bus interface from the top-level component.
        foreach (QSharedPointer<PortMap> portMap, *busInterface_->getPortMaps())
        {
            QSharedPointer<Port> port = component_->getPort(portMap->getPhysicalPort()->name_);
            component_->getPorts()->removeOne(port);
        }
    }

    QSharedPointer<AbstractionType> emptyAbstraction(new AbstractionType());
    emptyAbstraction->setAbstractionRef(QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference()));

    busInterface_->getAbstractionTypes()->clear();
    busInterface_->getAbstractionTypes()->append(emptyAbstraction);

    busInterface_->setBusType(VLNV());

    if (!oldName_.isEmpty())
    {
        setName(oldName_);
    }

    busInterface_->setInterfaceMode(General::INTERFACE_MODE_COUNT);

    // Remove the bus interface from the top-level component and destroy it.
    component_->getBusInterfaces()->removeOne(component_->getBusInterface(busInterface_->name()));
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
// Function: BusInterfaceItem::hasPortsCopied()
//-----------------------------------------------------------------------------
bool BusInterfaceItem::hasPortsCopied() const
{
    return portsCopied_;
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

    // Update the name if the bus interface is defined but its name is empty.
    if (busInterface_->getInterfaceMode() != General::INTERFACE_MODE_COUNT && busInterface_->name().isEmpty())
    {
        busInterface_->setName(other->getBusInterface()->name());
        updateInterface();
    }

    // If the bus interface is already defined or the other endpoint
    // does not have a valid bus interface (unpackaged), we don't have to do anything.
    if (busInterface_->getInterfaceMode() != General::INTERFACE_MODE_COUNT ||
        !other->getBusInterface()->getBusType().isValid())
    {
        return true;
    }

    oldName_ = busInterface_->name();

    // Determine the name for the interface.
    QString newBusIfName = other->getBusInterface()->name();
    unsigned int index = 0;

    while (getOwnerComponent()->getBusInterface(newBusIfName) != 0)
    {
        index++;
        newBusIfName = other->getBusInterface()->name() + "_" + QString::number(index);
    }

    // Create a new bus interface for the diagram interface.
    QSharedPointer<BusInterface> newBusIf(new BusInterface());

    setName(newBusIfName);

    newBusIf->setName(newBusIfName);
    newBusIf->setInterfaceMode(other->getBusInterface()->getInterfaceMode());
    newBusIf->setBusType(other->getBusInterface()->getBusType());

    QSharedPointer<AbstractionType> abstraction (
        new AbstractionType(*other->getBusInterface()->getAbstractionTypes()->first()));

    newBusIf->getAbstractionTypes()->clear();
    newBusIf->getAbstractionTypes()->append(abstraction);

    QSharedPointer<GenericEditProvider> editProvider = 
        static_cast<DesignDiagram*>(scene())->getEditProvider().dynamicCast<GenericEditProvider>();

    if (!editProvider->isPerformingUndoRedo())
    {
        // Ask the user if he wants to auto-copy the port maps from the other end point.
        QMessageBox msgBox(QMessageBox::Question, QCoreApplication::applicationName(),
            tr("Do you want to auto-copy the port maps from the port or perform manual setup instead?"),
            QMessageBox::NoButton, (QWidget*)scene()->parent());
        QPushButton* btnCopy = msgBox.addButton(tr("Copy"), QMessageBox::YesRole);
        QPushButton* btnManual = msgBox.addButton(tr("Manual Setup"), QMessageBox::NoRole);
        msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);

        msgBox.exec();

        if (msgBox.clickedButton() == btnCopy)
        {
            // The user pressed copy, so duplicate all physical ports from the other end point 
            // and create the new port maps at the same time.
            Q_ASSERT(other->encompassingComp() != 0);

            if (!clonePortMaps(newBusIf, other))
            {
                return false;
            }

            portsCopied_ = true;
        }
        else if (msgBox.clickedButton() == btnManual)
        {
            // Otherwise open the port map tool.
            PortmapDialog dialog(library_, component_, newBusIf, other->getBusInterface(), 
                static_cast<QWidget*>(scene()->parent()));

            // If the user canceled the dialog, do not make any connection.
            if (dialog.exec() == QDialog::Rejected)
            {
                return false;
            }

            portsCopied_ = false;
        }
        else
        {
            return false;
        }

        editProvider->setState("portsCopied", portsCopied_);
    }
    else
    {
        portsCopied_ = editProvider->getState("portsCopied").toBool();
    }

    busInterface_ = newBusIf;
    component_->getBusInterfaces()->append(newBusIf);

    // Update the interface visuals.
    updateInterface();

    return true;
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

    // Otherwise undefine the interface.
    QSharedPointer<GenericEditProvider> editProvider = 
        static_cast<DesignDiagram*>(scene())->getEditProvider().dynamicCast<GenericEditProvider>();

    if (editProvider->isPerformingUndoRedo())
    {
        portsCopied_ = editProvider->getState("portsCopied").toBool();
    }

    undefine(portsCopied_);

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
        (*otherBusIf->getAbstractionTypes()->first()->getAbstractionRef() == 
        *busInterface_->getAbstractionTypes()->first()->getAbstractionRef() &&
        otherBusIf->getBusType() == busInterface_->getBusType()));
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
    updateInterface();
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
// Function: BusInterfaceItem::clonePortMaps()
//-----------------------------------------------------------------------------
bool BusInterfaceItem::clonePortMaps(QSharedPointer<BusInterface> busIf, ConnectionEndpoint const* other)
{
    QSharedPointer<Component> otherComp = other->getOwnerComponent();

    // Duplicate the ports referenced by the port maps and duplicate the port maps.
    // Translation table keeps track of the mapped port names.
    QMap<QString, QString> nameMappings;
    QList< QSharedPointer<PortMap> > newPortMaps;

    foreach (QSharedPointer<PortMap> portMap, *other->getBusInterface()->getPortMaps())
    {
        // Duplicate the port if it hasn't already been duplicated.
        if (!nameMappings.contains(portMap->getPhysicalPort()->name_))
        {
            // Retrieve the port from the model.
            QSharedPointer<Port> port = otherComp->getPort(portMap->getPhysicalPort()->name_);

            // Check if the port was not found.
            if (!port)
            {
                emit errorMessage(tr("Port %1 was not found in component %2 while copying the bus interface.").arg(
                    portMap->getPhysicalPort()->name_, other->encompassingComp()->name()));
                return false;
            }

            // The new port inherits the same name as the physical port in the other component.
            QString newPortName = portMap->getPhysicalPort()->name_;
            unsigned int runningNumber = 0;

            // Make sure that the port name is unique.
            while (component_->getPort(newPortName) != 0)
            {
                ++runningNumber;
                newPortName = portMap->getPhysicalPort()->name_ + "_" + QString::number(runningNumber);
            }

            // Create the port and add it to the component.
            QSharedPointer<Port> newPort(new Port(*port));
            newPort->setName(newPortName);

            component_->getPorts()->append(newPort);

            // Update the translation table.
            nameMappings.insert(portMap->getPhysicalPort()->name_, newPortName);
        }

        // Duplicate and add the port map to the bus interface.
        QSharedPointer<PortMap> newPortMap(new PortMap(*portMap));

        QString portName = nameMappings.value(portMap->getPhysicalPort()->name_);
        QSharedPointer<PortMap::PhysicalPort> physicalPort(new PortMap::PhysicalPort(portName));

        if (portMap->getPhysicalPort()->partSelect_)
        {
            QSharedPointer<PartSelect> copiedPartSelect (new PartSelect(*portMap->getPhysicalPort()->partSelect_));
            physicalPort->partSelect_ = copiedPartSelect;
        }

        newPortMap->setPhysicalPort(physicalPort);

        newPortMaps.append(newPortMap);
    }

    busIf->getPortMaps()->clear();
    busIf->getPortMaps()->append(newPortMaps);

    return true;
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