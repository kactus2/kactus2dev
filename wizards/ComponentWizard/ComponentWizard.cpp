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
	  basePath_(basePath)
{
	setWindowTitle(tr("Component Wizard for %1").arg(component->getVlnv()->toString()));
    setWizardStyle(ModernStyle);
    resize(800, 800);

    setOption(NoCancelButton, true);
    setOption(NoDefaultButton, true);
    setOption(HaveFinishButtonOnEarlyPages, true);

    setPage(ComponentWizardPages::INTRO, new ComponentWizardIntroPage(component, this));
    setPage(ComponentWizardPages::GENERAL, new ComponentWizardGeneralInfoPage(component, this));    
    setPage(ComponentWizardPages::FILES, new ComponentWizardFilesPage(this));
    setPage(ComponentWizardPages::DEPENDENCY, new ComponentWizardDependencyPage(pluginMgr, this)); 
    
    ComponentWizardImportPage* importPage = new ComponentWizardImportPage(component, handler, pluginMgr, this);
    setPage(ComponentWizardPages::IMPORT, importPage);

    setPage(ComponentWizardPages::CONCLUSION, new ComponentWizardConclusionPage(handler, this));

    connect(importPage, SIGNAL(componentChanged(QSharedPointer<Component>)), 
        this, SLOT(onComponentChanged(QSharedPointer<Component>)), Qt::UniqueConnection);
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
// Function: ComponentWizard::getBasePath()
//-----------------------------------------------------------------------------
QString const& ComponentWizard::getBasePath() const
{
    return basePath_;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizard::cleanupPage()
//-----------------------------------------------------------------------------
void ComponentWizard::cleanupPage(int id)
{
    if (id == ComponentWizardPages::IMPORT)
    {
        workingComponent_ = originalComponent_;
    }

    QWizard::cleanupPage(id);
}

//-----------------------------------------------------------------------------
// Function: ComponentWizard::onComponentChanged()
//-----------------------------------------------------------------------------
void ComponentWizard::onComponentChanged(QSharedPointer<Component> component)
{
    workingComponent_ = component;
}
