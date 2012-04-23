//-----------------------------------------------------------------------------
// File: ComPropertyEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.4.2012
//
// Description:
// Editor for COM properties.
//-----------------------------------------------------------------------------

#include "ComPropertyEditor.h"

#include "ComPropertyDelegate.h"

#include <QHBoxLayout>

//-----------------------------------------------------------------------------
// Function: ComPropertyEditor::ComPropertyEditor()
//-----------------------------------------------------------------------------
ComPropertyEditor::ComPropertyEditor(QWidget* parent)
    : QGroupBox(tr("Properties"), parent),
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

    connect(&model_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&view_, SIGNAL(addItem(const QModelIndex&)),
        &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
            &model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComPropertyEditor::~ComPropertyEditor()
//-----------------------------------------------------------------------------
ComPropertyEditor::~ComPropertyEditor()
{
}

//-----------------------------------------------------------------------------
// Function: ComPropertyEditor::setData()
//-----------------------------------------------------------------------------
void ComPropertyEditor::setProperties(QList< QSharedPointer<ComProperty> > const& properties)
{
    model_.setProperties(properties);
}

//-----------------------------------------------------------------------------
// Function: ComPropertyEditor::getData()
//-----------------------------------------------------------------------------
QList< QSharedPointer<ComProperty> > const& ComPropertyEditor::getProperties() const
{
    return model_.getProperties();
}
