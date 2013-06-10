//-----------------------------------------------------------------------------
// File: ComponentWizardVhdlImportPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Määttä
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
ComponentWizardVhdlImportPage::ComponentWizardVhdlImportPage(ComponentWizard* parent)
    : QWizardPage(parent),
      parent_(parent),
      editor_(parent->getComponent(), parent->getBasePath(), this)
{
    setTitle(tr("Import VHDL"));
    setSubTitle(tr("Choose the top-level VHDL file to import ports and generics."));
    setFinalPage(true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&editor_);
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
