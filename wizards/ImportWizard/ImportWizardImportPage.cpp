//-----------------------------------------------------------------------------
// File: ImportWizardImportPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 15.10.2014
//
// Description:
// Import wizard page for importing elements from a file.
//-----------------------------------------------------------------------------

#include <wizards/ComponentWizard/ComponentWizardImportPage.h>

#include "ImportWizardImportPage.h"
#include "ImportWizardPages.h"

//-----------------------------------------------------------------------------
// Function: ImportWizardImportPage::ImportWizardImportPage()
//-----------------------------------------------------------------------------
ImportWizardImportPage::ImportWizardImportPage(QSharedPointer<Component> component,
    LibraryInterface* handler,
    PluginManager const& pluginMgr,
    QWidget* parent)
    : ComponentWizardImportPage(component, handler, pluginMgr, parent)
{
    setFinalPage(false);
}

//-----------------------------------------------------------------------------
// Function: ImportWizardImportPage::~ImportWizardImportPage()
//-----------------------------------------------------------------------------
ImportWizardImportPage::~ImportWizardImportPage()
{
}

//-----------------------------------------------------------------------------
// Function: ImportWizardImportPage::nextId()
//-----------------------------------------------------------------------------
int ImportWizardImportPage::nextId() const
{
    return ImportWizardPages::CONCLUSION;
}
