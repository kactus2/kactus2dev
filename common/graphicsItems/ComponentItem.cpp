//-----------------------------------------------------------------------------
// File: ComponentItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 6.11.2011
//
// Description:
// Base class for all graphics items that represent an IP-XACT component.
//-----------------------------------------------------------------------------

#include "ComponentItem.h"

#include "ConnectionEndpoint.h"
#include "IGraphicsItemStack.h"
#include "GraphicsConnection.h"

#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/Design/ComponentInstance.h>

#include <QFont>
#include <QPen>
#include <QTextDocument>

//-----------------------------------------------------------------------------
// Function: ComponentItem::ComponentItem()
//-----------------------------------------------------------------------------
ComponentItem::ComponentItem(QRectF const& size, LibraryInterface* libInterface,
    QSharedPointer<ComponentInstance> instance, QSharedPointer<Component> component,
    QGraphicsItem* parent) : 
QGraphicsRectItem(parent),
    libInterface_(libInterface),
    component_(component), 
    componentInstance_(instance),
    nameLabel_(new QGraphicsTextItem(instance->getInstanceName(), this))
{
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsScenePositionChanges);

    setRect(size);
    setPen(QPen(Qt::black, 0));

    // Create the name label.
    QFont font = nameLabel_->font();
    font.setWeight(QFont::Bold);

    nameLabel_->setFont(font);
    nameLabel_->setTextWidth(COMPONENTWIDTH - 20);
    nameLabel_->setPos(-nameLabel_->boundingRect().width()/2, GridSize);

    setPos(instance->getPosition());
}

//-----------------------------------------------------------------------------
// Function: ~ComponentItem()
//-----------------------------------------------------------------------------
ComponentItem::~ComponentItem()
{
    emit destroyed(this);
}

