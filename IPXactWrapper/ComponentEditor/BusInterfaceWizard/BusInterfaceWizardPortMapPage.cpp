//-----------------------------------------------------------------------------
// File: BusInterfaceWizardPortMapPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.11.2013
//
// Description:
// Bus interface wizard page for defining port maps.
//-----------------------------------------------------------------------------

#include "BusInterfaceWizardPortMapPage.h"

#include "BusInterfaceWizard.h"

#include <models/fileset.h>
#include <models/component.h>
#include <library/LibraryManager/libraryinterface.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardPortMapPage::BusInterfaceWizardPortMapPage()
//-----------------------------------------------------------------------------
BusInterfaceWizardPortMapPage::BusInterfaceWizardPortMapPage(QSharedPointer<Component> component, 
    QSharedPointer<BusInterface> busIf, LibraryInterface* lh, 
    QStringList physicalPorts,
    BusInterfaceWizard* parent)
    : QWizardPage(parent),      
    component_(component),
    busIf_(busIf),
    handler_(lh),
    portMapTab_(lh, component, busIf.data(), this)
{
    setTitle(tr("Port Maps"));
    setSubTitle(tr("Create port maps for interface %1.").arg(busIf->getName()));
    setFinalPage(false);
    
    portMapTab_.setPhysicalPorts(physicalPorts);
    connect(&portMapTab_, SIGNAL(errorMessage(const QString&)),
        this, SLOT(showErrorMessage(const QString&)), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardPortMapPage::~BusInterfaceWizardPortMapPage()
//-----------------------------------------------------------------------------
BusInterfaceWizardPortMapPage::~BusInterfaceWizardPortMapPage()
{
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardPortMapPage::nextId()
//-----------------------------------------------------------------------------
int BusInterfaceWizardPortMapPage::nextId() const
{
    return BusInterfaceWizard::PAGE_SUMMARY;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardPortMapPage::initializePage()
//-----------------------------------------------------------------------------
void BusInterfaceWizardPortMapPage::initializePage()
{
    portMapTab_.setAbsType(busIf_->getAbstractionType(), busIf_->getInterfaceMode());
    
    portMapTab_.refresh();    
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardPortMapPage::validatePage()
//-----------------------------------------------------------------------------
bool BusInterfaceWizardPortMapPage::validatePage()
{
    QStringList errors;
    bool valid = portMapTab_.isValid(errors);

    if (!valid)
    {
        QMessageBox warningDialog(QMessageBox::Warning,
            tr("Warning"),
            tr("Port maps has the following error(s):\n") + errors.join("\n"),
            QMessageBox::Ok,
            this);        
        warningDialog.exec();
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardPortMapPage::showErrorMessage()
//-----------------------------------------------------------------------------
void BusInterfaceWizardPortMapPage::showErrorMessage(const QString& msg)
{
    QMessageBox warningDialog(QMessageBox::Warning,
        tr("Warning"),
        msg,
        QMessageBox::Ok,
        this);        
    warningDialog.exec();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardPortMapPage::setupLayout()
//-----------------------------------------------------------------------------
void BusInterfaceWizardPortMapPage::setupLayout()
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(&portMapTab_);
}
