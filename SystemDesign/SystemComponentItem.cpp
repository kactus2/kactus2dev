//-----------------------------------------------------------------------------
// File: SystemComponentItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.11.2011
//
// Description:
// Base class for all components visualized in a system design.
//-----------------------------------------------------------------------------

#include "SystemComponentItem.h"

#include "SWPortItem.h"

#include <common/graphicsItems/IGraphicsItemStack.h>
#include <common/graphicsItems/GraphicsConnection.h>
#include <common/layouts/HCollisionLayout.h>
#include <common/layouts/VCollisionLayout.h>

#include <models/component.h>
#include <models/ComInterface.h>
#include <models/ApiInterface.h>
#include <models/ComProperty.h>

//-----------------------------------------------------------------------------
// Function: SystemComponentItem()
//-----------------------------------------------------------------------------
SystemComponentItem::SystemComponentItem(QRectF const& size,
                                 LibraryInterface* libInterface,
                                 QSharedPointer<Component> component,
                                 QString const& instanceName, QString const& displayName,
                                 QString const& description,
                                 QMap<QString, QString> const& configurableElementValues,
                                 QGraphicsItem *parent)
    : ComponentItem(size, libInterface, component, instanceName, displayName, description, configurableElementValues, parent),
      imported_(false),
      importRef_(),
      portLayout_(new VCollisionLayout<SWPortItem>(SPACING)),
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

        if (right)
        {
            onAddPort(port, PORT_RIGHT);
        }
        else
        {
            onAddPort(port, PORT_LEFT);
        }

        right = !right;
    }

    // Create graphics items for COM interfaces.
    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        SWPortItem* port = new SWPortItem(comIf, this);

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

        if (right)
        {
            onAddPort(port, PORT_RIGHT);
        }
        else
        {
            onAddPort(port, PORT_LEFT);
        }

        right = !right;
    }
}

//-----------------------------------------------------------------------------
// Function: ~SystemComponentItem()
//-----------------------------------------------------------------------------
SystemComponentItem::~SystemComponentItem()
{

}

//-----------------------------------------------------------------------------
// Function: addPort()
//-----------------------------------------------------------------------------
SWPortItem* SystemComponentItem::addPort(QPointF const& pos)
{
    // Determine a unique name for the port.
    QString name = "interface";
    unsigned int count = 0;

    while (getSWPort(name, SWConnectionEndpoint::ENDPOINT_TYPE_COM) != 0 ||
           getSWPort(name, SWConnectionEndpoint::ENDPOINT_TYPE_API) != 0)
    {
        ++count;
        name = "interface_" + QString::number(count);
    }

    // Create the visualization for the bus interface.
    SWPortItem* port = new SWPortItem(name, this);

    port->setName(name);
    port->setPos(mapFromScene(pos));

    if (mapFromScene(pos).x() >= 0)
    {
        onAddPort(port, PORT_RIGHT);
    }
    else
    {
        onAddPort(port, PORT_LEFT);
    }

    // Update the component size.
    updateSize();
    return port;
}

//-----------------------------------------------------------------------------
// Function: addPort()
//-----------------------------------------------------------------------------
void SystemComponentItem::addPort(SWPortItem* port)
{
    port->setParentItem(this);

    if (port->getType() == SWPortItem::ENDPOINT_TYPE_API)
    {
        componentModel()->addApiInterface(port->getApiInterface());
    }
    else if (port->getType() == SWPortItem::ENDPOINT_TYPE_COM)
    {
        componentModel()->addComInterface(port->getComInterface());
    }

    // Make preparations.
    if (port->x() >= 0)
    {
        onAddPort(port, PORT_RIGHT);
    }
    else
    {
        onAddPort(port, PORT_LEFT);
    }

    // Update the component size.
    updateSize();
}

//-----------------------------------------------------------------------------
// Function: removePort()
//-----------------------------------------------------------------------------
void SystemComponentItem::removePort(SWPortItem* port)
{
    leftPorts_.removeAll(port);
    rightPorts_.removeAll(port);
    bottomPorts_.removeAll(port);
    updateSize();

    if (port->getType() == SWPortItem::ENDPOINT_TYPE_API)
    {
        componentModel()->removeApiInterface(port->getApiInterface().data());
    }
    else if (port->getType() == SWPortItem::ENDPOINT_TYPE_COM)
    {
        componentModel()->removeComInterface(port->getComInterface().data());
    }
}

//-----------------------------------------------------------------------------
// Function: updateComponent()
//-----------------------------------------------------------------------------
void SystemComponentItem::updateComponent()
{
    ComponentItem::updateComponent();
}

//-----------------------------------------------------------------------------
// Function: setImported()
//-----------------------------------------------------------------------------
void SystemComponentItem::setImported(bool imported)
{
    imported_ = imported;
    updateComponent();
}

//-----------------------------------------------------------------------------
// Function: isImported()
//-----------------------------------------------------------------------------
bool SystemComponentItem::isImported() const
{
    return imported_;
}

