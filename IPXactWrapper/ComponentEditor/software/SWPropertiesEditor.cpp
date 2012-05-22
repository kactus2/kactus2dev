//-----------------------------------------------------------------------------
// File: SWPropertiesEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 21.5.2012
//
// Description:
// Editor for SW properties.
//-----------------------------------------------------------------------------

#include "SWPropertiesEditor.h"

#include <IPXactWrapper/ComDefinitionEditor/ComPropertyDelegate.h>
#include <models/ComProperty.h>

#include <QHBoxLayout>

//-----------------------------------------------------------------------------
// Function: SWPropertiesEditor::SWPropertiesEditor()
//-----------------------------------------------------------------------------
SWPropertiesEditor::SWPropertiesEditor(QSharedPointer<Component> component, QWidget *parent)
    : ItemEditor(component, parent),
      view_(this),
      filter_(this),
      model_(this)
{
    // Set the model source and other options for the view.
    filter_.setSourceModel(&model_);

    view_.setModel(&filter_);
    view_.setSortingEnabled(true);
    view_.setItemsDraggable(false);
    view_.setItemDelegate(new ComPropertyDelegate(this));

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(&view_);

    model_.setProperties(component->getSWProperties());

    connect(&model_, SIGNAL(contentChanged()),
            this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&view_, SIGNAL(addItem(const QModelIndex&)),
            &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
            &model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: SWPropertiesEditor::~SWPropertiesEditor()
//-----------------------------------------------------------------------------
SWPropertiesEditor::~SWPropertiesEditor()
{
}

//-----------------------------------------------------------------------------
// Function: SWPropertiesEditor::isValid()
//-----------------------------------------------------------------------------
bool SWPropertiesEditor::isValid() const
{
    return model_.isValid();
}

//-----------------------------------------------------------------------------
// Function: SWPropertiesEditor::makeChanges()
//-----------------------------------------------------------------------------
void SWPropertiesEditor::makeChanges()
{
    QList< QSharedPointer<ComProperty> > properties;

    foreach (QSharedPointer<ComProperty> prop, model_.getProperties())
    {
        properties.append(QSharedPointer<ComProperty>(new ComProperty(*prop.data())));
    }

    component()->setSWProperties(properties);
}
