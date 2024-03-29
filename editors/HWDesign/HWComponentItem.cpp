//-----------------------------------------------------------------------------
// File: HWComponentItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: 
// Date:
//
// Description:
// HWComponentItem represents graphically an IP-XACT component instance.
//-----------------------------------------------------------------------------

#include "HWComponentItem.h"
#include "HWMoveCommands.h"

#include "columnview/HWColumn.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <common/IEditProvider.h>
#include <common/KactusColors.h>

#include <common/graphicsItems/CommonGraphicsUndoCommands.h>
#include <common/graphicsItems/GraphicsConnection.h>

#include <editors/common/diagramgrid.h>
#include <editors/common/DesignDiagram.h>
#include <editors/HWDesign/AdHocItem.h>
#include <editors/HWDesign/ActiveBusInterfaceItem.h>
#include <editors/HWDesign/ActivePortItem.h>
#include <editors/HWDesign/AdhocEditor/AdHocVisibilityEditor.h>
#include <editors/HWDesign/undoCommands/ComponentItemMoveCommand.h>

#include <IPXACTmodels/Design/ComponentInstance.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>

#include <QGraphicsDropShadowEffect>
#include <QFont>
#include <QTextDocument>

//-----------------------------------------------------------------------------
// Function: HWComponentItem::HWComponentItem()
//-----------------------------------------------------------------------------
HWComponentItem::HWComponentItem(LibraryInterface* libInterface,
QSharedPointer<ComponentInstance> instance, QSharedPointer<Component> component,
QGraphicsItem* parent) : 
ComponentItem(QRectF(), libInterface, instance, component, parent)
{
    setFlag(ItemIsMovable);
    setAdHocData(component, instance->getPortAdHocVisibilities());

    hierIcon_->setToolTip(tr("Hierarchical"));
    hierIcon_->setPos(COMPONENTWIDTH/2 - hierIcon_->pixmap().width() - SPACING, SPACING);

    static constexpr int portSpacing = 3*GridSize;
    int portCountOnLeft = component->getBusInterfaces()->size() / 2.0 + .5;
    setRect(-COMPONENTWIDTH/ 2, 0, COMPONENTWIDTH, 6 * GridSize + portSpacing * qMax(portCountOnLeft - 1, 0));

    positionBusInterfaceTerminals();

    positionAdHocPortTerminals();

    updateSize();
    HWComponentItem::updateComponent();
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::~HWComponentItem()
//-----------------------------------------------------------------------------
HWComponentItem::~HWComponentItem()
{
    // Remove all interconnections.
    for (QGraphicsItem *item : childItems())
    {
        if (item->type() == ActiveBusInterfaceItem::Type)
        {
            auto diagramPort = qgraphicsitem_cast<ActiveBusInterfaceItem*>(item);
            for (GraphicsConnection* interconnection : diagramPort->getConnections())
            {
                delete interconnection;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::updateComponent()
//-----------------------------------------------------------------------------
void HWComponentItem::updateComponent()
{
    ComponentItem::updateComponent();

    // Check whether the component is packaged (valid vlnv) or not.
    if (isDraft())
    {
        setBrush(QBrush(KactusColors::DRAFT_COMPONENT));
    }
    else if (getLibraryInterface()->contains(componentModel()->getVlnv()) == false)
    {
        setBrush(QBrush(KactusColors::MISSING_COMPONENT));
    }
    else if (componentModel()->isBus())
    {
        setBrush(QBrush(KactusColors::HW_BUS_COMPONENT));
    }
    else
    {
        setBrush(QBrush(KactusColors::HW_COMPONENT));
    }

    // Show a hierarchy icon if the component is a hierarchical one.
    hierIcon_->setVisible(componentModel()->isHierarchical());
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::setPortAdHocVisible()
//-----------------------------------------------------------------------------
void HWComponentItem::onAdHocVisibilityChanged(QString const& portName, bool visible)
{
    ActivePortItem* port = getAdHocPort(portName);

    // Create/destroy the ad-hoc port graphics item.
    if (visible)
    {
        QSharedPointer<Port> adhocPort = componentModel()->getPort(portName);
        if (!adhocPort)
        {
            adhocPort = QSharedPointer<Port>(new Port(portName));
        }

        if (!port)
        {
            port = new ActivePortItem(adhocPort, this);
            addPortToSideWithLessPorts(port);
        }
        
        getComponentInstance()->updateAdHocPortPosition(portName, port->pos());

        // Update the component's size after addition.
        updateSize();
    }
    else
    {
        // Search for the ad-hoc port from both sides.
        Q_ASSERT(port != nullptr);

        // Remove the port and delete it.
        removePort(port);
        delete port;
        port = nullptr;

        getComponentInstance()->hideAdHocPort(portName);
    }

    emit adHocVisibilitiesChanged();
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::attach()
//-----------------------------------------------------------------------------
void HWComponentItem::attach(AdHocVisibilityEditor* editor)
{
    connect(this, SIGNAL(adHocVisibilitiesChanged()), editor, SLOT(onContentChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::detach()
//-----------------------------------------------------------------------------
void HWComponentItem::detach(AdHocVisibilityEditor* editor)
{
    disconnect(editor);
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::adHocIdentifier()
//-----------------------------------------------------------------------------
QString HWComponentItem::adHocIdentifier() const
{
    return name();
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::getDiagramAdHocPort()
//-----------------------------------------------------------------------------
HWConnectionEndpoint* HWComponentItem::getDiagramAdHocPort(QString const& portName)
{
    return getAdHocPort(portName);
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::addPort()
//-----------------------------------------------------------------------------
ActiveBusInterfaceItem* HWComponentItem::addPort(QPointF const& pos)
{
    // Determine a unique name for the bus interface.
    QString name = QStringLiteral("bus");
    unsigned int count = 0;

    QStringList knownNames = componentModel()->getBusInterfaceNames();

    while (knownNames.contains(name))
    {
        count++;
        name = "bus_" + QString::number(count);
    }

    // Create an empty bus interface and add it to the component model.
    QSharedPointer<BusInterface> busIf(new BusInterface());
    busIf->setName(name);
    busIf->setInterfaceMode(General::INTERFACE_MODE_COUNT);
    componentModel()->getBusInterfaces()->append(busIf);

    // Create the visualization for the bus interface.
    auto draftPort = new ActiveBusInterfaceItem(busIf, getLibraryInterface(), this);
    draftPort->setTemporary(true);
    draftPort->setPos(mapFromScene(pos));
    
    addPortToSideByPosition(draftPort);

    onMovePort(draftPort);

    return draftPort;
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::addPort()
//-----------------------------------------------------------------------------
void HWComponentItem::addPort(HWConnectionEndpoint* port)
{
    port->setParentItem(this);

    if (port->type() == ActiveBusInterfaceItem::Type)
    {
        // Add the bus interface to the component.
        componentModel()->getBusInterfaces()->append(port->getBusInterface());
    }

    addPortToSideByPosition(port);

    onMovePort(port);
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::removePort()
//-----------------------------------------------------------------------------
void HWComponentItem::removePort(HWConnectionEndpoint* port)
{
    disconnect(port, SIGNAL(moved(ConnectionEndpoint*)), this, SIGNAL(endpointMoved(ConnectionEndpoint*)));

    leftPorts_.removeAll(port);
    rightPorts_.removeAll(port);
    updateSize();

    if (port->type() == ActiveBusInterfaceItem::Type)
    {
        componentModel()->getBusInterfaces()->removeOne(port->getBusInterface());
        getComponentInstance()->removeBusInterfacePosition(port->name());
    }
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::getBusPort()
//-----------------------------------------------------------------------------
ActiveBusInterfaceItem* HWComponentItem::getBusPort(QString const& name) const
{
    for (QGraphicsItem* item : QGraphicsRectItem::childItems())
    {
        if (item->type() == ActiveBusInterfaceItem::Type)
        {
            ActiveBusInterfaceItem* busPort = qgraphicsitem_cast<ActiveBusInterfaceItem*>(item);
            if (busPort->name() == name)
            {
                return busPort;
            }
        }
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::getAdHocPort()
//-----------------------------------------------------------------------------
ActivePortItem* HWComponentItem::getAdHocPort(QString const& portName) const
{
    auto matchName = [&portName](auto endpoint) {
        return dynamic_cast<ActivePortItem*>(endpoint) != nullptr && endpoint->name() == portName; };

    if (auto leftPort = std::find_if(leftPorts_.cbegin(), leftPorts_.cend(), matchName);
        leftPort != leftPorts_.cend())
    {
        return static_cast<ActivePortItem*>(*leftPort);
    }

    if (auto rightPort = std::find_if(rightPorts_.cbegin(), rightPorts_.cend(), matchName); 
        rightPort != rightPorts_.cend())
    {
        return static_cast<ActivePortItem*>(*rightPort);
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::onMovePort()
//-----------------------------------------------------------------------------
void HWComponentItem::onMovePort(ConnectionEndpoint* port)
{
    // Remove the port from the stacks (this simplifies code).
    leftPorts_.removeAll(port);
    rightPorts_.removeAll(port);

    // Restrict the position so that the port cannot be placed too high.
    port->setPos(snapPointToGrid(port->x(), qMax(MIN_Y_PLACEMENT - port->boundingRect().top(), port->y())));

    // Check on which side the port is to determine the stack to which it should be placed.
    if (port->x() < 0)
    {
        portLayout_->updateItemMove(leftPorts_, port, MIN_Y_PLACEMENT);
        checkPortLabelSize(port, rightPorts_);
    }
    else
    {
        portLayout_->updateItemMove(rightPorts_, port, MIN_Y_PLACEMENT);
        checkPortLabelSize(port, leftPorts_);
    }

    if (port->isBus())
    {
        getComponentInstance()->updateBusInterfacePosition(port->name(), port->pos());
    }
    else
    {
        getComponentInstance()->updateAdHocPortPosition(port->name(), port->pos());
    }

    updateSize();
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::getBusInterfacePositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> HWComponentItem::getBusInterfacePositions() const
{
    return getComponentInstance()->getBusInterfacePositions();
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::getAdHocPortPositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> HWComponentItem::getAdHocPortPositions() const
{
    return getComponentInstance()->getAdHocPortPositions();
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::setPackaged()
//-----------------------------------------------------------------------------
void HWComponentItem::setPackaged()
{
    getComponentInstance()->setDraft(false);

    updateComponent();
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::setDraft()
//-----------------------------------------------------------------------------
void HWComponentItem::setDraft()
{
    getComponentInstance()->setDraft(true);

    updateComponent();
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::isDraft()
//-----------------------------------------------------------------------------
bool HWComponentItem::isDraft() const
{
    return getComponentInstance()->isDraft();
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void HWComponentItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    ComponentItem::mousePressEvent(event);
    setZValue(1001.0);

    oldPos_ = scenePos();
    oldColumn_ = dynamic_cast<HWColumn*>(parentItem());

    // Begin the position update for all connections.
    for (QGraphicsItem *item : scene()->items())
    {
        auto conn = dynamic_cast<GraphicsConnection*>(item);

        if (conn != nullptr)
        {
            conn->beginUpdatePosition();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::mouseMoveEvent()
//-----------------------------------------------------------------------------
void HWComponentItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Discard movement if the diagram is protected.
    if (auto diagram = dynamic_cast<DesignDiagram*>(scene()); diagram == nullptr || diagram->isProtected())
    {
        return;
    }
    
    ComponentItem::mouseMoveEvent(event);

    if (oldColumn_ != nullptr)
    {
        setPos(parentItem()->mapFromScene(oldColumn_->mapToScene(pos())));

        auto column = dynamic_cast<HWColumn*>(parentItem());
        Q_ASSERT(column != nullptr);
        column->onMoveItem(this);
    }
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void HWComponentItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    ComponentItem::mouseReleaseEvent(event);
    setZValue(0.0);

    auto diagram = dynamic_cast<DesignDiagram*>(scene());
    if (diagram && oldColumn_ != nullptr && scenePos() != oldPos_)
    {
        auto column = dynamic_cast<HWColumn*>(parentItem());
        Q_ASSERT(column != nullptr);
        column->onReleaseItem(this);

        QSharedPointer<QUndoCommand> cmd(new ComponentItemMoveCommand(this, oldPos_, oldColumn_, diagram));

        // End the position update for all connections.
        for (QGraphicsItem *item : scene()->items())
        {
            auto conn = dynamic_cast<GraphicsConnection*>(item);

            if (conn != nullptr)
            {
                conn->endUpdatePosition(cmd.data());
            }
        }

        diagram->getEditProvider()->addCommand(cmd);
        cmd->redo();

        oldColumn_ = nullptr;
    }
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::positionBusInterfaceTerminals()
//-----------------------------------------------------------------------------
void HWComponentItem::positionBusInterfaceTerminals()
{
    QMap<QString, QPointF> instancePositions = getComponentInstance()->getBusInterfacePositions();

    for (QSharedPointer<BusInterface> busInterface : *componentModel()->getBusInterfaces())
    {
        auto port = new ActiveBusInterfaceItem(busInterface, getLibraryInterface(), this);

        // Check if instance specific position has been specified.
        if (instancePositions.contains(busInterface->name()))
        {
            port->setPos(instancePositions.value(busInterface->name()));
            addPortToSideByPosition(port);
        }
        // Check if the default position has been specified.
        else if (!busInterface->getDefaultPos().isNull())
        {
            port->setPos(busInterface->getDefaultPos());
            addPortToSideByPosition(port);
        }
        else
        {
            addPortToSideWithLessPorts(port);
        }
    }

    if (isDraft())
    {
        for (QString const& interfaceName : instancePositions.keys())
        {
            QSharedPointer<BusInterface> busInterface(new BusInterface());
            busInterface->setName(interfaceName);
            componentModel()->getBusInterfaces()->append(busInterface);

            auto port = new ActiveBusInterfaceItem(busInterface, getLibraryInterface(), this);

            port->setPos(instancePositions.value(interfaceName));
            port->setTemporary(true);
            addPortToSideByPosition(port);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::positionAdHocPortTerminals()
//-----------------------------------------------------------------------------
void HWComponentItem::positionAdHocPortTerminals()
{
    QMap<QString, QPointF> instancePositions = getComponentInstance()->getAdHocPortPositions();

    for(auto it = instancePositions.cbegin(); it != instancePositions.cend(); ++it)
    {
        auto const& portName = it.key();

        QSharedPointer<Port> adhocPort = componentModel()->getPort(portName);
        if (!adhocPort)
        {
            adhocPort = QSharedPointer<Port>(new Port(portName));
        }

        auto portItem = new ActivePortItem(adhocPort, this);
        portItem->setPos(it.value());

        addPortToSideByPosition(portItem);
    }

    for (QSharedPointer<Port> adhocPort : *componentModel()->getPorts())
    {
        if (adhocPort->isAdHocVisible() && !instancePositions.contains(adhocPort->name()))
        {
            auto adhocItem (new ActivePortItem(adhocPort, this));

            addPortToSideWithLessPorts(adhocItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::createAdhocItem()
//-----------------------------------------------------------------------------
AdHocItem* HWComponentItem::createAdhocItem(QString const& portName)
{
    QSharedPointer<Port> adhocPort = componentModel()->getPort(portName);
    if (!adhocPort)
    {
        adhocPort = QSharedPointer<Port>(new Port(portName));
    }

    ActivePortItem* portItem = getAdHocPort(portName);
    if (!portItem)
    {
        portItem = new ActivePortItem(adhocPort, this);
        addPortToSideWithLessPorts(portItem);

        updateSize();
    }

    return portItem;
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::showAdhocPort()
//-----------------------------------------------------------------------------
void HWComponentItem::showAdhocPort(AdHocItem* portItem)
{
    auto adhocPortItem = dynamic_cast<ActivePortItem*>(portItem);
    if (adhocPortItem)
    {
        getComponentInstance()->updateAdHocPortPosition(adhocPortItem->name(), adhocPortItem->pos());

        updateSize();

        emit adHocVisibilitiesChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::hideAdhocPort()
//-----------------------------------------------------------------------------
void HWComponentItem::hideAdhocPort(AdHocItem* portItem)
{
    auto adhocPortItem = dynamic_cast<ActivePortItem*>(portItem);
    if (adhocPortItem)
    {
        removePort(adhocPortItem);

        getComponentInstance()->hideAdHocPort(adhocPortItem->name());

        emit adHocVisibilitiesChanged();
    }
}
