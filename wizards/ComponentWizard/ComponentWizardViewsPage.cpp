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
#include "ViewListModel.h"

#include <editors/ComponentEditor/views/vieweditor.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>
#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>

#include <KactusAPI/include/ComponentInstantiationParameterFinder.h>
#include <KactusAPI/include/MultipleParameterFinder.h>
#include <KactusAPI/include/ModuleParameterInterface.h>

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>


//-----------------------------------------------------------------------------
// Function: ComponentWizardViewsPage::ComponentWizardViewsPage()
//-----------------------------------------------------------------------------
ComponentWizardViewsPage::ComponentWizardViewsPage(LibraryInterface* lh,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    ComponentWizard* parent):
QWizardPage(parent),
library_(lh),
parent_(parent),
editorTabs_(new QTabWidget(this)),
addButton_(new QPushButton(QIcon(":/icons/common/graphics/add.png"), QString(), this)),
removeButton_(new QPushButton(QIcon(":/icons/common/graphics/remove.png"), QString(), this)),
viewList_(new QListView(this)),
viewModel_(new ViewListModel(this)),
parameterFinder_(parameterFinder),
expressionFormatter_(expressionFormatter),
validator_(QSharedPointer<ExpressionParser>(new IPXactSystemVerilogParser(parameterFinder)),
           parent_->getComponent()->getModel())
{
    setTitle(tr("Views"));
    setSubTitle(tr("Setup the view(s) for the component.\n"
        "Views specify different representations of the component for e.g. simulation and synthesis."));
    setFinalPage(true);

    addButton_->setToolTip(tr("Add new view"));
    removeButton_->setToolTip(tr("Remove selection"));

    viewList_->setSelectionMode(QAbstractItemView::SingleSelection);
    viewList_->setModel(viewModel_);
    
    setupLayout();

    connect(addButton_, SIGNAL(clicked()), this, SLOT(onViewAdded()), Qt::UniqueConnection);
    connect(removeButton_, SIGNAL(clicked()), this, SLOT(onViewRemoved()), Qt::UniqueConnection);
    connect(viewList_, SIGNAL(clicked(QModelIndex const&)), 
        this, SLOT(onViewSelected(QModelIndex const&)), Qt::UniqueConnection);
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
   editorTabs_->clear();

   QSharedPointer<Component> component = parent_->getComponent();
   viewModel_->setComponent(component);
   
   validator_.componentChange(component->getModel());

   foreach(QSharedPointer<View> view, *component->getViews())
   {
       createEditorForView(component, view);
   }

   removeButton_->setEnabled(parent_->getComponent()->getViews()->count() > 1);
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardViewsPage::isComplete()
//-----------------------------------------------------------------------------
bool ComponentWizardViewsPage::isComplete() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardViewsPage::onViewEdited()
//-----------------------------------------------------------------------------
void ComponentWizardViewsPage::onViewEdited()
{    
    int currentIndex = editorTabs_->currentIndex();

    editorTabs_->setTabText(currentIndex, parent_->getComponent()->getViewNames().at(currentIndex));
    updateIconForTab(currentIndex);
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardViewsPage::onViewAdded()
//-----------------------------------------------------------------------------
void ComponentWizardViewsPage::onViewAdded()
{
    viewModel_->addView();

    QSharedPointer<Component> component = parent_->getComponent(); 
    QSharedPointer<View> createdView = component->getViews()->last();

    createEditorForView(component, createdView);

    removeButton_->setEnabled(true);

    emit completeChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardViewsPage::onViewRemoved()
//-----------------------------------------------------------------------------
void ComponentWizardViewsPage::onViewRemoved()
{
    QModelIndexList selectedIndexes = viewList_->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty())
    {
        return;
    }

    QModelIndex selectedIndex = selectedIndexes.first();
    QString selectedView = parent_->getComponent()->getViewNames().at(selectedIndex.row());    
    
    removeEditorsForView(selectedView);

    viewModel_->removeView(selectedIndex);

    removeButton_->setEnabled(parent_->getComponent()->getViews()->count() > 1);

    emit completeChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardViewsPage::onViewSelected()
//-----------------------------------------------------------------------------
void ComponentWizardViewsPage::onViewSelected(QModelIndex const& index)
{
    if (index.isValid())
    {
        editorTabs_->setCurrentIndex(index.row());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardViewsPage::createEditorForView()
//-----------------------------------------------------------------------------
void ComponentWizardViewsPage::createEditorForView(QSharedPointer<Component> component, QSharedPointer<View> view)
{
    QSharedPointer<ComponentInstantiationParameterFinder> instantiationParameterFinder(
        new ComponentInstantiationParameterFinder(QSharedPointer<ComponentInstantiation>()));

    QSharedPointer<MultipleParameterFinder> combinedInstantiationFinder =
        QSharedPointer<MultipleParameterFinder>(new MultipleParameterFinder);
    combinedInstantiationFinder->addFinder(parameterFinder_);
    combinedInstantiationFinder->addFinder(instantiationParameterFinder);

    QSharedPointer<IPXactSystemVerilogParser> expressionParser(
        new IPXactSystemVerilogParser(combinedInstantiationFinder));

    QSharedPointer<ExpressionFormatter> moduleFormatter(new ExpressionFormatter(combinedInstantiationFinder));

    QSharedPointer<ParameterValidator> validator(
        new ParameterValidator(expressionParser, component->getChoices(), component->getRevision()));
    ModuleParameterInterface* moduleParameterInterface = new ModuleParameterInterface(
        validator, expressionParser, moduleFormatter, instantiationParameterFinder);

    ViewEditor* editor = new ViewEditor(
        component, view, library_, combinedInstantiationFinder, moduleFormatter, moduleParameterInterface, this);
    int editorIndex = editorTabs_->addTab(editor, view->name());

    updateIconForTab(editorIndex);

    connect(editor, SIGNAL(contentChanged()), this, SLOT(onViewEdited()), Qt::UniqueConnection);
    connect(editor, SIGNAL(contentChanged()), this, SIGNAL(completeChanged()), Qt::UniqueConnection);    
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardViewsPage::updateIconForTab()
//-----------------------------------------------------------------------------
void ComponentWizardViewsPage::updateIconForTab(int tabIndex) const
{
    QSharedPointer<View> viewInTab = parent_->getComponent()->getViews()->at(tabIndex);
    if (validator_.validate(viewInTab))
    {
        editorTabs_->setTabIcon(tabIndex, QIcon());
    }
    else
    {
        editorTabs_->setTabIcon(tabIndex, QIcon(":icons/common/graphics/exclamation.png"));
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardViewsPage::removeEditorsForView()
//-----------------------------------------------------------------------------
void ComponentWizardViewsPage::removeEditorsForView(QString const& viewName)
{
    for(int i = 0; i < editorTabs_->count(); i++)
    {
        if (editorTabs_->tabText(i) == viewName)
        {         
            QWidget* removedEditor = editorTabs_->widget(i);
            editorTabs_->removeTab(i);
            delete removedEditor;
            i--;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardViewsPage::setupLayout()
//-----------------------------------------------------------------------------
void ComponentWizardViewsPage::setupLayout()
{
    viewList_->setMaximumHeight(75);
    editorTabs_->setContentsMargins(2, 2, 2, 2);

    QVBoxLayout* topLayout = new QVBoxLayout(this);

    QDialogButtonBox* viewListButtons = new QDialogButtonBox(Qt::Vertical, this);
    viewListButtons->addButton(addButton_, QDialogButtonBox::ActionRole);
    viewListButtons->addButton(removeButton_, QDialogButtonBox::ActionRole);
    
    QGroupBox* viewsBox = new QGroupBox(tr("Component views"), this);
    QHBoxLayout* viewsLayout = new QHBoxLayout(viewsBox);
    viewsLayout->addWidget(viewList_);
    viewsLayout->addWidget(viewListButtons);

    topLayout->addWidget(viewsBox);
    topLayout->addWidget(editorTabs_, 1);
}
