//-----------------------------------------------------------------------------
// File: ImportWizard.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 15.10.2014
//
// Description:
// Wizard for component import.
//-----------------------------------------------------------------------------

#include "ImportWizard.h"
#include "ImportWizardPages.h"
#include "ImportWizardIntroPage.h"
#include "ImportWizardImportPage.h"

#include <wizards/ComponentWizard/ComponentWizardConclusionPage.h>

#include <IPXACTmodels/component.h>


//-----------------------------------------------------------------------------
// Function: ImportWizard::ImportWizard()
//-----------------------------------------------------------------------------
ImportWizard::ImportWizard(QSharedPointer<const Component> component,
	                             PluginManager const& pluginMgr,
	                             LibraryInterface* handler,
	                             QWidget* parent)
    : QWizard(parent),
      workingComponent_(new Component(*component)),
      parameterFinder_(new ComponentParameterFinder(workingComponent_)),
      multipleParameterFinder_(new MultipleParameterFinder(workingComponent_))
{
	setWindowTitle(tr("Import Wizard for %1").arg(component->getVlnv()->toString()));
    setWizardStyle(ModernStyle);
    resize(800, 800);

    setOption(NoBackButtonOnStartPage, true);
    setOption(HaveFinishButtonOnEarlyPages, false);

    QSharedPointer<ExpressionFormatter> singleExpressionFormatter(new ExpressionFormatter(parameterFinder_));
    QSharedPointer<ExpressionFormatter> multipleExpressionFormatter(
        new ExpressionFormatter(multipleParameterFinder_));

    ImportWizardImportPage* importPage = new ImportWizardImportPage(workingComponent_, handler, pluginMgr,
        parameterFinder_, singleExpressionFormatter, this);
    ComponentWizardConclusionPage* finalPage = new ComponentWizardConclusionPage(workingComponent_, handler,
        multipleExpressionFormatter, this);

    setPage(ImportWizardPages::INTRO, new ImportWizardIntroPage(this));
    setPage(ImportWizardPages::IMPORT, importPage);
    setPage(ImportWizardPages::CONCLUSION, finalPage);

    connect(importPage, SIGNAL(componentChanged(QSharedPointer<Component>)), 
        this, SLOT(onComponentChanged(QSharedPointer<Component>)), Qt::UniqueConnection);
    connect(importPage, SIGNAL(componentChanged(QSharedPointer<Component>)), 
        finalPage, SLOT(onComponentChanged(QSharedPointer<Component>)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ImportWizard::~ImportWizard()
//-----------------------------------------------------------------------------
ImportWizard::~ImportWizard()
{

}

//-----------------------------------------------------------------------------
// Function: ImportWizard::getComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> ImportWizard::getComponent()
{
    return workingComponent_;
}

//-----------------------------------------------------------------------------
// Function: ImportWizard::onComponentChanged()
//-----------------------------------------------------------------------------
void ImportWizard::onComponentChanged(QSharedPointer<Component> component)
{
    workingComponent_ = component;

    parameterFinder_->setComponent(workingComponent_);
    multipleParameterFinder_->setComponent(workingComponent_);
}
