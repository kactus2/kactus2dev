//-----------------------------------------------------------------------------
// File: HWComponentItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date:
//
// Description:
// HWComponentItem represents graphically an IP-XACT component instance.
//-----------------------------------------------------------------------------

#include "HWComponentItem.h"
#include "BusPortItem.h"
#include "AdHocPortItem.h"
#include "HWMoveCommands.h"

#include "columnview/HWColumn.h"

#include <library/LibraryManager/libraryinterface.h>

#include <common/IEditProvider.h>
#include <common/KactusColors.h>
#include <common/layouts/VCollisionLayout.h>

#include <common/graphicsItems/CommonGraphicsUndoCommands.h>
#include <common/graphicsItems/GraphicsConnection.h>

#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/DesignDiagram.h>
#include <designEditors/HWDesign/AdHocItem.h>
#include <designEditors/HWDesign/AdHocVisibilityEditor/AdHocVisibilityEditor.h>
#include <designEditors/HWDesign/undoCommands/ComponentItemMoveCommand.h>

#include <IPXACTmodels/Design/ComponentInstance.h>


#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>

#include <IPXACTmodels/common/VendorExtension.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>

#include <QGraphicsDropShadowEffect>
#include <QFont>
#include <QTextDocument>

//-----------------------------------------------------------------------------
// Function: HWComponentItem::HWComponentItem()
//-----------------------------------------------------------------------------
HWComponentItem::HWComponentItem(LibraryInterface* libInterface,
QSharedPointer<ComponentInstance> instance, QSharedPointer<Component> component,
QGraphicsItem* parent) : 
ComponentItem(QRectF(), libInterface, instance, component, parent),
hierIcon_(new QGraphicsPixmapItem(QPixmap(":icons/common/graphics/hierarchy.png"), this)),
oldColumn_(0),
portLayout_(new VCollisionLayout<HWConnectionEndpoint>(SPACING)),
leftPorts_(),
rightPorts_(),
oldPos_()
{
    setFlag(ItemIsMovable);
    setAdHocData(component, instance->getPortAdHocVisibilities());

    hierIcon_->setToolTip(tr("Hierarchical"));
    hierIcon_->setPos(COMPONENTWIDTH/2 - hierIcon_->pixmap().width() - SPACING, SPACING);

    int portSpacing = 3*GridSize;
    int portCountOnLeft = component->getBusInterfaces()->size() / 2.0 + .5;
    setRect(-COMPONENTWIDTH/ 2, 0, COMPONENTWIDTH, 6 * GridSize + portSpacing * qMax(portCountOnLeft - 1, 0));

    positionBusInterfaceTerminals();

    positionAdHocPortTerminals();

    updateSize();
    updateComponent();
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::~HWComponentItem()
//-----------------------------------------------------------------------------
HWComponentItem::~HWComponentItem()
{
    // Remove all interconnections.
    foreach (QGraphicsItem *item, childItems())
    {
        if (item->type() == BusPortItem::Type)
        {
            BusPortItem *diagramPort = qgraphicsitem_cast<BusPortItem *>(item);
            foreach (GraphicsConnection *interconnection, diagramPort->getConnections())
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

    VLNV vlnv = componentModel()->getVlnv();

    // Check whether the component is packaged (valid vlnv) or not.
    if (isDraft())
    {
        setBrush(QBrush(KactusColors::DRAFT_COMPONENT));
    }
    else if (getLibraryInterface()->contains(vlnv))
    {
        if (componentModel()->isBus())
        {
            setBrush(QBrush(KactusColors::HW_BUS_COMPONENT));
        }
        else
        {
            setBrush(QBrush(KactusColors::HW_COMPONENT));
        }
    }
    else
    {
        setBrush(QBrush(KactusColors::MISSING_COMPONENT));
    }

    // Show a hierarchy icon if the component is a hierarchical one.
    hierIcon_->setVisible(componentModel()->isHierarchical());
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::getHeight()
//-----------------------------------------------------------------------------
qreal HWComponentItem::getHeight()
{
    // Update the component's size based on the port that is positioned at the lowest level of them all.
    qreal maxY = 4 * GridSize;

    if (!leftPorts_.empty())
    {
        maxY = leftPorts_.back()->y();
    }

    if (!rightPorts_.empty())
    {
        maxY = qMax(maxY, rightPorts_.back()->y());
    }

    return (maxY + BOTTOM_MARGIN);
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::setPortAdHocVisible()
//-----------------------------------------------------------------------------
void HWComponentItem::onAdHocVisibilityChanged(QString const& portName, bool visible)
{
    // Create/destroy the ad-hoc port graphics item.
    if (visible)
    {
        QSharedPointer<Port> adhocPort = componentModel()->getPort(portName);
        if (!adhocPort)
        {
            adhocPort = QSharedPointer<Port>(new Port(portName));
        }

        AdHocPortItem* port = getAdHocPort(portName);
        if (!port)
        {
            port = new AdHocPortItem(adhocPort, this);
            addPortToSideWithLessPorts(port);
        }
        
        getComponentInstance()->updateAdHocPortPosition(portName, port->pos());

        // Update the component's size after addition.
        updateSize();
    }
    else
    {
        // Search for the ad-hoc port from both sides.
        HWConnectionEndpoint* found = getAdHocPort(portName);
        Q_ASSERT(found != 0);

        // Remove the port and delete it.
        removePort(found);
        delete found;
        found = 0;

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
    connect(this, SIGNAL(destroyed(ComponentItem*)), editor, SLOT(clear()), Qt::UniqueConnection);
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
BusPortItem* HWComponentItem::addPort(QPointF const& pos)
{
    // Determine a unique name for the bus interface.
    QString name = "bus";
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
    BusPortItem* draftPort = new BusPortItem(busIf, getLibraryInterface(), this);
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

    if (port->type() == BusPortItem::Type)
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

    if (port->type() == BusPortItem::Type)
    {
        componentModel()->getBusInterfaces()->removeOne(port->getBusInterface());
        getComponentInstance()->removeBusInterfacePosition(port->name());
    }
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::getBusPort()
//-----------------------------------------------------------------------------
BusPortItem* HWComponentItem::getBusPort(QString const& name) const
{
    foreach (QGraphicsItem* item, QGraphicsRectItem::childItems())
    {
        if (item->type() == BusPortItem::Type)
        {
            BusPortItem* busPort = qgraphicsitem_cast<BusPortItem*>(item);
            if (busPort->name() == name)
            {
                return busPort;
            }
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::getAdHocPort()
//-----------------------------------------------------------------------------
AdHocPortItem* HWComponentItem::getAdHocPort(QString const& portName) const
{
    QList<HWConnectionEndpoint*> allInterfaces;
    allInterfaces.append(leftPorts_);
    allInterfaces.append(rightPorts_);

    foreach (HWConnectionEndpoint* endpoint, allInterfaces)
    {
        if (dynamic_cast<AdHocPortItem*>(endpoint) != 0 && endpoint->name() == portName)
        {
            return static_cast<AdHocPortItem*>(endpoint);
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::onMovePort()
//-----------------------------------------------------------------------------
void HWComponentItem::onMovePort(HWConnectionEndpoint* port)
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
    foreach (QSharedPointer<VendorExtension> extension, *getComponentInstance()->getVendorExtensions())
    {
        if (extension->type() == "kactus2:draft")
        {
            getComponentInstance()->getVendorExtensions()->removeAll(extension);
        }
    }

    updateComponent();
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::setDraft()
//-----------------------------------------------------------------------------
void HWComponentItem::setDraft()
{
    if (!isDraft())
    {
        getComponentInstance()->getVendorExtensions()->append(
            QSharedPointer<Kactus2Placeholder>(new Kactus2Placeholder("kactus2:draft")));
    }

    updateComponent();
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::isDraft()
//-----------------------------------------------------------------------------
bool HWComponentItem::isDraft()
{
    foreach(QSharedPointer<VendorExtension> extension, *getComponentInstance()->getVendorExtensions())
    {
        if (extension->type() == "kactus2:draft")
        {
            return true;
        }
    }

    return false;
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
// Function: HWComponentItem::mouseMoveEvent()
//-----------------------------------------------------------------------------
void HWComponentItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Discard movement if the diagram is protected.
    DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(scene());
    if (diagram == 0 || diagram->isProtected())
    {
        return;
    }
    
    ComponentItem::mouseMoveEvent(event);
    
    if (oldColumn_ != 0)
    {
        setPos(parentItem()->mapFromScene(oldColumn_->mapToScene(pos())));

        HWColumn* column = dynamic_cast<HWColumn*>(parentItem());
        Q_ASSERT(column != 0);
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

    if (oldColumn_ != 0 && scenePos() != oldPos_)
    {
        HWColumn* column = dynamic_cast<HWColumn*>(parentItem());
        Q_ASSERT(column != 0);
        column->onReleaseItem(this);

        QSharedPointer<QUndoCommand> cmd(new ComponentItemMoveCommand(this, oldPos_, oldColumn_));
        
        // End the position update for all connections.
        foreach (QGraphicsItem *item, scene()->items())
        {
            GraphicsConnection* conn = dynamic_cast<GraphicsConnection*>(item);

            if (conn != 0)
            {
                conn->endUpdatePosition(cmd.data());
            }
        }

        static_cast<DesignDiagram*>(scene())->getEditProvider()->addCommand(cmd);

        oldColumn_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::addPortToSideByPosition()
//-----------------------------------------------------------------------------
void HWComponentItem::addPortToSideByPosition(HWConnectionEndpoint* port)
{
    if (port->pos().x() >= 0)
    {
        addPortToRight(port);
    }
    else
    {
        addPortToLeft(port);
    }
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::addPortToSideWithLessPorts()
//-----------------------------------------------------------------------------
void HWComponentItem::addPortToSideWithLessPorts(HWConnectionEndpoint* port)
{
    // Place the port at the bottom of the side that contains fewer ports.
    if (leftPorts_.size() < rightPorts_.size())
    {
        if (!leftPorts_.empty())
        {
            port->setPos(QPointF(0, leftPorts_.last()->pos().y() + GridSize * 3) + rect().topLeft());
        }
        else
        {
            port->setPos(QPointF(0, GridSize * 4) + rect().topLeft());
        }

        addPortToLeft(port);
    }
    else
    {
        if (!rightPorts_.empty())
        {
            port->setPos(QPointF(rect().width(), rightPorts_.last()->pos().y() + GridSize * 3) + rect().topLeft());
        }
        else
        {
            port->setPos(QPointF(rect().width(), GridSize * 4) + rect().topLeft());
        }

        addPortToRight(port);
    }
}

//-----------------------------------------------------------------------------
// Function: addPortToLeft()
//-----------------------------------------------------------------------------
void HWComponentItem::addPortToLeft(HWConnectionEndpoint* port)
{
    connect(port, SIGNAL(moved(ConnectionEndpoint*)), this, SIGNAL(endpointMoved(ConnectionEndpoint*)));

    leftPorts_.append(port);
    portLayout_->updateItemMove(leftPorts_, port, MIN_Y_PLACEMENT);		
    portLayout_->setItemPos(leftPorts_, port, rect().left(), MIN_Y_PLACEMENT);
    checkPortLabelSize(port, rightPorts_);
}

//-----------------------------------------------------------------------------
// Function: addPortToRight()
//-----------------------------------------------------------------------------
void HWComponentItem::addPortToRight(HWConnectionEndpoint* port)
{
    connect(port, SIGNAL(moved(ConnectionEndpoint*)), this, SIGNAL(endpointMoved(ConnectionEndpoint*)));

    rightPorts_.append(port);
    portLayout_->updateItemMove(rightPorts_, port, MIN_Y_PLACEMENT);
    portLayout_->setItemPos(rightPorts_, port, rect().right(), MIN_Y_PLACEMENT);
    checkPortLabelSize(port, leftPorts_);
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::checkPortLabelSize()
//-----------------------------------------------------------------------------
void HWComponentItem::checkPortLabelSize(HWConnectionEndpoint* port, QList<HWConnectionEndpoint*> otherSide)
{
	foreach (HWConnectionEndpoint* otherSidePort, otherSide)
	{ 
		if (port->y() == otherSidePort->y())
		{
			qreal portLabelWidth = port->getNameLength();
			qreal otherLabelWidth = otherSidePort->getNameLength();

			// Check if both of the labels exceed the mid section of the component.
		    if (portLabelWidth + 2*SPACING > (ComponentItem::COMPONENTWIDTH/2) &&
				otherLabelWidth + 2*SPACING > (ComponentItem::COMPONENTWIDTH)/2)
		    {
				port->shortenNameLabel( ComponentItem::COMPONENTWIDTH / 2 );
				otherSidePort->shortenNameLabel( ComponentItem::COMPONENTWIDTH / 2 );
		    }				
			// Check if the other port is wider than the other.
		    else if (portLabelWidth > otherLabelWidth )
		    {
				port->shortenNameLabel(ComponentItem::COMPONENTWIDTH - otherLabelWidth - SPACING * 2);
		    }

		    else
		    {
				otherSidePort->shortenNameLabel(ComponentItem::COMPONENTWIDTH - portLabelWidth - SPACING * 2);
		    }				
			
			return;
		} 
	}

	// If the port gets here, there is no ports with the same y() value, and so the port name is restored.
	port->shortenNameLabel(ComponentItem::COMPONENTWIDTH);
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::positionBusInterfaceTerminals()
//-----------------------------------------------------------------------------
void HWComponentItem::positionBusInterfaceTerminals()
{
    QMap<QString, QPointF> instancePositions = getComponentInstance()->getBusInterfacePositions();

    foreach (QSharedPointer<BusInterface> busInterface, *componentModel()->getBusInterfaces())
    {
        BusPortItem *port = new BusPortItem(busInterface, getLibraryInterface(), this);

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
        foreach (QString const& interfaceName, instancePositions.keys())
        {
            QSharedPointer<BusInterface> busInterface(new BusInterface());
            busInterface->setName(interfaceName);

            BusPortItem *port = new BusPortItem(busInterface, getLibraryInterface(), this);

            port->setPos(instancePositions.value(busInterface->name()));
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

    QMapIterator<QString, QPointF> positionIterator(instancePositions);
    while(positionIterator.hasNext())
    {
        positionIterator.next();

        QString portName = positionIterator.key();
        QPointF portPosition = positionIterator.value();

        QSharedPointer<Port> adhocPort = componentModel()->getPort(portName);
        if (!adhocPort)
        {
            adhocPort = QSharedPointer<Port>(new Port(portName));
        }

        AdHocPortItem* portItem = new AdHocPortItem(adhocPort, this);
        portItem->setPos(portPosition);

        addPortToSideByPosition(portItem);
    }

    foreach (QSharedPointer<Port> adhocPort, *componentModel()->getPorts())
    {
        if (adhocPort->isAdHocVisible() && !instancePositions.contains(adhocPort->name()))
        {
            AdHocPortItem* adhocItem (new AdHocPortItem(adhocPort, this));

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

    AdHocPortItem* portItem = getAdHocPort(portName);
    if (!portItem)
    {
        portItem = new AdHocPortItem(adhocPort, this);
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
    AdHocPortItem* adhocPortItem = dynamic_cast<AdHocPortItem*>(portItem);
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
    AdHocPortItem* adhocPortItem = dynamic_cast<AdHocPortItem*>(portItem);
    if (adhocPortItem)
    {
        removePort(adhocPortItem);

        getComponentInstance()->hideAdHocPort(adhocPortItem->name());

        emit adHocVisibilitiesChanged();
    }
}
