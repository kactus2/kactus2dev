//-----------------------------------------------------------------------------
// File: SWViewEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.5.2012
//
// Description:
// SW view editor.
//-----------------------------------------------------------------------------

#include "SWViewEditor.h"

#include <models/SWView.h>

//-----------------------------------------------------------------------------
// Function: SWViewEditor::SWViewEditor()
//-----------------------------------------------------------------------------
SWViewEditor::SWViewEditor(QSharedPointer<Component> component, void* dataPointer,
                           LibraryInterface* libHandler, QWidget *parent)
    : ItemEditor(component, parent), 
      libHandler_(libHandler),
      view_(static_cast<SWView*>(dataPointer)),
      nameGroup_(this),
      hierRefEditor_(VLNV::DESIGNCONFIGURATION, libHandler, this, this)
{
    connect(&nameGroup_, SIGNAL(nameChanged(const QString&)),
            this, SIGNAL(nameChanged(const QString&)), Qt::UniqueConnection);
    connect(&nameGroup_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&hierRefEditor_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    nameGroup_.setTitle(tr("Name and description"));
    hierRefEditor_.setTitle(tr("Hierarchy reference"));

    nameGroup_.setName(view_->getName());
    nameGroup_.setDisplayName(view_->getDisplayName());
    nameGroup_.setDescription(view_->getDescription());
    hierRefEditor_.setVLNV(view_->getHierarchyRef());

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&nameGroup_);
    layout->addWidget(&hierRefEditor_);
    layout->addStretch();
}

//-----------------------------------------------------------------------------
// Function: SWViewEditor::~SWViewEditor()
//-----------------------------------------------------------------------------
SWViewEditor::~SWViewEditor()
{

}

//-----------------------------------------------------------------------------
// Function: SWViewEditor::isValid()
//-----------------------------------------------------------------------------
bool SWViewEditor::isValid() const
{
    // if name group is not valid
    if (!nameGroup_.isValid() || !hierRefEditor_.isValid())
        return false;

    return true;
}

//-----------------------------------------------------------------------------
// Function: SWViewEditor::removeModel()
//-----------------------------------------------------------------------------
void SWViewEditor::removeModel()
{
    component()->removeSWView(view_->getName());
    view_ = 0;
}

//-----------------------------------------------------------------------------
// Function: SWViewEditor::makeChanges()
//-----------------------------------------------------------------------------
void SWViewEditor::makeChanges()
{
    view_->setName(nameGroup_.getName());
    view_->setDisplayName(nameGroup_.getDisplayName());
    view_->setDescription(nameGroup_.getDescription());
    view_->setHierarchyRef(hierRefEditor_.getVLNV());
}
