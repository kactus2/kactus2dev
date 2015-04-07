//-----------------------------------------------------------------------------
// File: ComponentWizard.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.06.2013
//
// Description:
// Component wizard dialog.
//-----------------------------------------------------------------------------

#include "ComponentWizard.h"
#include "ComponentWizardPages.h"

#include <QLabel>
#include <QVBoxLayout>

#include "ComponentWizardIntroPage.h"
#include "ComponentWizardFilesPage.h"
#include "ComponentWizardDependencyPage.h"
#include "ComponentWizardGeneralInfoPage.h"
#include "ComponentWizardImportPage.h"
#include "ComponentWizardViewsPage.h"
#include "ComponentWizardConclusionPage.h"

//-----------------------------------------------------------------------------
// Function: ComponentWizard::ComponentWizard()
//-----------------------------------------------------------------------------
ComponentWizard::ComponentWizard(QSharedPointer<Component> component,
	                             QString const& basePath,
	                             PluginManager const& pluginMgr,
	                             LibraryInterface* handler,
	                             QWidget* parent)
    : QWizard(parent),
	  originalComponent_(component),
      workingComponent_(component),
      parameterFinder_(new ComponentParameterFinder(workingComponent_)),
      expressionFormatter_(new ExpressionFormatter(parameterFinder_)),
      referenceCounter_(new ParameterReferenceCounter(parameterFinder_))
{
	setWindowTitle(tr("Component Wizard for %1").arg(component->getVlnv()->toString()));
    setWizardStyle(ModernStyle);
    resize(800, 1000);

    setOption(NoBackButtonOnStartPage, true);
    setOption(NoCancelButton, true);
    setOption(NoDefaultButton, true);
    setOption(HaveFinishButtonOnEarlyPages, true);

    ComponentWizardImportPage* importPage = new ComponentWizardImportPage(component, handler, pluginMgr,
        parameterFinder_, expressionFormatter_, this);

    ComponentWizardConclusionPage* conclusionPage = new ComponentWizardConclusionPage(component, handler,
        expressionFormatter_, this);

    setPage(ComponentWizardPages::INTRO, new ComponentWizardIntroPage(component, this));
    setPage(ComponentWizardPages::GENERAL, new ComponentWizardGeneralInfoPage(component, this));    
    setPage(ComponentWizardPages::FILES, new ComponentWizardFilesPage(component, basePath, this));
    setPage(ComponentWizardPages::DEPENDENCY, new ComponentWizardDependencyPage(component, basePath, pluginMgr, 
        this));     
    setPage(ComponentWizardPages::IMPORT, importPage);
    setPage(ComponentWizardPages::VIEWS, new ComponentWizardViewsPage(handler, parameterFinder_,
        expressionFormatter_, this));
    setPage(ComponentWizardPages::CONCLUSION, conclusionPage);

    connect(importPage, SIGNAL(componentChanged(QSharedPointer<Component>)), 
        this, SLOT(onComponentChanged(QSharedPointer<Component>)), Qt::UniqueConnection);

    connect(importPage, SIGNAL(componentChanged(QSharedPointer<Component>)), 
        conclusionPage, SLOT(onComponentChanged(QSharedPointer<Component>)), Qt::UniqueConnection);

    connect(importPage, SIGNAL(increaseReferences(QString)),
        referenceCounter_.data(), SLOT(increaseReferenceCount(QString)), Qt::UniqueConnection);
    connect(importPage, SIGNAL(decreaseReferences(QString)),
        referenceCounter_.data(), SLOT(decreaseReferenceCount(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ComponentWizard::~ComponentWizard()
//-----------------------------------------------------------------------------
ComponentWizard::~ComponentWizard()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentWizard::getComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> ComponentWizard::getComponent()
{
    return workingComponent_;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizard::cleanupPage()
//-----------------------------------------------------------------------------
void ComponentWizard::cleanupPage(int id)
{
    if (id == ComponentWizardPages::IMPORT)
    {
        workingComponent_ = originalComponent_;

        parameterFinder_->setComponent(workingComponent_);
    }

    QWizard::cleanupPage(id);
}

//-----------------------------------------------------------------------------
// Function: ComponentWizard::onComponentChanged()
//-----------------------------------------------------------------------------
void ComponentWizard::onComponentChanged(QSharedPointer<Component> component)
{
    workingComponent_ = component;

    parameterFinder_->setComponent(workingComponent_);
}
