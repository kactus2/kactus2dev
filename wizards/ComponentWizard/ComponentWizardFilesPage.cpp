//-----------------------------------------------------------------------------
// File: ComponentWizardFilesPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.06.2013
//
// Description:
// Files page for the component wizard.
//-----------------------------------------------------------------------------

#include "ComponentWizardFilesPage.h"

#include "ComponentWizard.h"

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>
#include <IPXACTmodels/component.h>

//-----------------------------------------------------------------------------
// Function: ComponentWizardFilesPage::ComponentWizardFilesPage()
//-----------------------------------------------------------------------------
ComponentWizardFilesPage::ComponentWizardFilesPage(ComponentWizard* parent)
    : QWizardPage(parent),
      parent_(parent),
      editor_(parent->getBasePath(), QStringList(), this)
{
    setTitle(tr("Add Files"));
    setSubTitle(tr("Add files to the component by specifying the source directories to scan."));
    setFinalPage(true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&editor_);
    layout->addStretch(1);
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardFilesPage::~ComponentWizardFilesPage()
//-----------------------------------------------------------------------------
ComponentWizardFilesPage::~ComponentWizardFilesPage()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardFilesPage::nextId()
//-----------------------------------------------------------------------------
int ComponentWizardFilesPage::nextId() const
{
    return ComponentWizard::PAGE_DEPENDENCY;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardFilesPage::validatePage()
//-----------------------------------------------------------------------------
bool ComponentWizardFilesPage::validatePage()
{
    parent_->getComponent()->setSourceDirectories(editor_.getSourceDirectories());
    return true;
}
