//-----------------------------------------------------------------------------
// File: BusInterfaceWizardIntroPage
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.12.2013
//
// Description:
// Bus interface wizard page for introduction.
//-----------------------------------------------------------------------------

#include "BusInterfaceWizardIntroPage.h"
#include "BusInterfaceWizard.h"

#include <QVBoxLayout>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardIntroPage::BusInterfaceWizardIntroPage()
//-----------------------------------------------------------------------------
BusInterfaceWizardIntroPage::BusInterfaceWizardIntroPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Introduction"));
    setSubTitle(tr("This wizard will guide you through creating a new interface to the component."));
    setFinalPage(false);
    
    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardIntroPage::~BusInterfaceWizardIntroPage()
//-----------------------------------------------------------------------------
BusInterfaceWizardIntroPage::~BusInterfaceWizardIntroPage()
{
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardIntroPage::nextId()
//-----------------------------------------------------------------------------
int BusInterfaceWizardIntroPage::nextId() const
{
    return BusInterfaceWizard::PAGE_GENERALOPTIONS;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardIntroPage::setupLayout()
//-----------------------------------------------------------------------------
void BusInterfaceWizardIntroPage::setupLayout()
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);

    QString infoText = tr("This wizard contains the following steps:");
    infoText += "<ul><li>" + tr("Bus Interface General Information");
    infoText += "<li>" + tr("Bus Editor");
    infoText += "<li>" + tr("Port Maps");
    infoText += "<li>" + tr("Summary") + "</ul>";

    QLabel* introTextLabel = new QLabel(infoText);
    
    topLayout->addWidget(introTextLabel);
}


