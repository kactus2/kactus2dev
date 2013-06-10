//-----------------------------------------------------------------------------
// File: ComponentWizardVhdlImportPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.06.2013
//
// Description:
// VHDL import page for the component wizard.
//-----------------------------------------------------------------------------

#include "ComponentWizardVhdlImportPage.h"

#include "ComponentWizard.h"

//-----------------------------------------------------------------------------
// Function: ComponentWizardVhdlImportPage::ComponentWizardVhdlImportPage()
//-----------------------------------------------------------------------------
ComponentWizardVhdlImportPage::ComponentWizardVhdlImportPage(QSharedPointer<Component> component, 
	LibraryInterface* handler, 
	ComponentWizard* parent)
    : QWizardPage(parent),
      parent_(parent),
      //editor_(parent->getComponent(), parent->getBasePath(), this)
		editor_(new VhdlImportEditor(parent->getBasePath(), component, handler, this))
{
    setTitle(tr("Import VHDL"));
    setSubTitle(tr("Choose the top-level VHDL file to import ports and generics."));
    setFinalPage(true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(editor_);
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardVhdlImportPage::~ComponentWizardVhdlImportPage()
//-----------------------------------------------------------------------------
ComponentWizardVhdlImportPage::~ComponentWizardVhdlImportPage()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardVhdlImportPage::nextId()
//-----------------------------------------------------------------------------
int ComponentWizardVhdlImportPage::nextId() const
{
    return ComponentWizard::PAGE_CONCLUSION;
}

void ComponentWizardVhdlImportPage::initializePage() {
	editor_->initializeFileSelection();
}

bool ComponentWizardVhdlImportPage::validatePage() {
	return editor_->checkEditorValidity();
}