//-----------------------------------------------------------------------------
// Function: onAddPort()
//-----------------------------------------------------------------------------
void SystemComponentItem::onAddPort(SWPortItem* port, PortDirection dir)
{
    if (dir == PORT_RIGHT)
    {
        rightPorts_.append(port);
        portLayout_->updateItemMove(rightPorts_, port, MIN_Y_PLACEMENT);
        portLayout_->setItemPos(rightPorts_, port, rect().right(), MIN_Y_PLACEMENT);
    }
    else if (dir == PORT_LEFT)
    {
        leftPorts_.append(port);
        portLayout_->updateItemMove(leftPorts_, port, MIN_Y_PLACEMENT);
        portLayout_->setItemPos(leftPorts_, port, rect().left(), MIN_Y_PLACEMENT);
    }
    else if (dir == PORT_BOTTOM)
    {
        bottomPorts_.append(port);
        HCollisionLayout::updateItemMove(bottomPorts_, port, 0.0, SPACING);
        HCollisionLayout::setItemPos(bottomPorts_, port, rect().bottom(), 0.0, SPACING);
    }
}

//-----------------------------------------------------------------------------
// Function: onMovePort()
//-----------------------------------------------------------------------------
void SystemComponentItem::onMovePort(SWPortItem* port)
{
    // Remove the port from the stacks (this simplifies code).
    leftPorts_.removeAll(port);
    rightPorts_.removeAll(port);
    bottomPorts_.removeAll(port);

    // Restrict the position so that the port cannot be placed too high.
    port->setPos(snapPointToGrid(port->x(), qMax(MIN_Y_PLACEMENT - port->boundingRect().top(), port->y())));

    // Check on which side the port is to determine the stack to which it should be placed.
    /*if (port->y() - rect().bottom() >= -35.0 && qAbs(port->x()) <= 50.0)
    {
        HCollisionLayout::updateItemMove(bottomPorts_, port, 0.0, SPACING);
    }
    else*/ if (port->x() < 0.0)
    {
        portLayout_->updateItemMove(leftPorts_, port, MIN_Y_PLACEMENT);
    }
    else
    {
        portLayout_->updateItemMove(rightPorts_, port, MIN_Y_PLACEMENT);
    }

    updateSize();
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::updateSize()
//-----------------------------------------------------------------------------
void SystemComponentItem::updateSize()
{
    QRectF oldRect = rect();
    oldRect.setHeight(getHeight());

    setRect(oldRect);

    IGraphicsItemStack* stack = dynamic_cast<IGraphicsItemStack*>(parentItem());

    if (stack != 0)
    {
        stack->updateItemPositions();
    }
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::getHeight()
//-----------------------------------------------------------------------------
qreal SystemComponentItem::getHeight() const
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
        maxY = qMax(maxY, rightPorts_.back()->y());
    }

    return (maxY + BOTTOM_MARGIN);
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::isConnectionUpdateDisabled()
//-----------------------------------------------------------------------------
bool SystemComponentItem::isConnectionUpdateDisabled() const
{
    return connUpdateDisabled_;
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::setConnectionUpdateDisabled()
//-----------------------------------------------------------------------------
void SystemComponentItem::setConnectionUpdateDisabled(bool disabled)
{
    connUpdateDisabled_ = disabled;
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::getSWPort()
//-----------------------------------------------------------------------------
SWPortItem* SystemComponentItem::getSWPort(QString const& name, SWConnectionEndpoint::EndpointType type) const
{
    foreach (QGraphicsItem *item, QGraphicsRectItem::children())
    {
        if (item->type() == SWPortItem::Type)
        {
            SWPortItem* port = static_cast<SWPortItem*>(item);

            if (port->name() == name &&
                (port->getType() == type || port->getType() == SWConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED ||
                 type == SWConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED))
            {
                return port;
            }
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::offsetPortPositions()
//-----------------------------------------------------------------------------
void SystemComponentItem::offsetPortPositions(qreal minY)
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

    foreach (SWPortItem* port, bottomPorts_)
    {
        port->setY(boundingRect().bottom());
    }
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::setPropertyValues()
//-----------------------------------------------------------------------------
void SystemComponentItem::setPropertyValues(QMap<QString, QString> const& values)
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
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::getPropertyValues()
//-----------------------------------------------------------------------------
QMap<QString, QString> const& SystemComponentItem::getPropertyValues() const
{
    return propertyValues_;
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::setImportRef()
//-----------------------------------------------------------------------------
void SystemComponentItem::setImportRef(QString const& nameRef)
{
    importRef_ = nameRef;
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::getImportRef()
//-----------------------------------------------------------------------------
QString const& SystemComponentItem::getImportRef() const
{
    return importRef_;
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::revalidateConnections()
//-----------------------------------------------------------------------------
void SystemComponentItem::revalidateConnections()
{
    foreach (ConnectionEndpoint* endpoint, getEndpoints())
    {
        endpoint->revalidateConnections();
    }
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::itemChange()
//-----------------------------------------------------------------------------
QVariant SystemComponentItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemParentHasChanged)
    {
        revalidateConnections();
    }

    return ComponentItem::itemChange(change, value);
}
