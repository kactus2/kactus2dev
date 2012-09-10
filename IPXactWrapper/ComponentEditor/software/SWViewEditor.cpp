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
SWViewEditor::SWViewEditor(QSharedPointer<Component> component, 
						   QSharedPointer<SWView> swView,
                           LibraryInterface* libHandler, 
						   QWidget *parent):
ItemEditor(component, parent), 
libHandler_(libHandler),
view_(swView.data()),
nameEditor_(swView->getNameGroup(), this, tr("Name and description")),
hierRefEditor_(VLNV::DESIGNCONFIGURATION, libHandler, this, this)
{
    connect(&nameEditor_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&hierRefEditor_, SIGNAL(vlnvEdited()),
        this, SLOT(onHierRefChange()), Qt::UniqueConnection);

    hierRefEditor_.setTitle(tr("Hierarchy reference"));

    refresh();

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&nameEditor_);
    layout->addWidget(&hierRefEditor_);
    layout->addStretch();
	layout->setContentsMargins(0, 0, 0, 0);
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
    if (!nameEditor_.isValid() || !hierRefEditor_.isValid())
        return false;

    return true;
}

void SWViewEditor::refresh() {
	nameEditor_.refresh();
	hierRefEditor_.setVLNV(view_->getHierarchyRef());
}

void SWViewEditor::onHierRefChange() {
	view_->setHierarchyRef(hierRefEditor_.getVLNV());
	emit contentChanged();
}

void SWViewEditor::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/swview.html");
}
