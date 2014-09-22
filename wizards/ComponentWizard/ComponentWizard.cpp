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
#include "ComponentWizardVhdlImportPage.h"
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
	  component_(component),
	  basePath_(basePath)
{
    VLNV* vlnv = component->getVlnv();
	setWindowTitle(tr("Component Wizard for %1:%2:%3:%4").arg(vlnv->getVendor()).arg(vlnv->getLibrary()).arg(vlnv->getName()).arg(vlnv->getVersion()));
    setWizardStyle(ModernStyle);
    setOption(NoCancelButton, true);
    setOption(NoDefaultButton, true);
    setOption(HaveFinishButtonOnEarlyPages, true);
    resize(800, 800);
    
    setPage(ComponentWizardPages::INTRO, new ComponentWizardIntroPage(this));
    setPage(ComponentWizardPages::GENERAL, new ComponentWizardGeneralInfoPage(component, this));    
    setPage(ComponentWizardPages::FILES, new ComponentWizardFilesPage(this));
    setPage(ComponentWizardPages::DEPENDENCY, new ComponentWizardDependencyPage(pluginMgr, this));    
    setPage(ComponentWizardPages::VHDL_IMPORT, new ComponentWizardVhdlImportPage(component, handler, pluginMgr, 
        this));
    setPage(ComponentWizardPages::CONCLUSION, new ComponentWizardConclusionPage(handler, this));
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
    return component_;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizard::getBasePath()
//-----------------------------------------------------------------------------
QString const& ComponentWizard::getBasePath() const
{
    return basePath_;
}
