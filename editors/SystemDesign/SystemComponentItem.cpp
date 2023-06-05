//-----------------------------------------------------------------------------
// File: SystemComponentItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 24.11.2011
//
// Description:
// Base class for all components visualized in a system design.
//-----------------------------------------------------------------------------

#include "SystemComponentItem.h"

#include "SWPortItem.h"

#include <common/layouts/VCollisionLayout.h>

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/Design/ComponentInstance.h>

#include <IPXACTmodels/kactusExtensions/ComInterface.h>
#include <IPXACTmodels/kactusExtensions/ApiInterface.h>
#include <IPXACTmodels/kactusExtensions/ComProperty.h>

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::SystemComponentItem()
//-----------------------------------------------------------------------------
SystemComponentItem::SystemComponentItem(QRectF const& size, LibraryInterface* libInterface,
                                         QSharedPointer<ComponentInstance> instance,
										 QSharedPointer<Component> component,
										 QGraphicsItem *parent):
ComponentItem(QRectF(), libInterface, instance, component, parent)
{
    setRect(size);

    positionAPIInterfaceTerminals();

    positionCOMInterfaceTerminals();
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::positionAPIInterfaceTerminals()
//-----------------------------------------------------------------------------
void SystemComponentItem::positionAPIInterfaceTerminals()
{
    auto apiInterfacePositions = getComponentInstance()->getApiInterfacePositions();

    if (!getComponentInstance()->isDraft())
    {
        // Create graphics items for API interfaces.
        for (QSharedPointer<ApiInterface> apiIf : componentModel()->getApiInterfaces())
        {
            auto port = new SWPortItem(apiIf, componentModel(), this);

            // Check if the default position has been specified.
            if (!apiIf->getDefaultPos().isNull())
            {
                port->setPos(apiIf->getDefaultPos());

                addPortToSideByPosition(port);
            }
            else if (apiInterfacePositions.contains(apiIf->name()))
            {
                port->setPos(apiInterfacePositions.value(apiIf->name()));
                addPortToSideByPosition(port);
            }
            else
            {
                addPortToSideWithLessPorts(port);
            }
        }
    }
    else
    {
        for (QString const& interfaceName : apiInterfacePositions.keys())
        {
            QSharedPointer<ApiInterface> apiInterface(new ApiInterface());
            apiInterface->setName(interfaceName);

            auto port = new SWPortItem(apiInterface->name(), componentModel(), this);

            port->setPos(apiInterfacePositions.value(interfaceName));
            addPortToSideByPosition(port);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::positionCOMInterfaceTerminals()
//-----------------------------------------------------------------------------
void SystemComponentItem::positionCOMInterfaceTerminals()
{
    auto comInterfacePositions = getComponentInstance()->getComInterfacePositions();

    if (!getComponentInstance()->isDraft())
    {
        // Create graphics items for COM interfaces.
        for (QSharedPointer<ComInterface> comIf : componentModel()->getComInterfaces())
        {
            auto port = new SWPortItem(comIf, componentModel(), this);

            // Check if the default position has been specified.
            if (!comIf->getDefaultPos().isNull())
            {
                port->setPos(comIf->getDefaultPos());
                addPortToSideByPosition(port);
            }
            else if (comInterfacePositions.contains(comIf->name()))
            {
                port->setPos(comInterfacePositions.value(comIf->name()));
                addPortToSideByPosition(port);
            }
            else
            {
                addPortToSideWithLessPorts(port);
            }
        }
    }
    else
    {
        for (QString const& interfaceName : comInterfacePositions.keys())
        {
            QSharedPointer<ComInterface> comInterface(new ComInterface());
            comInterface->setName(interfaceName);

            auto port = new SWPortItem(comInterface->name(), componentModel(), this);

            port->setPos(comInterfacePositions.value(interfaceName));
            addPortToSideByPosition(port);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::addPort()
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
    SWPortItem* port = new SWPortItem(name, componentModel(), this);

    port->setName(name);
    port->setPos(mapFromScene(pos));

    addPortToSideByPosition(port);

    getComponentInstance()->updateApiInterfacePosition(name, port->pos());

    // Update the component size.
    updateSize();
    return port;
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::addPort()
//-----------------------------------------------------------------------------
void SystemComponentItem::addPort(SWPortItem* port)
{
    port->setParentItem(this);

    if (port->getType() == SWPortItem::ENDPOINT_TYPE_API)
    {
        if (!getComponentInstance()->isDraft())
        {
            QList<QSharedPointer<ApiInterface> > apiInterfaces = componentModel()->getApiInterfaces();
            apiInterfaces.append(port->getApiInterface());
            componentModel()->setApiInterfaces(apiInterfaces);
        }

        getComponentInstance()->updateApiInterfacePosition(port->name(), port->pos());
    }
    else if (port->getType() == SWPortItem::ENDPOINT_TYPE_COM)
    {
        if (!getComponentInstance()->isDraft())
        {
            QList<QSharedPointer<ComInterface> > comInterfaces = componentModel()->getComInterfaces();
            comInterfaces.append(port->getComInterface());
            componentModel()->setComInterfaces(comInterfaces);
        }

        getComponentInstance()->updateComInterfacePosition(port->name(), port->pos());
    }

    // Make preparations.
    addPortToSideByPosition(port);

    // Update the component size.
    updateSize();
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::removePort()
//-----------------------------------------------------------------------------
void SystemComponentItem::removePort(SWPortItem* port)
{
    leftPorts_.removeAll(port);
    rightPorts_.removeAll(port);
    
	updateSize();

    if (port->getType() == SWPortItem::ENDPOINT_TYPE_API)
    {
        componentModel()->getVendorExtensions()->removeOne(port->getApiInterface());
    }
    else if (port->getType() == SWPortItem::ENDPOINT_TYPE_COM)
    {
        componentModel()->getVendorExtensions()->removeOne(port->getComInterface());
    }
}

//-----------------------------------------------------------------------------
// Function: setImported()
//-----------------------------------------------------------------------------
void SystemComponentItem::setImported(bool imported)
{
    getComponentInstance()->setImported(imported);
    updateComponent();
}

//-----------------------------------------------------------------------------
// Function: isImported()
//-----------------------------------------------------------------------------
bool SystemComponentItem::isImported() const
{
    return getComponentInstance()->isImported();
}

//-----------------------------------------------------------------------------
// Function: onMovePort()
//-----------------------------------------------------------------------------
void SystemComponentItem::onMovePort(ConnectionEndpoint* port)
{
    // Remove the port from the stacks (this simplifies code).
    leftPorts_.removeAll(port);
    rightPorts_.removeAll(port);
    
    // Restrict the position so that the port cannot be placed too high.
    port->setPos(snapPointToGrid(port->x(), qMax(MIN_Y_PLACEMENT - port->boundingRect().top(), port->y())));

    if (port->x() < 0.0)
    {
        portLayout_->updateItemMove(leftPorts_, port, MIN_Y_PLACEMENT);

		checkPortLabelSize( port, rightPorts_ );
    }
    else
    {
        portLayout_->updateItemMove(rightPorts_, port, MIN_Y_PLACEMENT);

		checkPortLabelSize( port, leftPorts_ );
    }

    QMap<QString, QPointF> comPositions = getComponentInstance()->getComInterfacePositions();
    QMap<QString, QPointF> apiPositions = getComponentInstance()->getApiInterfacePositions();
    if (port->getComInterface() ||
        (port->getType() == ConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED && comPositions.contains(port->name())))
    {
        getComponentInstance()->updateComInterfacePosition(port->name(), port->pos());
    }
    else if (port->getApiInterface() ||
            (port->getType() == ConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED && apiPositions.contains(port->name())))
    {
        getComponentInstance()->updateApiInterfacePosition(port->name(), port->pos());
    }

    updateSize();
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
    for (QGraphicsItem *item : QGraphicsRectItem::childItems())
    {
        if (item->type() == SWPortItem::Type)
        {
            auto port = static_cast<SWPortItem*>(item);

            if (port->name() == name &&
                (port->getType() == type || port->getType() == SWConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED ||
                 type == SWConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED))
            {
                return port;
            }
        }
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::getSWPortWithTypeAndDirection()
//-----------------------------------------------------------------------------
SWPortItem* SystemComponentItem::getSWPortMatchingOtherEndPoint(ConnectionEndpoint* otherEndPoint) const
{
    for (QGraphicsItem* item : QGraphicsRectItem::childItems())
    {
        if (item->type() == SWPortItem::Type)
        {
            auto port = dynamic_cast<SWPortItem*>(item);
            if (port && port->getType() == otherEndPoint->getType())
            {
                if ((port->getType() == ConnectionEndpoint::ENDPOINT_TYPE_COM &&
                    port->getComInterface() && otherEndPoint->getComInterface() &&
                    port->getComInterface()->getComType() == otherEndPoint->getComInterface()->getComType() &&
                    port->getComInterface()->getDirection() == otherEndPoint->getComInterface()->getDirection()) ||
                    (port->getType() == ConnectionEndpoint::ENDPOINT_TYPE_API &&
                    port->getApiInterface() && otherEndPoint->getApiInterface() &&
                    port->getApiInterface()->getApiType() == otherEndPoint->getApiInterface()->getApiType() &&
                    port->getApiInterface()->getDependencyDirection() ==
                    otherEndPoint->getApiInterface()->getDependencyDirection()) ||
                    (port->getType() == ConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED))
                {
                    return port;
                }
            }
        }
    }

    return nullptr;
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

    for (ConnectionEndpoint* port : leftPorts_)
    {
        if (port->y() < minY)
        {
            port->setPos(port->x(), port->y() + offset);
        }
    }

    for (ConnectionEndpoint* port : rightPorts_)
    {
        if (port->y() < minY)
        {
            port->setPos(port->x(), port->y() + offset);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::setPropertyValues()
//-----------------------------------------------------------------------------
void SystemComponentItem::setPropertyValues(QMap<QString, QString> const& values)
{
    propertyValues_ = values;

    // Check that at least all required properties are found in the values.
    for (QSharedPointer<ComProperty const> prop : *componentModel()->getSWProperties())
    {
        if (prop->isRequired() && !propertyValues_.contains(prop->name()))
        {
            propertyValues_.insert(prop->name(), prop->getDefaultValue());
        }
    }

    emit propertyValuesChanged(propertyValues_);
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::getPropertyValues()
//-----------------------------------------------------------------------------
QMap<QString, QString> SystemComponentItem::getPropertyValues() const
{
    return getComponentInstance()->getPropertyValues();
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::setImportRef()
//-----------------------------------------------------------------------------
void SystemComponentItem::setImportRef(QString const& nameRef)
{
    getComponentInstance()->setImportRef(nameRef);
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::getImportRef()
//-----------------------------------------------------------------------------
QString SystemComponentItem::getImportRef() const
{
    return getComponentInstance()->getImportRef();
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::revalidateConnections()
//-----------------------------------------------------------------------------
void SystemComponentItem::revalidateConnections()
{
    for (ConnectionEndpoint* endpoint : getEndpoints())
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

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::setApiInterfacePositions()
//-----------------------------------------------------------------------------
void SystemComponentItem::setApiInterfacePositions(QMap<QString, QPointF> const& positions, bool createMissing /*= false*/)
{
    QMapIterator<QString, QPointF> itrPortPos(positions);

    while (itrPortPos.hasNext())
    {
        itrPortPos.next();
        SWPortItem* port = getSWPort(itrPortPos.key(), SWConnectionEndpoint::ENDPOINT_TYPE_API);

        // If the port was not found, create it.
        if (port == nullptr)
        {
            if (!createMissing || componentModel()->getVlnv().isValid())
            {
                continue;
            }

            port = new SWPortItem(itrPortPos.key(), componentModel(), this);
            addPort(port);
        }

        port->setPos(itrPortPos.value());
        onMovePort(port);
    }
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::setComInterfacePositions()
//-----------------------------------------------------------------------------
void SystemComponentItem::setComInterfacePositions(QMap<QString, QPointF> const& positions, bool createMissing /*= false*/)
{
    QMapIterator<QString, QPointF> itrPortPos(positions);

    while (itrPortPos.hasNext())
    {
        itrPortPos.next();
        SWPortItem* port = getSWPort(itrPortPos.key(), SWConnectionEndpoint::ENDPOINT_TYPE_COM);

        // If the port was not found, create it.
        if (port == nullptr)
        {
            if (!createMissing || componentModel()->getVlnv().isValid())
            {
                continue;
            }

            port = new SWPortItem(itrPortPos.key(), componentModel(), this);
            addPort(port);
        }

        port->setPos(itrPortPos.value());
        onMovePort(port);
    }
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::getApiInterfacePositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> SystemComponentItem::getApiInterfacePositions() const
{
    QMap<QString, QPointF> positions;
    QListIterator< QSharedPointer<ApiInterface> > itrApiIf(componentModel()->getApiInterfaces());

    while (itrApiIf.hasNext())
    {
        QSharedPointer<ApiInterface> apiIf = itrApiIf.next();
        positions[apiIf->name()] = getSWPort(apiIf->name(), SWConnectionEndpoint::ENDPOINT_TYPE_API)->pos();
    }

    return positions;
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::getComInterfacePositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> SystemComponentItem::getComInterfacePositions() const
{
    QMap<QString, QPointF> positions;
    QListIterator< QSharedPointer<ComInterface> > itrComIf(componentModel()->getComInterfaces());

    while (itrComIf.hasNext())
    {
        QSharedPointer<ComInterface> comIf = itrComIf.next();
        positions[comIf->name()] = getSWPort(comIf->name(), SWConnectionEndpoint::ENDPOINT_TYPE_COM)->pos();
    }

    return positions;
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::setPacketized()
//-----------------------------------------------------------------------------
void SystemComponentItem::setPackaged()
{
    // Nothing to do here.
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::setDraft()
//-----------------------------------------------------------------------------
void SystemComponentItem::setDraft()
{
    // Nothing to do here.
}