//-----------------------------------------------------------------------------
// File: ComponentWizardConclusionPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.06.2013
//
// Description:
// Conclusion page for the component wizard.
//-----------------------------------------------------------------------------

#include "ComponentWizardConclusionPage.h"

#include "ComponentWizard.h"

//-----------------------------------------------------------------------------
// Function: ComponentWizardConclusionPage::ComponentWizardConclusionPage()
//-----------------------------------------------------------------------------
ComponentWizardConclusionPage::ComponentWizardConclusionPage(ComponentWizard* parent)
    : QWizardPage(parent),
    parent_(parent)
{
    setTitle(tr("Conclusion"));
    setFinalPage(true);
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardConclusionPage::~ComponentWizardConclusionPage()
//-----------------------------------------------------------------------------
ComponentWizardConclusionPage::~ComponentWizardConclusionPage()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardConclusionPage::nextId()
//-----------------------------------------------------------------------------
int ComponentWizardConclusionPage::nextId() const
{
    // The wizard ends here.
    return -1;
}
