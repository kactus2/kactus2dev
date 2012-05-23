//-----------------------------------------------------------------------------
// File: SWComponentItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.11.2011
//
// Description:
// Base class for all SW components visualized in a system design.
//-----------------------------------------------------------------------------

#include "SWComponentItem.h"

#include "SWPortItem.h"
#include "IComponentStack.h"

#include <common/layouts/VCollisionLayout.h>

#include <models/component.h>
#include <models/ComInterface.h>
#include <models/ApiInterface.h>
#include <models/ComProperty.h>

//-----------------------------------------------------------------------------
// Function: SWComponentItem()
//-----------------------------------------------------------------------------
SWComponentItem::SWComponentItem(QRectF const& size,
                                 LibraryInterface* libInterface,
                                 QSharedPointer<Component> component,
                                 QString const& instanceName, QString const& displayName,
                                 QString const& description,
                                 QMap<QString, QString> const& configurableElementValues,
                                 QGraphicsItem *parent)
    : ComponentItem(size, libInterface, component, instanceName, displayName, description, configurableElementValues, parent),
      imported_(false),
      connUpdateDisabled_(false)
{
    int portSpacing = 3 * GridSize;
    int leftY = 4 * GridSize;
    int rightY = 4 * GridSize;
    bool right = false;

    // Create graphics items for API interfaces.
    foreach (QSharedPointer<ApiInterface> apiIf, component->getApiInterfaces())
    {
        SWPortItem* port = new SWPortItem(apiIf, this);
        connect(port, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

        if (right)
        {
            port->setPos(QPointF(rect().width(), rightY) + rect().topLeft());
            rightY += portSpacing;
        }
        else
        {
            port->setPos(QPointF(0, leftY) + rect().topLeft());
            leftY += portSpacing;
        }

        onAddPort(port, right);
        right = !right;
    }

    // Create graphics items for COM interfaces.
    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        SWPortItem* port = new SWPortItem(comIf, this);
        connect(port, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

        if (right)
        {
            port->setPos(QPointF(rect().width(), rightY) + rect().topLeft());
            rightY += portSpacing;
        }
        else
        {
            port->setPos(QPointF(0, leftY) + rect().topLeft());
            leftY += portSpacing;
        }

        onAddPort(port, right);
        right = !right;
    }
}

//-----------------------------------------------------------------------------
// Function: ~SWComponentItem()
//-----------------------------------------------------------------------------
SWComponentItem::~SWComponentItem()
{

}

//-----------------------------------------------------------------------------
// Function: addPort()
//-----------------------------------------------------------------------------
SWPortItem* SWComponentItem::addPort(QPointF const& pos)
{
    // Determine a unique name for the port.
    QString name = "interface";
    unsigned int count = 0;

    while (componentModel()->getBusInterface(name) != 0)
    {
        ++count;
        name = "interface_" + QString::number(count);
    }

    // Create an empty bus interface and add it to the component model.
//     QSharedPointer<BusInterface> busIf(new BusInterface());
//     busIf->setName(name);
//     busIf->setInterfaceMode(General::MODE_UNDEFINED);
//     componentModel()->addBusInterface(busIf);

    // Create the visualization for the bus interface.
    SWPortItem* port = new SWPortItem(name, this);
    port->setName(name);
    port->setPos(mapFromScene(pos));
    onAddPort(port, mapFromScene(pos).x() >= 0);

    // Update the component size.
    updateSize();
    return port;
}

//-----------------------------------------------------------------------------
// Function: addPort()
//-----------------------------------------------------------------------------
void SWComponentItem::addPort(SWPortItem* port)
{
    port->setParentItem(this);

//     if (port->type() == DiagramPort::Type)
//     {
//         // Add the bus interface to the component.
//         componentModel()->add(port->getBusInterface());
//     }

    // Make preparations.
    onAddPort(port, port->x() >= 0);

    // Update the component size.
    updateSize();
}

//-----------------------------------------------------------------------------
// Function: removePort()
//-----------------------------------------------------------------------------
void SWComponentItem::removePort(SWPortItem* port)
{
    leftPorts_.removeAll(port);
    rightPorts_.removeAll(port);
    updateSize();

//     if (port->type() == DiagramPort::Type)
//     {
//         componentModel()->removeBusInterface(port->getBusInterface().data());
//     }
}

//-----------------------------------------------------------------------------
// Function: updateComponent()
//-----------------------------------------------------------------------------
void SWComponentItem::updateComponent()
{
    ComponentItem::updateComponent();
}

//-----------------------------------------------------------------------------
// Function: setImported()
//-----------------------------------------------------------------------------
void SWComponentItem::setImported(bool imported)
{
    imported_ = imported;
    updateComponent();
}

//-----------------------------------------------------------------------------
// Function: isImported()
//-----------------------------------------------------------------------------
bool SWComponentItem::isImported() const
{
    return imported_;
}

//-----------------------------------------------------------------------------
// Function: onAddPort()
//-----------------------------------------------------------------------------
void SWComponentItem::onAddPort(SWPortItem* port, bool right)
{
    if (right)
    {
        rightPorts_.append(port);
        VCollisionLayout::updateItemMove(rightPorts_, port, MIN_Y_PLACEMENT, SPACING);
        VCollisionLayout::setItemPos(rightPorts_, port, rect().right(), MIN_Y_PLACEMENT, SPACING);
    }
    else
    {
        leftPorts_.append(port);
        VCollisionLayout::updateItemMove(leftPorts_, port, MIN_Y_PLACEMENT, SPACING);
        VCollisionLayout::setItemPos(leftPorts_, port, rect().left(), MIN_Y_PLACEMENT, SPACING);
    }
}

//-----------------------------------------------------------------------------
// Function: onMovePort()
//-----------------------------------------------------------------------------
void SWComponentItem::onMovePort(SWPortItem* port)
{
    // Remove the port from the stacks (this simplifies code).
    leftPorts_.removeAll(port);
    rightPorts_.removeAll(port);

    // Restrict the position so that the port cannot be placed too high.
    port->setPos(snapPointToGrid(port->x(), std::max(MIN_Y_PLACEMENT - port->boundingRect().top(), port->y())));

    // Check on which side the port is to determine the stack to which it should be placed.
    if (port->x() < 0.0)
    {
        VCollisionLayout::updateItemMove(leftPorts_, port, MIN_Y_PLACEMENT, SPACING);
    }
    else
    {
        VCollisionLayout::updateItemMove(rightPorts_, port, MIN_Y_PLACEMENT, SPACING);
    }

    updateSize();
}

//-----------------------------------------------------------------------------
// Function: SWComponentItem::updateSize()
//-----------------------------------------------------------------------------
void SWComponentItem::updateSize()
{
    QRectF oldRect = rect();
    oldRect.setHeight(getHeight());

    setRect(oldRect);

    IComponentStack* stack = dynamic_cast<IComponentStack*>(parentItem());

    if (stack != 0)
    {
        stack->updateItemPositions();
    }
}

//-----------------------------------------------------------------------------
// Function: SWComponentItem::getHeight()
//-----------------------------------------------------------------------------
qreal SWComponentItem::getHeight() const
{
    // Update the component's size based on the port that is positioned at
    // the lowest level of them all.
    qreal maxY = 4 * GridSize;

    if (!leftPorts_.empty())
    {
        maxY = leftPorts_.back()->y();
    }

    if (!rightPorts_.empty())
    {
        maxY = std::max(maxY, rightPorts_.back()->y());
    }

    return (maxY + BOTTOM_MARGIN);
}

//-----------------------------------------------------------------------------
// Function: SWComponentItem::isConnectionUpdateDisabled()
//-----------------------------------------------------------------------------
bool SWComponentItem::isConnectionUpdateDisabled() const
{
    return connUpdateDisabled_;
}

//-----------------------------------------------------------------------------
// Function: SWComponentItem::setConnectionUpdateDisabled()
//-----------------------------------------------------------------------------
void SWComponentItem::setConnectionUpdateDisabled(bool disabled)
{
    connUpdateDisabled_ = disabled;
}

//-----------------------------------------------------------------------------
// Function: SWComponentItem::getSWPort()
//-----------------------------------------------------------------------------
SWPortItem* SWComponentItem::getSWPort(QString const& name, SWConnectionEndpoint::EndpointType type) const
{
    foreach (QGraphicsItem *item, QGraphicsRectItem::children())
    {
        if (item->type() == SWPortItem::Type)
        {
            SWPortItem* port = static_cast<SWPortItem*>(item);

            if (port->name() == name &&
                (port->getType() == type || port->getType() == SWConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED))
            {
                return port;
            }
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: SWComponentItem::offsetPortPositions()
//-----------------------------------------------------------------------------
void SWComponentItem::offsetPortPositions(qreal minY)
{
    // Determine the smallest Y from the ports.
    int curMinY = MIN_Y_PLACEMENT;

    if (!leftPorts_.isEmpty())
    {
        curMinY = leftPorts_.first()->pos().y();
    }
    else if (!rightPorts_.isEmpty())
    {
        curMinY = rightPorts_.first()->pos().y();
    }

    qreal offset = minY - curMinY;

    foreach (SWPortItem* port, leftPorts_)
    {
        port->setPos(port->x(), port->y() + offset);
    }

    foreach (SWPortItem* port, rightPorts_)
    {
        port->setPos(port->x(), port->y() + offset);
    }
}

//-----------------------------------------------------------------------------
// Function: SWComponentItem::setPropertyValues()
//-----------------------------------------------------------------------------
void SWComponentItem::setPropertyValues(QMap<QString, QString> const& values)
{
    propertyValues_ = values;

    // Check that at least all required properties are found in the values.
    foreach (QSharedPointer<ComProperty const> prop, componentModel()->getSWProperties())
    {
        if (prop->isRequired() && !propertyValues_.contains(prop->getName()))
        {
            propertyValues_.insert(prop->getName(), prop->getDefaultValue());
        }
    }


    emit propertyValuesChanged(propertyValues_);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SWComponentItem::getPropertyValues()
//-----------------------------------------------------------------------------
QMap<QString, QString> const& SWComponentItem::getPropertyValues() const
{
    return propertyValues_;
}
