//-----------------------------------------------------------------------------
// File: ComponentWizardImportPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.06.2013
//
// Description:
// Import page for the component wizard.
//-----------------------------------------------------------------------------

#include "ComponentWizardImportPage.h"
#include "ComponentWizardPages.h"

#include <IPXACTmodels/component.h>

#include <wizards/ComponentWizard/ImportEditor/ImportEditor.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ComponentWizardVhdlImportPage::ComponentWizardVhdlImportPage()
//-----------------------------------------------------------------------------
ComponentWizardImportPage::ComponentWizardImportPage(QSharedPointer<Component> component, 
	LibraryInterface* handler, PluginManager const& pluginMgr, QWidget* parent)
    : QWizardPage(parent),
      editor_(new ImportEditor(component, handler, pluginMgr, this))
{
    setTitle(tr("Import file"));
    setSubTitle(tr("Choose the top-level file to import into component."));
    setFinalPage(true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(editor_);

    connect(editor_, SIGNAL(contentChanged()), this, SIGNAL(completeChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardVhdlImportPage::~ComponentWizardVhdlImportPage()
//-----------------------------------------------------------------------------
ComponentWizardImportPage::~ComponentWizardImportPage()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardVhdlImportPage::nextId()
//-----------------------------------------------------------------------------
int ComponentWizardImportPage::nextId() const
{
    return ComponentWizardPages::CONCLUSION;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardVhdlImportPage::initializePage()
//-----------------------------------------------------------------------------
void ComponentWizardImportPage::initializePage()
{	
    editor_->initializeFileSelection();
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardVhdlImportPage::isComplete()
//-----------------------------------------------------------------------------
bool ComponentWizardImportPage::isComplete() const
{
	return editor_->checkEditorValidity();
}
