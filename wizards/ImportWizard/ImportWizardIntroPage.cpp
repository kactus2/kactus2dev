//-----------------------------------------------------------------------------
// File: ImportWizardIntroPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 27.10.2014
//
// Description:
// Intro page for the import wizard.
//-----------------------------------------------------------------------------

#include "ImportWizardIntroPage.h"

#include "ImportWizardPages.h"

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ImportWizardIntroPage::ImportWizardIntroPage()
//-----------------------------------------------------------------------------
ImportWizardIntroPage::ImportWizardIntroPage(QWidget* parent)
    : QWizardPage(parent),
      infoLabel_(this)
{
    setTitle(tr("Introduction"));
    setSubTitle(tr("This wizard will guide you through the steps of importing file contents to a component."));

    QString infoText = tr("This wizard contains the following steps:");    
    infoText += "<ul><li>" + tr("Import elements from file.");
    infoText += "<li>" + tr("Summary") + "</ul>";

    infoLabel_.setText(infoText);
    infoLabel_.setWordWrap(true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&infoLabel_);
}

//-----------------------------------------------------------------------------
// Function: ImportWizardIntroPage::~ImportWizardIntroPage()
//-----------------------------------------------------------------------------
ImportWizardIntroPage::~ImportWizardIntroPage()
{
}

//-----------------------------------------------------------------------------
// Function: ImportWizardIntroPage::nextId()
//-----------------------------------------------------------------------------
int ImportWizardIntroPage::nextId() const
{
    return ImportWizardPages::IMPORT;
}
