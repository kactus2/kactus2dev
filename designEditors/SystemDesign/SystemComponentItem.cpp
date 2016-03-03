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

#include <common/layouts/VCollisionLayout.h>

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/Design/ComponentInstance.h>

#include <IPXACTmodels/kactusExtensions/ComInterface.h>
#include <IPXACTmodels/kactusExtensions/ApiInterface.h>
#include <IPXACTmodels/kactusExtensions/ComProperty.h>

//-----------------------------------------------------------------------------
// Function: SystemComponentItem()
//-----------------------------------------------------------------------------
SystemComponentItem::SystemComponentItem(QRectF const& size, LibraryInterface* libInterface,
                                         QSharedPointer<ComponentInstance> instance,
										 QSharedPointer<Component> component,
										 QGraphicsItem *parent):
ComponentItem(QRectF(), libInterface, instance, component, parent),
imported_(false),
importRef_(),
portLayout_(new VCollisionLayout<SWPortItem>(SPACING)),
connUpdateDisabled_(false)
{
    setRect(size);

    positionAPIInterfaceTerminals();

    positionCOMInterfaceTerminals();
}

//-----------------------------------------------------------------------------
// Function: ~SystemComponentItem()
//-----------------------------------------------------------------------------
SystemComponentItem::~SystemComponentItem()
{

}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::positionAPIInterfaceTerminals()
//-----------------------------------------------------------------------------
void SystemComponentItem::positionAPIInterfaceTerminals()
{
    QMap<QString, QPointF> apiInterfacePositions = getComponentInstance()->getApiInterfacePositions();

    if (!getComponentInstance()->isDraft())
    {
        // Create graphics items for API interfaces.
        foreach (QSharedPointer<ApiInterface> apiIf, componentModel()->getApiInterfaces())
        {
            SWPortItem* port = new SWPortItem(apiIf, this);

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
        foreach (QString const& interfaceName, apiInterfacePositions.keys())
        {
            QSharedPointer<ApiInterface> apiInterface(new ApiInterface());
            apiInterface->setName(interfaceName);

            SWPortItem* port = new SWPortItem(apiInterface->name(), this);

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
    QMap<QString, QPointF> comInterfacePositions = getComponentInstance()->getComInterfacePositions();

    if (!getComponentInstance()->isDraft())
    {
        // Create graphics items for COM interfaces.
        foreach (QSharedPointer<ComInterface> comIf, componentModel()->getComInterfaces())
        {
            SWPortItem* port = new SWPortItem(comIf, this);

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
        foreach (QString const& interfaceName, comInterfacePositions.keys())
        {
            QSharedPointer<ComInterface> comInterface(new ComInterface());
            comInterface->setName(interfaceName);

            SWPortItem* port = new SWPortItem(comInterface->name(), this);

            port->setPos(comInterfacePositions.value(interfaceName));
            addPortToSideByPosition(port);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::addPortToSideByPosition()
//-----------------------------------------------------------------------------
void SystemComponentItem::addPortToSideByPosition(SWPortItem* port)
{
    if (port->pos().x() >= 0)
    {
        onAddPort(port, PORT_RIGHT);
    }
    else
    {
        onAddPort(port, PORT_LEFT);
    }
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::addPortToSideWithLessPorts()
//-----------------------------------------------------------------------------
void SystemComponentItem::addPortToSideWithLessPorts(SWPortItem* port)
{
    int portSpacing = GridSize * 3;
    int firstPortY = GridSize * 4;

    if (leftPorts_.size() <= rightPorts_.size())
    {
        if (!leftPorts_.isEmpty())
        {
            port->setPos(QPointF(0, leftPorts_.last()->pos().y() + portSpacing) + rect().topLeft());
        }
        else
        {
            port->setPos(QPointF(0, firstPortY) + rect().topLeft());
        }

        onAddPort(port, PORT_LEFT);
    }
    else
    {
        if (!rightPorts_.isEmpty())
        {
            port->setPos(QPointF(rect().width(), rightPorts_.last()->pos().y() + portSpacing) + rect().topLeft());
        }
        else
        {
            port->setPos(QPointF(rect().width(), firstPortY) + rect().topLeft());
        }

        onAddPort(port, PORT_RIGHT);
    }
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

    addPortToSideByPosition(port);

    getComponentInstance()->updateApiInterfacePosition(name, port->pos());

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

		checkPortLabelSize( port, leftPorts_ );
    }
    else if (dir == PORT_LEFT)
    {
        leftPorts_.append(port);
        portLayout_->updateItemMove(leftPorts_, port, MIN_Y_PLACEMENT);
        portLayout_->setItemPos(leftPorts_, port, rect().left(), MIN_Y_PLACEMENT);

		checkPortLabelSize( port, rightPorts_ );
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
// Function: SystemComponentItem::checkPortLabelSize()
//-----------------------------------------------------------------------------
void SystemComponentItem::checkPortLabelSize( SWPortItem* port, QList<SWPortItem*> otherSide )
{
	for ( int i = 0; i < otherSide.size(); ++i)
	{ 
		if (port->y() == otherSide.at(i)->y())
		{
			qreal portLabelWidth = port->getNameLength();
			qreal otherLabelWidth = otherSide.at(i)->getNameLength();

			// Check if both of the labels exceed the mid section of the component.
			if (portLabelWidth + SPACING * 2 > (ComponentItem::COMPONENTWIDTH / 2 ) &&
				otherLabelWidth + SPACING * 2 > (ComponentItem::COMPONENTWIDTH) / 2)
			{
				port->shortenNameLabel( ComponentItem::COMPONENTWIDTH / 2 );
				otherSide.at(i)->shortenNameLabel( ComponentItem::COMPONENTWIDTH / 2 );
			}

			// Check if the other port is wider than the other.
			else if (portLabelWidth > otherLabelWidth )
			{
				port->shortenNameLabel( ComponentItem::COMPONENTWIDTH - otherLabelWidth - SPACING * 2 );
			}

			else
			{
				otherSide.at(i)->shortenNameLabel( ComponentItem::COMPONENTWIDTH - portLabelWidth - SPACING * 2 );
			}				

			return;
		} 
	}

	// If the port gets here, there is no ports with the same y() value, and so the port name is restored.
	port->shortenNameLabel( ComponentItem::COMPONENTWIDTH );
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::getHeight()
//-----------------------------------------------------------------------------
qreal SystemComponentItem::getHeight()
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
    foreach (QGraphicsItem *item, QGraphicsRectItem::childItems())
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
// Function: SystemComponentItem::getSWPortWithTypeAndDirection()
//-----------------------------------------------------------------------------
SWPortItem* SystemComponentItem::getSWPortMatchingOtherEndPoint(ConnectionEndpoint* otherEndPoint) const
{
    foreach (QGraphicsItem* item, QGraphicsRectItem::childItems())
    {
        if (item->type() == SWPortItem::Type)
        {
            SWPortItem* port = dynamic_cast<SWPortItem*>(item);
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
        if (port->y() < minY)
        {
            port->setPos(port->x(), port->y() + offset);
        }
    }

    foreach (SWPortItem* port, rightPorts_)
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
    foreach (QSharedPointer<ComProperty const> prop, *componentModel()->getSWProperties())
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
        if (port == 0)
        {
            if (!createMissing || componentModel()->getVlnv().isValid())
            {
                continue;
            }

            port = new SWPortItem(itrPortPos.key(), this);
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
        if (port == 0)
        {
            if (!createMissing || componentModel()->getVlnv().isValid())
            {
                continue;
            }

            port = new SWPortItem(itrPortPos.key(), this);
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