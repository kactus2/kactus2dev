//-----------------------------------------------------------------------------
// File: ComponentWizardIntroPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.06.2013
//
// Description:
// Intro page for the component wizard.
//-----------------------------------------------------------------------------

#include "ComponentWizardIntroPage.h"

#include "ComponentWizard.h"

#include <QVBoxLayout>

#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>
#include <IPXACTmodels/component.h>

//-----------------------------------------------------------------------------
// Function: ComponentWizardIntroPage::ComponentWizardIntroPage()
//-----------------------------------------------------------------------------
ComponentWizardIntroPage::ComponentWizardIntroPage(ComponentWizard* parent)
    : QWizardPage(parent),
      parent_(parent),
      infoLabel_(this)
{
    setTitle(tr("Introduction"));
    setSubTitle(tr("This wizard will guide you through the basic steps of creating a component."));
    setFinalPage(true);

    QString infoText = tr("This wizard contains the following steps:");
    infoText += "<ul><li>" + tr("Component General Information");
    infoText += "<li>" + tr("Add Files");
    infoText += "<li>" + tr("Dependency Analysis & File Sets");

    if (parent_->getComponent()->getComponentImplementation() == KactusAttribute::KTS_HW)
    {
        infoText += "<li>" + tr("Import VHDL");
    }

    infoText += "<li>" + tr("Summary") + "</ul>";
    infoText += "<br>" + tr("The wizard can be skipped by clicking Finish.");

    infoLabel_.setText(infoText);
    infoLabel_.setWordWrap(true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&infoLabel_);
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardIntroPage::~ComponentWizardIntroPage()
//-----------------------------------------------------------------------------
ComponentWizardIntroPage::~ComponentWizardIntroPage()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardIntroPage::nextId()
//-----------------------------------------------------------------------------
int ComponentWizardIntroPage::nextId() const
{
    return ComponentWizard::PAGE_GENERAL;
}
