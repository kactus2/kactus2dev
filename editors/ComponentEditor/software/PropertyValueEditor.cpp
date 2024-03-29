//-----------------------------------------------------------------------------
// File: PropertyValueEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 16.4.2012
//
// Description:
// Editor for property values.
//-----------------------------------------------------------------------------

#include "PropertyValueEditor.h"

#include <QHBoxLayout>

//-----------------------------------------------------------------------------
// Function: PropertyValueEditor::PropertyValueEditor()
//-----------------------------------------------------------------------------
PropertyValueEditor::PropertyValueEditor(QWidget* parent)
    : QGroupBox(tr("Property values"), parent),
      view_(this),
      delegate_(this),
      filter_(this),
      model_(this)
{

    // Set the model source and other options for the view.
    filter_.setSourceModel(&model_);

    view_.setModel(&filter_);
    view_.setSortingEnabled(true);
    view_.setItemsDraggable(false);
    view_.setItemDelegate(&delegate_);

    setFlat(true);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(&view_);

    connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&view_, SIGNAL(addItem(const QModelIndex&)),
            &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
            &model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: PropertyValueEditor::~PropertyValueEditor()
//-----------------------------------------------------------------------------
PropertyValueEditor::~PropertyValueEditor()
{
}

//-----------------------------------------------------------------------------
// Function: PropertyValueEditor::setAllowedProperties()
//-----------------------------------------------------------------------------
void PropertyValueEditor::setAllowedProperties(QList< QSharedPointer<ComProperty> > const& properties)
{
    delegate_.setAllowedProperties(properties);
    model_.setAllowedProperties(properties);
}

//-----------------------------------------------------------------------------
// Function: PropertyValueEditor::setData()
//-----------------------------------------------------------------------------
void PropertyValueEditor::setData(QMap<QString, QString> const& propertyValues)
{
    model_.setData(propertyValues);
}

//-----------------------------------------------------------------------------
// Function: PropertyValueEditor::getData()
//-----------------------------------------------------------------------------
QMap<QString, QString> PropertyValueEditor::getData() const
{
    return model_.getData();
}

//-----------------------------------------------------------------------------
// Function: PropertyValueEditor::setLock()
//-----------------------------------------------------------------------------
void PropertyValueEditor::setLock(bool locked)
{
    model_.setLock(locked);
}
