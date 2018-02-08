//-----------------------------------------------------------------------------
// File: BusInterfaceWizardConclusionPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.12.2013
//
// Description:
// Bus interface wizard page for summary.
//-----------------------------------------------------------------------------

#include "BusInterfaceWizardConclusionPage.h"
#include "BusInterfaceWizard.h"

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <QFormLayout>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardConclusionPage::BusInterfaceWizardConclusionPage()
//-----------------------------------------------------------------------------
BusInterfaceWizardConclusionPage::BusInterfaceWizardConclusionPage(QSharedPointer<BusInterface> busIf,
    QStringList portNames, BusInterfaceWizard *parent):
QWizardPage(parent),
busIf_(busIf),
ports_(portNames),
nameLabel_(this),
modeLabel_(this),
busDefLabel_(this),
absDefLabel_(this)
{
    setTitle(tr("Summary"));
    setSubTitle(tr("You have successfully completed the interface wizard. Verify the choices by clicking Finish."));
    setFinalPage(true);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardConclusionPage::~BusInterfaceWizardConclusionPage()
//-----------------------------------------------------------------------------
BusInterfaceWizardConclusionPage::~BusInterfaceWizardConclusionPage()
{

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardConclusionPage::nextId()
//-----------------------------------------------------------------------------
int BusInterfaceWizardConclusionPage::nextId() const
{
    // This is the final page of the wizard.
    return -1;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardConclusionPage::initializePage()
//-----------------------------------------------------------------------------
void BusInterfaceWizardConclusionPage::initializePage()
{
    // Set label texts.
    nameLabel_.setText(busIf_->name());
    modeLabel_.setText(General::interfaceMode2Str(busIf_->getInterfaceMode()));
    busDefLabel_.setText(busIf_->getBusType().toString());

    QString abstractionText("");
    if (busIf_->getAbstractionTypes())
    {
        foreach (QSharedPointer<AbstractionType> abstraction, *busIf_->getAbstractionTypes())
        {
            if (abstraction->getAbstractionRef())
            {
                QString abstractionVLNVText = abstraction->getAbstractionRef()->toString();
                abstractionText.append(abstractionVLNVText);

                QStringList mappedPorts;

                foreach (QSharedPointer<PortMap> portMap, *abstraction->getPortMaps())
                {
                    if (portMap->getPhysicalPort() && ports_.contains(portMap->getPhysicalPort()->name_))
                    {
                        mappedPorts.append(portMap->getPhysicalPort()->name_);
                    }
                }

                abstractionText.append(" (" + QString::number(mappedPorts.size()) + "/" +
                    QString::number(ports_.size()) + " ports mapped)");

                if (abstraction != busIf_->getAbstractionTypes()->last())
                {
                    abstractionText.append("\n");
                }
            }
        }
    }

    absDefLabel_.setText(abstractionText);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardConclusionPage::setupLayout()
//-----------------------------------------------------------------------------
void BusInterfaceWizardConclusionPage::setupLayout()
{
    QVBoxLayout* topLaout = new QVBoxLayout(this);

    QString summary = tr("The bus interface will be created with the following details.<br><br>");
    topLaout->addWidget(new QLabel(summary));

    QFormLayout* detailsLayout = new QFormLayout();
    detailsLayout->addRow(tr("Name:"), &nameLabel_);
    detailsLayout->addRow(tr("Mode:"), &modeLabel_);
    detailsLayout->addRow(tr("Bus Definition:"), &busDefLabel_);
    detailsLayout->addRow(tr("Abstraction Definition(s):"), &absDefLabel_);

    topLaout->addLayout(detailsLayout);
}