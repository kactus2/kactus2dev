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
#include <wizards/ImportWizard/ImportWizardInstancesPage.h>

#include <KactusAPI/include/BusInterfaceInterface.h>
#include <KactusAPI/include/BusInterfaceInterfaceFactory.h>
#include <KactusAPI/include/MultipleParameterFinder.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/IPXactSystemVerilogParser.h>

#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: ImportWizard::ImportWizard()
//-----------------------------------------------------------------------------
ImportWizard::ImportWizard(QSharedPointer<Component> component, LibraryInterface* handler, QWidget* parent):
QWizard(parent),
workingComponent_(new Component(*component)),
updatingFinder_(new ComponentParameterFinder(workingComponent_)),
referenceCounter_(new ParameterReferenceCounter(updatingFinder_))
{
    QSharedPointer<MultipleParameterFinder> multiFinder(new MultipleParameterFinder());
    QSharedPointer<ParameterFinder> notImportedFinder(new ComponentParameterFinder(workingComponent_));
    multiFinder->addFinder(notImportedFinder);
    multiFinder->addFinder(updatingFinder_);

    setPixmap(QWizard::LogoPixmap, QPixmap(":icons/common/graphics/import.png"));
	setWindowTitle(tr("Import Wizard for %1").arg(component->getVlnv().toString()));
    setWizardStyle(ModernStyle);
    resize(800, 1000);

    setOption(NoBackButtonOnStartPage, true);
    setOption(HaveFinishButtonOnEarlyPages, false);

    QSharedPointer<ExpressionFormatter> singleExpressionFormatter(new ExpressionFormatter(updatingFinder_));
    QSharedPointer<ExpressionFormatter> multipleExpressionFormatter(new ExpressionFormatter(multiFinder));

    QSharedPointer<ExpressionParser> parser(new IPXactSystemVerilogParser(multiFinder));

    BusInterfaceInterface* busInterface = BusInterfaceInterfaceFactory::createBusInterface(
        multiFinder, multipleExpressionFormatter, parser, component, handler);

    ImportWizardImportPage* importPage = new ImportWizardImportPage(workingComponent_, handler, 
        updatingFinder_, singleExpressionFormatter, busInterface, this);

    ImportWizardInstancesPage* importInstancesPage =
        new ImportWizardInstancesPage(workingComponent_, handler, this);

    ComponentWizardConclusionPage* finalPage = new ComponentWizardConclusionPage(workingComponent_, handler,
        multipleExpressionFormatter, this);

    setPage(ImportWizardPages::INTRO, new ImportWizardIntroPage(this));
    setPage(ImportWizardPages::IMPORT, importPage);
    setPage(ImportWizardPages::INSTANCES, importInstancesPage);
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
