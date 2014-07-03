//-----------------------------------------------------------------------------
// File: BusInterfaceWizardVLNVSelectionPage.cpp
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

#include <IPXACTmodels/component.h>
#include <library/LibraryManager/libraryinterface.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardVLNVSelectionPage::BusInterfaceWizardVLNVSelectionPage()
//-----------------------------------------------------------------------------
BusInterfaceWizardGeneralOptionsPage::BusInterfaceWizardGeneralOptionsPage(QSharedPointer<Component> component,
    QSharedPointer<BusInterface> busIf, LibraryInterface* lh,  bool absDefEditable, BusInterfaceWizard* parent) : 
    QWizardPage(parent),
    component_(component),
    busIf_(busIf),
    handler_(lh),
    newBus_(true),
    generalTab_(new BusIfGeneralTab(lh, busIf, component, this, parent))
{
    setTitle(tr("Bus interface general options"));
    setSubTitle(tr("Setup the general options for bus interface."));
    setFinalPage(false);

    generalTab_->setAbsTypeMandatory(true);
    generalTab_->setBusTypesLock(!absDefEditable);

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
// Function: BusInterfaceWizardVLNVSelectionPage::validatePage()
//-----------------------------------------------------------------------------
bool BusInterfaceWizardGeneralOptionsPage::validatePage()
{
    QStringList errors;
    bool valid = true;
 
   if (component_->hasInterface(busIf_->getName()) && 
        component_->getBusInterface(busIf_->getName()) != busIf_)
    {
        errors.append(tr("Component %1 already has interface named %2.").arg(component_->getVlnv()->toString(), 
            busIf_->getName()));
        valid = false;
    }    
    
    valid = generalTab_->isValid(errors) && valid;
    if (!valid)
    {
        QMessageBox warningDialog(QMessageBox::Warning,
            tr("Warning"),
            tr("Interface definition has the following error(s):\n") + errors.join("\n"),
            QMessageBox::Ok,
            this);        
        warningDialog.exec();
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardVLNVSelectionPage::isComplete()
//-----------------------------------------------------------------------------
bool BusInterfaceWizardGeneralOptionsPage::isComplete()
{
    return generalTab_->isValid();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardVLNVSelectionPage::setupLayout()
//-----------------------------------------------------------------------------
void BusInterfaceWizardGeneralOptionsPage::setupLayout()
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);    
    topLayout->addWidget(generalTab_);
}

