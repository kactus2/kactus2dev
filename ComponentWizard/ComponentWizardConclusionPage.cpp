//-----------------------------------------------------------------------------
// File: ComponentWizardConclusionPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.06.2013
//
// Description:
// Conclusion page for the component wizard.
//-----------------------------------------------------------------------------

#include "ComponentWizardConclusionPage.h"

#include "ComponentWizard.h"

#include <models/fileset.h>
#include <models/component.h>

#include <QHBoxLayout>

//-----------------------------------------------------------------------------
// Function: ComponentWizardConclusionPage::ComponentWizardConclusionPage()
//-----------------------------------------------------------------------------
ComponentWizardConclusionPage::ComponentWizardConclusionPage(LibraryInterface* lh, ComponentWizard* parent)
    : QWizardPage(parent),
      parent_(parent),
      summaryLabel_(this),
      previewBox_(lh)
{
    setTitle(tr("Summary"));
    setSubTitle(tr("Verify the choices made in the wizard and click Finish."));
    setFinalPage(true);

    summaryLabel_.setWordWrap(true);
    previewBox_.setFixedWidth(300);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(&summaryLabel_, 1, Qt::AlignTop);
    layout->addWidget(&previewBox_);
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardConclusionPage::~ComponentWizardConclusionPage()
//-----------------------------------------------------------------------------
ComponentWizardConclusionPage::~ComponentWizardConclusionPage()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardConclusionPage::nextId()
//-----------------------------------------------------------------------------
int ComponentWizardConclusionPage::nextId() const
{
    // The wizard ends here.
    return -1;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardConclusionPage::initializePage()
//-----------------------------------------------------------------------------
void ComponentWizardConclusionPage::initializePage()
{
    QString summary = "You have successfully completed the component wizard. "
                      "The component will be created with the following details.<br><br>";

    if (parent_->getComponent()->getFileSets().isEmpty())
    {
        summary += "<b>No file sets specified.</b><br>";
    }
    else
    {
        summary += "<b>File Sets:</b><br>";

        foreach (QSharedPointer<FileSet> fileSet, parent_->getComponent()->getFileSets())
        {
            summary += tr("%1: %2 files<br>").arg(fileSet->getName()).arg(fileSet->getFiles().count());
        }
    }

    if (parent_->getComponent()->getComponentImplementation() == KactusAttribute::KTS_HW)
    {
        summary += "<br>";

        if (parent_->getComponent()->getModelParameters().isEmpty())
        {
            summary += "<b>No model parameters imported.</b>";
        }
        else
        {
            summary += tr("<b>%1 model parameters imported.</b>").arg(parent_->getComponent()->getModelParameters().size());
        }

        summary += "<br><br>";

        if (parent_->getComponent()->getModelParameters().isEmpty())
        {
            summary += "<b>No ports imported.</b>";
        }
        else
        {
            summary += tr("<b>%1 ports imported.</b>").arg(parent_->getComponent()->getPorts().size());
        }
    }

    summaryLabel_.setText(summary);

    previewBox_.setComponent(parent_->getComponent());
}
