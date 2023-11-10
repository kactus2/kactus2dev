//-----------------------------------------------------------------------------
// File: ComponentItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
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
    Associable(),
    libInterface_(libInterface),
    component_(component), 
    componentInstance_(instance)
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
    nameLabel_->setPlainText(instance->getInstanceName());

    setPos(instance->getPosition());
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::~ComponentItem()
//-----------------------------------------------------------------------------
ComponentItem::~ComponentItem()
{
    emit destroyed(this);
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::updateComponent()
//-----------------------------------------------------------------------------
void ComponentItem::updateComponent()
{
    updateNameLabel();

    VLNV vlnv = component_->getVlnv();

    QString toolTipText;
    if (!vlnv.isEmpty())
    {
        toolTipText += QStringLiteral("<b>Vendor:</b> ") + vlnv.getVendor() +
            QStringLiteral("<br><b>Library:</b> ") + vlnv.getLibrary() + 
            QStringLiteral("<br><b>Name:</b> ") + vlnv.getName() + 
            QStringLiteral("<br><b>Version:</b> ") + vlnv.getVersion();
    }
    else
    {
        toolTipText += QStringLiteral("Unpackaged component. No VLNV assigned!");
    }

    toolTipText += QStringLiteral("<br><br><b>Instance name:</b> ") + name();

    if (!description().isEmpty())
    {
        toolTipText += QStringLiteral("<br><br><b>Description:</b><br>") + description();
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

	auto stack = dynamic_cast<IGraphicsItemStack*>(parentItem());
	if (stack != nullptr)
	{
		stack->updateItemPositions();
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::getWidth()
//-----------------------------------------------------------------------------
qreal ComponentItem::getWidth() noexcept
{
	return COMPONENTWIDTH;
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::setName()
//-----------------------------------------------------------------------------
void ComponentItem::setName(QString const& newName)
{
    QString oldName = name();

    componentInstance_->setInstanceName(newName);

    updateNameLabel();

    updateComponent();
    
    emit nameChanged(newName, oldName);
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::setDisplayName()
//-----------------------------------------------------------------------------
void ComponentItem::setDisplayName(QString const& displayName)
{
    componentInstance_->setDisplayName(displayName);

    updateNameLabel();

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
    return componentInstance_->displayName();
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::description()
//-----------------------------------------------------------------------------
QString ComponentItem::description() const
{
    return componentInstance_->description();
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
        positionUpdated();
    }

    return QGraphicsItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::updateNameLabel()
//-----------------------------------------------------------------------------
void ComponentItem::updateNameLabel()
{
    auto text = QString();
    if (!displayName().isEmpty())
    {
        text = displayName();
    }
    else
    {
        text = name();
    }

    nameLabel_->setHtml(QStringLiteral("<center>") + text + QStringLiteral("</center>"));
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::getEndpoints()
//-----------------------------------------------------------------------------
QList<ConnectionEndpoint*> ComponentItem::getEndpoints() const
{
    QList<ConnectionEndpoint*> endpoints;

    for (QGraphicsItem* item : QGraphicsRectItem::childItems())
    {
        auto endpoint = dynamic_cast<ConnectionEndpoint*>(item);

        if (endpoint != nullptr)
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
// Function: ComponentItem::getLibraryInterface()
//-----------------------------------------------------------------------------
LibraryInterface* ComponentItem::getLibraryInterface()
{
    return libInterface_;
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::addPortToSideByPosition()
//-----------------------------------------------------------------------------
void ComponentItem::addPortToSideByPosition(ConnectionEndpoint* port)
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
// Function: ComponentItem::addPortToSideWithLessPorts()
//-----------------------------------------------------------------------------
void ComponentItem::addPortToSideWithLessPorts(ConnectionEndpoint* port)
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
// Function: ComponentItem::addPortToLeft()
//-----------------------------------------------------------------------------
void ComponentItem::addPortToLeft(ConnectionEndpoint* port)
{
    connect(port, SIGNAL(moved(ConnectionEndpoint*)), this, SIGNAL(endpointMoved(ConnectionEndpoint*)));

    leftPorts_.append(port);
    portLayout_->updateItemMove(leftPorts_, port, MIN_Y_PLACEMENT);
    portLayout_->setItemPos(leftPorts_, port, rect().left(), MIN_Y_PLACEMENT);
    checkPortLabelSize(port, rightPorts_);
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::addPortToRight()
//-----------------------------------------------------------------------------
void ComponentItem::addPortToRight(ConnectionEndpoint* port)
{
    connect(port, SIGNAL(moved(ConnectionEndpoint*)), this, SIGNAL(endpointMoved(ConnectionEndpoint*)));

    rightPorts_.append(port);
    portLayout_->updateItemMove(rightPorts_, port, MIN_Y_PLACEMENT);
    portLayout_->setItemPos(rightPorts_, port, rect().right(), MIN_Y_PLACEMENT);
    checkPortLabelSize(port, leftPorts_);
}

//-----------------------------------------------------------------------------
// Function: SystemComponentItem::checkPortLabelSize()
//-----------------------------------------------------------------------------
void ComponentItem::checkPortLabelSize(ConnectionEndpoint* port, QList<ConnectionEndpoint*> const& otherSide)
{
    if (auto it = std::find_if(otherSide.cbegin(), otherSide.cend(),
        [y = port->y()](auto const& otherPort) { return otherPort->y() == y; }); 
        it != otherSide.cend())
    {
        auto opposingPort = *it;
        qreal portLabelWidth = port->getNameLength();
        qreal otherLabelWidth = opposingPort->getNameLength();

        constexpr auto halfWidth = ComponentItem::COMPONENTWIDTH/2;

        // Check if both of the labels exceed the mid section of the component.
        if (portLabelWidth + SPACING * 2 > halfWidth && otherLabelWidth + SPACING * 2 > halfWidth)
        {
            port->shortenNameLabel(halfWidth);
            opposingPort->shortenNameLabel(halfWidth);
        }
        // Check if the other port is wider than the other.
        else if (portLabelWidth > otherLabelWidth)
        {
            port->shortenNameLabel(ComponentItem::COMPONENTWIDTH - otherLabelWidth - SPACING * 2);
        }
        else
        {
            opposingPort->shortenNameLabel(ComponentItem::COMPONENTWIDTH - portLabelWidth - SPACING * 2);
        }
    }
    else
    {
        // If there is no ports with the same y() value, the full port name width is restored.
        port->shortenNameLabel(ComponentItem::COMPONENTWIDTH);
    }
}
