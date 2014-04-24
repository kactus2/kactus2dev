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

#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardConclusionPage::BusInterfaceWizardConclusionPage()
//-----------------------------------------------------------------------------
BusInterfaceWizardConclusionPage::BusInterfaceWizardConclusionPage(QSharedPointer<BusInterface> busIf,
    QStringList portNames, 
    BusInterfaceWizard *parent)
    : QWizardPage(parent),
    busIf_(busIf),
    ports_(portNames),
    nameLabel_(this),
    modeLabel_(this),
    busDefLabel_(this),
    absDefLabel_(this),
    portMapLabel_(this)
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
    nameLabel_.setText(busIf_->getName());
    modeLabel_.setText(General::interfaceMode2Str(busIf_->getInterfaceMode()));
    busDefLabel_.setText(busIf_->getBusType().toString());
    absDefLabel_.setText(busIf_->getAbstractionType().toString());

    // Search through all ports to see which ones are mapped in port maps.
    QStringList mappedPorts;

    foreach(QString portName, ports_)
    {
        foreach(QSharedPointer<General::PortMap> portMap, busIf_->getPortMaps())
        {
            if (QString::compare(portName, portMap->physicalPort_) == 0 &&
                !mappedPorts.contains(portName))
            {
                mappedPorts.append(portName);                
            }
        }
    }
    portMapLabel_.setText(QString::number(mappedPorts.size()) + "/" + QString::number(ports_.size()));
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
    detailsLayout->addRow(tr("Name"), &nameLabel_);
    detailsLayout->addRow(tr("Mode"), &modeLabel_);
    detailsLayout->addRow(tr("Bus Definition"), &busDefLabel_);
    detailsLayout->addRow(tr("Abstraction Definition"), &absDefLabel_);
    detailsLayout->addRow(tr("Ports mapped"), &portMapLabel_);

    topLaout->addLayout(detailsLayout);
}