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
#include "ComponentWizardPages.h"

#include <IPXACTmodels/component.h>

#include <wizards/ComponentWizard/VhdlImportEditor/vhdlimporteditor.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ComponentWizardVhdlImportPage::ComponentWizardVhdlImportPage()
//-----------------------------------------------------------------------------
ComponentWizardVhdlImportPage::ComponentWizardVhdlImportPage(QSharedPointer<Component> component, 
	LibraryInterface* handler, PluginManager const& pluginMgr, QWidget* parent)
    : QWizardPage(parent),
      editor_(new VhdlImportEditor(component, handler, pluginMgr, this))
{
    setTitle(tr("Import VHDL"));
    setSubTitle(tr("Choose the top-level VHDL file to import ports and generics."));
    setFinalPage(true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(editor_);

    connect(editor_, SIGNAL(contentChanged()), this, SIGNAL(completeChanged()), Qt::UniqueConnection);
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
    return ComponentWizardPages::CONCLUSION;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardVhdlImportPage::initializePage()
//-----------------------------------------------------------------------------
void ComponentWizardVhdlImportPage::initializePage()
{	
    editor_->initializeFileSelection();
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardVhdlImportPage::isComplete()
//-----------------------------------------------------------------------------
bool ComponentWizardVhdlImportPage::isComplete() const
{
	return editor_->checkEditorValidity();
}
