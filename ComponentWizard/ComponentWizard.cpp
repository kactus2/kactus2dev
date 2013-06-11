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

#include <QLabel>
#include <QVBoxLayout>

#include "ComponentWizardIntroPage.h"
#include "ComponentWizardFilesPage.h"
#include "ComponentWizardDependencyPage.h"
#include "ComponentWizardVhdlImportPage.h"
#include "ComponentWizardConclusionPage.h"

//-----------------------------------------------------------------------------
// Function: ComponentWizard::ComponentWizard()
//-----------------------------------------------------------------------------
ComponentWizard::ComponentWizard(QSharedPointer<Component> component,
	                             QString const& basePath,
	                             PluginManager& pluginMgr,
	                             LibraryInterface* handler,
	                             QWidget* parent)
    : QWizard(parent),
	  component_(component),
	  basePath_(basePath)
{
	setWindowTitle(tr("Component Wizard"));
    setWizardStyle(ModernStyle);
    setOption(NoCancelButton, true);
    setOption(HaveFinishButtonOnEarlyPages, true);
    resize(800, 600);

    setPage(PAGE_INTRO, new ComponentWizardIntroPage(this));
    setPage(PAGE_FILES, new ComponentWizardFilesPage(this));
    setPage(PAGE_DEPENDENCY, new ComponentWizardDependencyPage(pluginMgr, this));
    setPage(PAGE_VHDL_IMPORT, new ComponentWizardVhdlImportPage(component, handler, this));
    setPage(PAGE_CONCLUSION, new ComponentWizardConclusionPage(handler, this));
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
