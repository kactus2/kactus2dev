//-----------------------------------------------------------------------------
// File: BusInterfaceWizardGeneralOptionsPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.11.2013
//
// Description:
// Bus interface wizard page for bus interface general options.
//-----------------------------------------------------------------------------

#include "BusInterfaceWizardGeneralOptionsPage.h"
#include "BusInterfaceWizard.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>

#include <library/LibraryManager/libraryinterface.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardVLNVSelectionPage::BusInterfaceWizardVLNVSelectionPage()
//-----------------------------------------------------------------------------
BusInterfaceWizardGeneralOptionsPage::BusInterfaceWizardGeneralOptionsPage(QSharedPointer<Component> component,
    QSharedPointer<BusInterface> busIf, LibraryInterface* lh,  bool absDefEditable,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> ExpressionParser, 
    QSharedPointer<BusInterfaceValidator> validator,
    BusInterfaceWizard* parent): 
QWizardPage(parent),
    component_(component),
    busIf_(busIf),
    handler_(lh),
    validator_(validator),
    newBus_(true),
    generalTab_(new BusIfGeneralTab(lh, busIf, component, parameterFinder, expressionFormatter, ExpressionParser,
    this, parent))
{
    setTitle(tr("Bus interface general options"));
    setSubTitle(tr("Setup the general options for the bus interface."));
    setFinalPage(false);

    generalTab_->setAbsTypeMandatory(true);
    generalTab_->setBusTypesLock(!absDefEditable);

    connect(generalTab_, SIGNAL(contentChanged()), this, SIGNAL(completeChanged()), Qt::UniqueConnection);
    connect(generalTab_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(generalTab_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardVLNVSelectionPage::~BusInterfaceWizardVLNVSelectionPage()
//-----------------------------------------------------------------------------
BusInterfaceWizardGeneralOptionsPage::~BusInterfaceWizardGeneralOptionsPage()
{

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardVLNVSelectionPage::nextId()
//-----------------------------------------------------------------------------
int BusInterfaceWizardGeneralOptionsPage::nextId() const
{
    return BusInterfaceWizard::PAGE_BUSDEFINITION;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardVLNVSelectionPage::initializePage()
//-----------------------------------------------------------------------------
void BusInterfaceWizardGeneralOptionsPage::initializePage()
{
    generalTab_->refresh();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardVLNVSelectionPage::isComplete()
//-----------------------------------------------------------------------------
bool BusInterfaceWizardGeneralOptionsPage::isComplete() const
{
    if (component_->hasInterface(busIf_->name()) && component_->getBusInterface(busIf_->name()) != busIf_)
    {
        return false;
    }        

    return validator_->validate(busIf_);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardVLNVSelectionPage::setupLayout()
//-----------------------------------------------------------------------------
void BusInterfaceWizardGeneralOptionsPage::setupLayout()
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);    
    topLayout->addWidget(generalTab_);
}

