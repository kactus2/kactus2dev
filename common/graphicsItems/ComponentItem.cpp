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

#include <LibraryManager/vlnv.h>

#include <EndpointDesign/EndpointDesignDiagram.h>
#include <designwidget/blockdiagram.h>

#include <models/component.h>

#include <QFont>
#include <QTextDocument>

//-----------------------------------------------------------------------------
// Function: ComponentItem()
//-----------------------------------------------------------------------------
ComponentItem::ComponentItem(QRectF const& size, QSharedPointer<Component> component,
                             QString const& instanceName, QString const& displayName,
                             QString const& description,
                             QMap<QString, QString> const& configurableElementValues,
                             QGraphicsItem *parent) :
QGraphicsRectItem(parent), component_(component), name_(instanceName), nameLabel_(0), displayName_(displayName),
description_(description), configurableValues_(configurableElementValues)
{
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsSelectable);
    setRect(size);
    setObjectName(instanceName);

    Q_ASSERT_X(component, "ComponentItem constructor",
        "Null component-pointer given as parameter");

    // Create the name label.
    nameLabel_ = new QGraphicsTextItem(instanceName, this);
    nameLabel_->setHtml("<center>"+instanceName+"</center>");
    QFont font = nameLabel_->font();
    font.setWeight(QFont::Bold);
    nameLabel_->setFont(font);
    nameLabel_->setTextWidth(rect().width());
    nameLabel_->setPos(-nameLabel_->boundingRect().width()/2, GridSize);

    connect(nameLabel_->document(), SIGNAL(contentsChanged()), this, SIGNAL(contentChanged()));
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

    // Set the tooltip.
    if (vlnv->isValid())
    {
        setToolTip("Vendor : " + vlnv->getVendor() + "\n" +
            "Library : " + vlnv->getLibrary() + "\n" +
            "Name : " + vlnv->getName() + "\n" +
            "Version : " + vlnv->getVersion());
    }
    else
    {
        setToolTip("Unpackaged component. No VLNV assigned!");
    }
}

//-----------------------------------------------------------------------------
// Function: setName()
//-----------------------------------------------------------------------------
void ComponentItem::setName(QString const& name)
{
    QString oldName = name_;

    DesignDiagram* diagram = static_cast<DesignDiagram*>(scene());
    diagram->updateInstanceName(oldName, name);

    name_ = name;
    updateNameLabel(name);

    emit contentChanged();
    emit nameChanged(name, oldName);
}

//-----------------------------------------------------------------------------
// Function: setDisplayName()
//-----------------------------------------------------------------------------
void ComponentItem::setDisplayName(QString const& displayName)
{
    displayName_ = displayName;
    emit contentChanged();
    emit displayNameChanged(displayName_);
}

//-----------------------------------------------------------------------------
// Function: setDescription()
//-----------------------------------------------------------------------------
void ComponentItem::setDescription(const QString& description)
{
    description_ = description;
    emit contentChanged();
    emit descriptionChanged(description_);
}

//-----------------------------------------------------------------------------
// Function: setConfigurableElements()
//-----------------------------------------------------------------------------
void ComponentItem::setConfigurableElements(const QMap<QString, QString>& confElements)
{
    configurableValues_ = confElements;
    emit contentChanged();
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
    if (change == ItemPositionChange) {
        QPointF newPos = value.toPointF();
        return snapPointToGrid(newPos);
    }
    else if (change == ItemPositionHasChanged)
    {
        emit contentChanged();
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