//-----------------------------------------------------------------------------
// Function: updateComponent()
//-----------------------------------------------------------------------------
void ComponentItem::updateComponent()
{
    if (!displayName().isEmpty())
    {
        updateNameLabel(displayName());
    }
    else
    {
        updateNameLabel(name());
    }

    VLNV vlnv = component_->getVlnv();

    QString toolTipText = "";
    if (!vlnv.isEmpty())
    {
        toolTipText += "<b>Vendor:</b> " + vlnv.getVendor() + "<br>" +
                       "<b>Library:</b> " + vlnv.getLibrary() + "<br>" +
                       "<b>Name:</b> " + vlnv.getName() + "<br>" +
                       "<b>Version:</b> " + vlnv.getVersion();
    }
    else
    {
        toolTipText += "Unpackaged component. No VLNV assigned!";
    }

    toolTipText += "<br><br><b>Instance name:</b> " + name();

    if (!description().isEmpty())
    {
        toolTipText += "<br><br><b>Description:</b><br>" + description();
    }

    setToolTip(toolTipText);
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::updateSize()
//-----------------------------------------------------------------------------
void ComponentItem::updateSize()
{
	QRectF oldRect = rect();
	oldRect.setHeight(getHeight());

	qreal width = getWidth();

	setRect(-width/2, oldRect.y(), width, oldRect.height());

	IGraphicsItemStack* stack = dynamic_cast<IGraphicsItemStack*>(parentItem());
	if (stack != 0)
	{
		stack->updateItemPositions();
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::getWidth()
//-----------------------------------------------------------------------------
qreal ComponentItem::getWidth()
{
	return COMPONENTWIDTH;
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::setName()
//-----------------------------------------------------------------------------
void ComponentItem::setName(QString const& newName)
{
    QString oldName = name();

    // Find all connections that are using the default naming and should be simultaneously renamed.
    QList<GraphicsConnection*> renamedConnections;
    foreach (ConnectionEndpoint* endpoint, getEndpoints())
    {
        foreach (GraphicsConnection* conn, endpoint->getConnections())
        {
            conn->changeConnectionComponentReference(oldName, newName);

            if (conn->hasDefaultName())
            {
                renamedConnections.append(conn);
            }
        }

        foreach (GraphicsConnection* conn, endpoint->getOffPageConnector()->getConnections())
        {
            conn->changeConnectionComponentReference(oldName, newName);

            if (conn->hasDefaultName())
            {
                renamedConnections.append(conn);
            }
        }
    }

    componentInstance_->setInstanceName(newName);

    if (displayName().isEmpty())
    {
        updateNameLabel(newName);
    }

    updateComponent();

    foreach (GraphicsConnection* conn, renamedConnections)
    {
        conn->setName(conn->createDefaultName());
    }

    emit nameChanged(newName, oldName);
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::setDisplayName()
//-----------------------------------------------------------------------------
void ComponentItem::setDisplayName(QString const& displayName)
{
    componentInstance_->setDisplayName(displayName);

    if (!displayName.isEmpty())
    {
        updateNameLabel(displayName);
    }
    else
    {
        updateNameLabel(name());
    }

    emit displayNameChanged(displayName);
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::setDescription()
//-----------------------------------------------------------------------------
void ComponentItem::setDescription(QString const& description)
{
    componentInstance_->setDescription(description);
    updateComponent();
    emit descriptionChanged(description);
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::name()
//-----------------------------------------------------------------------------
QString ComponentItem::name() const
{
    return componentInstance_->getInstanceName();
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::displayName()
//-----------------------------------------------------------------------------
QString ComponentItem::displayName() const
{
    return componentInstance_->getDisplayName();
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::description()
//-----------------------------------------------------------------------------
QString ComponentItem::description() const
{
    return componentInstance_->getDescription();
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::componentModel()
//-----------------------------------------------------------------------------
QSharedPointer<Component> ComponentItem::componentModel() const
{
    return component_;
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::getComponentInstance()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstance> ComponentItem::getComponentInstance() const
{
    return componentInstance_;
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::getViews()
//-----------------------------------------------------------------------------
QStringList ComponentItem::getViews() const
{
    return component_->getViewNames();
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::itemChange()
//-----------------------------------------------------------------------------
QVariant ComponentItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange)
    {
        return snapPointToGrid(value.toPointF());
    }
    else if (change == ItemScenePositionHasChanged)
    {
        componentInstance_->setPosition(scenePos());
    }

    return QGraphicsItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: updateNameLabel()
//-----------------------------------------------------------------------------
void ComponentItem::updateNameLabel(QString const& text)
{
    nameLabel_->setHtml("<center>" + text + "</center>");
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::getEndpoints()
//-----------------------------------------------------------------------------
QList<ConnectionEndpoint*> ComponentItem::getEndpoints() const
{
    QList<ConnectionEndpoint*> endpoints;

    foreach (QGraphicsItem* item, QGraphicsRectItem::childItems())
    {
        ConnectionEndpoint* endpoint = dynamic_cast<ConnectionEndpoint*>(item);

        if (endpoint != 0)
        {
            endpoints.append(endpoint);
        }
    }

    return endpoints;
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::getParentStack()
//-----------------------------------------------------------------------------
IGraphicsItemStack* ComponentItem::getParentStack()
{
    return dynamic_cast<IGraphicsItemStack*>(parentItem());
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::getUuid()
//-----------------------------------------------------------------------------
QString ComponentItem::getUuid() const
{
	return componentInstance_->getUuid();
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::connectionPoint()
//-----------------------------------------------------------------------------
QPointF ComponentItem::connectionPoint(QPointF const&) const
{
    return mapToScene(rect().center());
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::getHeight()
//-----------------------------------------------------------------------------
qreal ComponentItem::getHeight()
{
	return 8*GridSize;
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::getLibraryInterface()
//-----------------------------------------------------------------------------
LibraryInterface* ComponentItem::getLibraryInterface()
{
    return libInterface_;
}