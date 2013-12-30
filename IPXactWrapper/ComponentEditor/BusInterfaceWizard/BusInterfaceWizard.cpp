//-----------------------------------------------------------------------------
// File: BusInterfaceWizard.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.11.2013
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "BusInterfaceWizard.h"

#include <QLabel>
#include <QVBoxLayout>

#include "BusInterfaceWizardIntroPage.h"
#include "BusInterfaceWizardGeneralOptionsPage.h"
#include "BusInterfaceWizardBusDefinitionPage.h"
#include "BusInterfaceWizardPortMapPage.h"
#include "BusInterfaceWizardConclusionPage.h"

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizard::BusInterfaceWizard()
//-----------------------------------------------------------------------------
BusInterfaceWizard::BusInterfaceWizard(QSharedPointer<Component> component, QSharedPointer<BusInterface> busIf,
    LibraryInterface* handler,    
    QWidget* parent)
    : QWizard(parent),
    component_(component),
    busIf_(busIf),
    handler_(handler),
    ports_(),
    createdVLVN_(),
    namesFromDescription_(false)
{
    setWindowTitle(tr("Bus Interface Wizard"));
    setWizardStyle(ModernStyle);
    setOption(NoCancelButton, true);
    setOption(HaveFinishButtonOnEarlyPages, false);
    resize(900, 1000);

    setPage(PAGE_INTRO, new BusInterfaceWizardIntroPage(this));
    setPage(PAGE_VLNVSELECTION, new BusInterfaceWizardGeneralOptionsPage(component,  busIf, handler, this));
    setPage(PAGE_BUSDEFINITION, new BusInterfaceWizardBusEditorPage(component_, busIf, handler, this));
    setPage(PAGE_PORTMAP, new BusInterfaceWizardPortMapPage(component, busIf, handler, this));
    setPage(PAGE_SUMMARY, new BusInterfaceWizardConclusionPage(busIf, this));
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizard::~BusInterfaceWizard()
//-----------------------------------------------------------------------------
BusInterfaceWizard::~BusInterfaceWizard()
{
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizard::getPorts()
//-----------------------------------------------------------------------------
QStringList BusInterfaceWizard::getPorts() const
{
    return ports_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizard::setPorts()
//-----------------------------------------------------------------------------
void BusInterfaceWizard::setPorts(QStringList const& ports)
{
    ports_ = ports;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizard::getCreatedAbsDef()
//-----------------------------------------------------------------------------
VLNV BusInterfaceWizard::getCreatedAbsDef()
{
    return createdVLVN_;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizard::setCreatedAbsDef()
//-----------------------------------------------------------------------------
void BusInterfaceWizard::setCreatedAbsDef(VLNV& createdVLNV, bool useDescription)
{
    createdVLVN_ = createdVLNV;
    if (useDescription)
    {
        BusInterfaceWizardBusEditorPage* busPage = dynamic_cast<BusInterfaceWizardBusEditorPage*>(page(PAGE_BUSDEFINITION));
        Q_ASSERT(busPage);
        busPage->setSignalNaming(BusInterfaceWizardBusEditorPage::DESCRIPTION);
    }
}



