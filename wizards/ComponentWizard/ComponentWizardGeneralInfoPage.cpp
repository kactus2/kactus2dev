//-----------------------------------------------------------------------------
// File: ComponentWizardGeneralInfoPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.06.2013
//
// Description:
// General component information page for the component wizard.
//-----------------------------------------------------------------------------

#include "ComponentWizardGeneralInfoPage.h"
#include "ComponentWizardPages.h"

#include <QFormLayout>
#include <QLineEdit>
#include <QWizardPage>
#include <QSettings>

#include <common/utils.h>

//-----------------------------------------------------------------------------
// Function: ComponentWizardGeneralInfoPage::ComponentWizardGeneralInfoPage()
//-----------------------------------------------------------------------------
ComponentWizardGeneralInfoPage::ComponentWizardGeneralInfoPage(QSharedPointer<Component> component, 
	QWidget* parent)
    : QWizardPage(parent),
      component_(component),
      authorEditor_(this),
      descriptionEditor_(this)
{
    setTitle(tr("General information"));
    setSubTitle(tr("Fill in the general information of the component to create."));
    setFinalPage(true);

     Q_ASSERT(component);

    // Pre-assign username to author field.
    QSettings settings;
    authorEditor_.setText(settings.value("General/Username",  Utils::getCurrentUser()).toString());

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardGeneralInfoPage::~ComponentWizardGeneralInfoPage()
//-----------------------------------------------------------------------------
ComponentWizardGeneralInfoPage::~ComponentWizardGeneralInfoPage()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardGeneralInfoPage::nextId()
//-----------------------------------------------------------------------------
int ComponentWizardGeneralInfoPage::nextId() const
{
    return ComponentWizardPages::FILES;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardGeneralInfoPage::isComplete()
//-----------------------------------------------------------------------------
bool ComponentWizardGeneralInfoPage::isComplete() const
{
	return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardGeneralInfoPage::validatePage()
//-----------------------------------------------------------------------------
bool ComponentWizardGeneralInfoPage::validatePage()
{
    component_->setDescription(descriptionEditor_.toPlainText());
    component_->setAuthor(authorEditor_.text());

    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardGeneralInfoPage::setupLayout()
//-----------------------------------------------------------------------------
void ComponentWizardGeneralInfoPage::setupLayout()
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);

    QFormLayout* formLayout = new QFormLayout();     
    formLayout->addRow(tr("&Author:"), &authorEditor_);
    formLayout->addRow(tr("&Description:"), &descriptionEditor_);

    topLayout->addLayout(formLayout);  
}
