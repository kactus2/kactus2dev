//-----------------------------------------------------------------------------
// File: ComponentWizardViewsPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 09.10.2014
//
// Description:
// Component wizard page for setting up views.
//-----------------------------------------------------------------------------

#include "ComponentWizardViewsPage.h"
#include "ComponentWizardPages.h"
#include "ComponentWizard.h"

#include <editors/ComponentEditor/views/vieweditor.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/component.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ComponentWizardViewsPage::ComponentWizardViewsPage()
//-----------------------------------------------------------------------------
ComponentWizardViewsPage::ComponentWizardViewsPage(LibraryInterface* lh, ComponentWizard* parent)
    : QWizardPage(parent), 
    library_(lh), 
    parent_(parent),
    viewTabs_(new QTabWidget(this)),
    views_()
{
    setTitle(tr("Views"));
    setSubTitle(tr("Setup the views for the component."));
    setFinalPage(true);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardViewsPage::~ComponentWizardViewsPage()
//-----------------------------------------------------------------------------
ComponentWizardViewsPage::~ComponentWizardViewsPage()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentWizardViewsPage::nextId()
//-----------------------------------------------------------------------------
int ComponentWizardViewsPage::nextId() const
{
    return ComponentWizardPages::CONCLUSION;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardViewsPage::initializePage()
//-----------------------------------------------------------------------------
void ComponentWizardViewsPage::initializePage()
{   
   views_.clear();
   viewTabs_->clear();

   QSharedPointer<Component> component = parent_->getComponent();
   foreach(QSharedPointer<View> view, component->getViews())
   {
       ViewEditor* editor = new ViewEditor(component, view, library_, this);       

       int editorIndex = viewTabs_->addTab(editor, view->getName());
       views_.insert(editorIndex, view);

       connect(editor, SIGNAL(contentChanged()), this, SLOT(onViewEdited()), Qt::UniqueConnection);
       connect(editor, SIGNAL(contentChanged()), this, SIGNAL(completeChanged()), Qt::UniqueConnection);     
   }
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardViewsPage::isComplete()
//-----------------------------------------------------------------------------
bool ComponentWizardViewsPage::isComplete() const
{
    for(int i = 0; i < viewTabs_->count(); i++)
    {
        ViewEditor* editor = dynamic_cast<ViewEditor*>(viewTabs_->widget(i));
        if (!editor->isValid())
        {         
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardViewsPage::onViewEdited()
//-----------------------------------------------------------------------------
void ComponentWizardViewsPage::onViewEdited()
{    
    int currentIndex = viewTabs_->currentIndex();
    viewTabs_->setTabText(currentIndex, views_.value(currentIndex)->getName());

    ViewEditor* editor = dynamic_cast<ViewEditor*>(viewTabs_->currentWidget());
    if (editor->isValid())
    {
        viewTabs_->setTabIcon(currentIndex, QIcon());        
    }
    else
    {
        viewTabs_->setTabIcon(currentIndex, QIcon(":icons/common/graphics/exclamation.png"));
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardViewsPage::setupLayout()
//-----------------------------------------------------------------------------
void ComponentWizardViewsPage::setupLayout()
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);

    topLayout->addWidget(viewTabs_);
}
