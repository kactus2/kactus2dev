//-----------------------------------------------------------------------------
// File: SWComponentItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 27.9.2011
//
// Description:
// Base class for graphics items representing SW components.
//-----------------------------------------------------------------------------

#include "SWComponentItem.h"

#include "EndpointDesignDiagram.h"

#include <LibraryManager/vlnv.h>

#include <models/component.h>

#include <QFont>
#include <QTextDocument>

//-----------------------------------------------------------------------------
// Function: SWComponentItem()
//-----------------------------------------------------------------------------
SWComponentItem::SWComponentItem(QRectF const& size,
                                 QSharedPointer<Component> component,
                                 QString const& instanceName,
                                 QString const& displayName,
                                 QString const& description,
                                 QMap<QString, QString> const& configurableElementValues,
                                 QGraphicsItem *parent) :
QGraphicsRectItem(parent), component_(component), nameLabel_(0), displayName_(displayName),
description_(description), configurableValues_(configurableElementValues)
{
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsSelectable);
    setRect(size);
    setObjectName(instanceName);

    Q_ASSERT_X(component, "SWComponentItem constructor",
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
// Function: ~SWComponentItem()
//-----------------------------------------------------------------------------
SWComponentItem::~SWComponentItem()
{
    emit destroyed(this);
}

//-----------------------------------------------------------------------------
// Function: updateComponent()
//-----------------------------------------------------------------------------
void SWComponentItem::updateComponent()
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
void SWComponentItem::setName(QString const& name)
{
    QString oldName = nameLabel_->toPlainText();
    //EndpointDesignDiagram* diagram = static_cast<EndpointDesignDiagram*>(scene());
    //diagram->updateInstanceName(oldName, name); TODO: Enable this when possible.
    nameLabel_->setHtml("<center>"+name+"</center>");

    emit contentChanged();
    emit nameChanged(name, oldName);
}

//-----------------------------------------------------------------------------
// Function: setDisplayName()
//-----------------------------------------------------------------------------
void SWComponentItem::setDisplayName(QString const& displayName)
{
    displayName_ = displayName;
    emit contentChanged();
    emit displayNameChanged(displayName_);
}

//-----------------------------------------------------------------------------
// Function: setDescription()
//-----------------------------------------------------------------------------
void SWComponentItem::setDescription(const QString& description)
{
    description_ = description;
    emit contentChanged();
    emit descriptionChanged(description_);
}

//-----------------------------------------------------------------------------
// Function: setConfigurableElements()
//-----------------------------------------------------------------------------
void SWComponentItem::setConfigurableElements(const QMap<QString, QString>& confElements)
{
    configurableValues_ = confElements;
    emit contentChanged();
    emit confElementsChanged(configurableValues_);
}

//-----------------------------------------------------------------------------
// Function: name()
//-----------------------------------------------------------------------------
QString SWComponentItem::name() const
{
    return nameLabel_->toPlainText();
}

//-----------------------------------------------------------------------------
// Function: displayName()
//-----------------------------------------------------------------------------
QString const& SWComponentItem::displayName() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: description()
//-----------------------------------------------------------------------------
QString const& SWComponentItem::description() const
{
    return description_;
}

//-----------------------------------------------------------------------------
// Function: getConfigurableElements()
//-----------------------------------------------------------------------------
QMap<QString, QString>& SWComponentItem::getConfigurableElements()
{
    return configurableValues_;
}

//-----------------------------------------------------------------------------
// Function: componentModel()
//-----------------------------------------------------------------------------
QSharedPointer<Component> SWComponentItem::componentModel()
{
    return component_;
}

//-----------------------------------------------------------------------------
// Function: getViews()
//-----------------------------------------------------------------------------
QStringList SWComponentItem::getViews() const
{
    return component_->getViewNames();
}

//-----------------------------------------------------------------------------
// Function: itemChange()
//-----------------------------------------------------------------------------
QVariant SWComponentItem::itemChange(GraphicsItemChange change, const QVariant &value)
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
