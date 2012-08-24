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

#include <LibraryManager/vlnv.h>

#include <common/DesignDiagram.h>

#include <models/component.h>

#include <QFont>
#include <QTextDocument>

//-----------------------------------------------------------------------------
// Function: ComponentItem()
//-----------------------------------------------------------------------------
ComponentItem::ComponentItem(QRectF const& size,
                             LibraryInterface* libInterface,
                             QSharedPointer<Component> component,
                             QString const& instanceName,
                             QString const& displayName,
                             QString const& description,
                             QMap<QString, QString> const& configurableElementValues,
                             QGraphicsItem *parent)
    : QGraphicsRectItem(parent),
      libInterface_(libInterface),
      component_(component),
      name_(instanceName),
      nameLabel_(0),
      displayName_(displayName),
      description_(description),
      configurableValues_(configurableElementValues)
{
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsSelectable);
    setRect(size);
    setObjectName(instanceName);

    Q_ASSERT_X(component, "ComponentItem constructor",
        "Null component-pointer given as parameter");

    // Create the name label.
    nameLabel_ = new QGraphicsTextItem(instanceName, this);
    QFont font = nameLabel_->font();
    font.setWeight(QFont::Bold);
    nameLabel_->setFont(font);
    nameLabel_->setTextWidth(rect().width());
    nameLabel_->setPos(-nameLabel_->boundingRect().width()/2, GridSize);

    if (!displayName_.isEmpty())
    {
        updateNameLabel(displayName_);
    }
    else
    {
        updateNameLabel(name_);
    }
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
    VLNV* vlnv = component_->getVlnv();

    QString toolTipText = "";

    // Set the tooltip.
    if (vlnv->isValid())
    {
        toolTipText += "<b>Vendor:</b> " + vlnv->getVendor() + "<br>" +
                       "<b>Library:</b> " + vlnv->getLibrary() + "<br>" +
                       "<b>Name:</b> " + vlnv->getName() + "<br>" +
                       "<b>Version:</b> " + vlnv->getVersion();
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
// Function: setName()
//-----------------------------------------------------------------------------
void ComponentItem::setName(QString const& name)
{
    QString oldName = name_;

    DesignDiagram* diagram = static_cast<DesignDiagram*>(scene());
    diagram->updateInstanceName(oldName, name);

    // Find all connections that are using the default naming and should be simultaneously renamed.
    QList<GraphicsConnection*> connections;

    foreach (ConnectionEndpoint* endpoint, getEndpoints())
    {
        foreach (GraphicsConnection* conn, endpoint->getConnections())
        {
            if (conn->hasDefaultName())
            {
                connections.append(conn);
            }
        }
    }

    name_ = name;

    if (displayName_.isEmpty())
    {
        updateNameLabel(name);
    }

    updateComponent();

    foreach (GraphicsConnection* conn, connections)
    {
        conn->setName(conn->createDefaultName());
    }

    emit nameChanged(name, oldName);
}

//-----------------------------------------------------------------------------
// Function: setDisplayName()
//-----------------------------------------------------------------------------
void ComponentItem::setDisplayName(QString const& displayName)
{
    displayName_ = displayName;

    if (!displayName_.isEmpty())
    {
        updateNameLabel(displayName_);
    }
    else
    {
        updateNameLabel(name_);
    }

    emit displayNameChanged(displayName_);
}

//-----------------------------------------------------------------------------
// Function: setDescription()
//-----------------------------------------------------------------------------
void ComponentItem::setDescription(const QString& description)
{
    description_ = description;
    updateComponent();
    emit descriptionChanged(description_);
}

//-----------------------------------------------------------------------------
// Function: setConfigurableElements()
//-----------------------------------------------------------------------------
void ComponentItem::setConfigurableElements(const QMap<QString, QString>& confElements)
{
    configurableValues_ = confElements;
    emit confElementsChanged(configurableValues_);
}

//-----------------------------------------------------------------------------
// Function: name()
//-----------------------------------------------------------------------------
QString ComponentItem::name() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: displayName()
//-----------------------------------------------------------------------------
QString const& ComponentItem::displayName() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: description()
//-----------------------------------------------------------------------------
QString const& ComponentItem::description() const
{
    return description_;
}

//-----------------------------------------------------------------------------
// Function: getConfigurableElements()
//-----------------------------------------------------------------------------
QMap<QString, QString>& ComponentItem::getConfigurableElements()
{
    return configurableValues_;
}

//-----------------------------------------------------------------------------
// Function: getConfigurableElements()
//-----------------------------------------------------------------------------
QMap<QString, QString> const& ComponentItem::getConfigurableElements() const
{
    return configurableValues_;
}

//-----------------------------------------------------------------------------
// Function: componentModel()
//-----------------------------------------------------------------------------
QSharedPointer<Component> ComponentItem::componentModel()
{
    return component_;
}

//-----------------------------------------------------------------------------
// Function: componentModel()
//-----------------------------------------------------------------------------
QSharedPointer<Component const> ComponentItem::componentModel() const
{
    return component_;
}

//-----------------------------------------------------------------------------
// Function: getViews()
//-----------------------------------------------------------------------------
QStringList ComponentItem::getViews() const
{
    return component_->getViewNames();
}

//-----------------------------------------------------------------------------
// Function: itemChange()
//-----------------------------------------------------------------------------
QVariant ComponentItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange)
    {
        QPointF newPos = value.toPointF();
        return snapPointToGrid(newPos);
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
// Function: ComponentItem::getLibraryInterface()
//-----------------------------------------------------------------------------
LibraryInterface* ComponentItem::getLibraryInterface()
{
    return libInterface_;
}

//-----------------------------------------------------------------------------
// Function: ComponentItem::getEndpoints()
//-----------------------------------------------------------------------------
QList<ConnectionEndpoint*> ComponentItem::getEndpoints() const
{
    QList<ConnectionEndpoint*> endpoints;

    foreach (QGraphicsItem* item, QGraphicsRectItem::children())
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
