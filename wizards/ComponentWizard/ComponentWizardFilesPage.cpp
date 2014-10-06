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

#include "ComponentWizardPages.h"

#include <IPXACTmodels/component.h>

//-----------------------------------------------------------------------------
// Function: ComponentWizardFilesPage::ComponentWizardFilesPage()
//-----------------------------------------------------------------------------
ComponentWizardFilesPage::ComponentWizardFilesPage(QSharedPointer<Component> component, 
    QString const& componentPath, QWidget* parent)
    : QWizardPage(parent),
      component_(component),
      editor_(componentPath, QStringList(), this)
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
    return ComponentWizardPages::DEPENDENCY;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardFilesPage::validatePage()
//-----------------------------------------------------------------------------
bool ComponentWizardFilesPage::validatePage()
{
    component_->setSourceDirectories(editor_.getSourceDirectories());
    return true;
}
