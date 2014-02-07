//-----------------------------------------------------------------------------
// File: BusInterfaceWizard.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.11.2013
//
// Description:
// Wizard for generating bus interface from a set of ports.
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
BusInterfaceWizard::BusInterfaceWizard(QSharedPointer<Component> component,  
    QSharedPointer<BusInterface> busIf,
    LibraryInterface* handler, 
    QStringList portNames,         
    QWidget* parent, 
    VLNV absDefVLNV, 
    bool descriptionAsLogicalName)
    : QWizard(parent)
{
    setWindowTitle(tr("Bus Interface Wizard"));
    setWizardStyle(ModernStyle);
    setOption(NoCancelButton, true);
    setOption(NoDefaultButton, true);
    setOption(HaveFinishButtonOnEarlyPages, false);
    resize(800, 800);

    BusInterfaceWizardBusEditorPage::SignalNamingPolicy namingPolicy = BusInterfaceWizardBusEditorPage::NAME;
    if (descriptionAsLogicalName)
    {
        namingPolicy = BusInterfaceWizardBusEditorPage::DESCRIPTION;
    }

    setPage(PAGE_INTRO, new BusInterfaceWizardIntroPage(this));
    setPage(PAGE_GENERALOPTIONS, new BusInterfaceWizardGeneralOptionsPage(component, busIf, handler, 
        !absDefVLNV.isValid(), this));
    setPage(PAGE_BUSDEFINITION, new BusInterfaceWizardBusEditorPage(component, busIf, handler, portNames, 
        this, absDefVLNV, namingPolicy));
    setPage(PAGE_PORTMAPS, new BusInterfaceWizardPortMapPage(component, busIf, handler, portNames, this));
    setPage(PAGE_SUMMARY, new BusInterfaceWizardConclusionPage(busIf, portNames, this));
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizard::~BusInterfaceWizard()
//-----------------------------------------------------------------------------
BusInterfaceWizard::~BusInterfaceWizard()
{
}
