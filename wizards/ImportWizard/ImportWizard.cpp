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

#include <editors/ComponentEditor/common/MultipleParameterFinder.h>

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
      updatingFinder_(new ComponentParameterFinder(workingComponent_)),
      referenceCounter_(new ParameterReferenceCounter(updatingFinder_))
{
    QSharedPointer<MultipleParameterFinder> multiFinder(new MultipleParameterFinder());
    QSharedPointer<ParameterFinder> notImportedFinder(new ComponentParameterFinder(workingComponent_));
    multiFinder->addFinder(notImportedFinder);
    multiFinder->addFinder(updatingFinder_);

	setWindowTitle(tr("Import Wizard for %1").arg(component->getVlnv()->toString()));
    setWizardStyle(ModernStyle);
    resize(800, 1000);

    setOption(NoBackButtonOnStartPage, true);
    setOption(HaveFinishButtonOnEarlyPages, false);

    QSharedPointer<ExpressionFormatter> singleExpressionFormatter(new ExpressionFormatter(updatingFinder_));
    QSharedPointer<ExpressionFormatter> multipleExpressionFormatter(new ExpressionFormatter(multiFinder));

    ImportWizardImportPage* importPage = new ImportWizardImportPage(workingComponent_, handler, pluginMgr,
        updatingFinder_, singleExpressionFormatter, this);
    ComponentWizardConclusionPage* finalPage = new ComponentWizardConclusionPage(workingComponent_, handler,
        multipleExpressionFormatter, this);

    setPage(ImportWizardPages::INTRO, new ImportWizardIntroPage(this));
    setPage(ImportWizardPages::IMPORT, importPage);
    setPage(ImportWizardPages::CONCLUSION, finalPage);

    connect(importPage, SIGNAL(componentChanged(QSharedPointer<Component>)), 
        this, SLOT(onComponentChanged(QSharedPointer<Component>)), Qt::UniqueConnection);
    connect(importPage, SIGNAL(componentChanged(QSharedPointer<Component>)), 
        finalPage, SLOT(onComponentChanged(QSharedPointer<Component>)), Qt::UniqueConnection);

    connect(importPage, SIGNAL(increaseReferences(QString)),
        referenceCounter_.data(), SLOT(increaseReferenceCount(QString)), Qt::UniqueConnection);
    connect(importPage, SIGNAL(decreaseReferences(QString)),
        referenceCounter_.data(), SLOT(decreaseReferenceCount(QString)), Qt::UniqueConnection);
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
    updatingFinder_->setComponent(workingComponent_);
}
