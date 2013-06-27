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
#include "ComponentWizard.h"

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QIcon>
#include <QTextStream>
#include <QWizardPage>
#include <QSettings>

#include <common/widgets/vhdlParser/VhdlParserWidget.h>
#include <common/widgets/fileViewer/FileViewer.h>

//-----------------------------------------------------------------------------
// Function: ComponentWizardGeneralInfoPage::ComponentWizardGeneralInfoPage()
//-----------------------------------------------------------------------------
ComponentWizardGeneralInfoPage::ComponentWizardGeneralInfoPage(QSharedPointer<Component> component, 
	/*LibraryInterface* handler, */
	ComponentWizard* parent)
    : QWizardPage(parent),
      parent_(parent),
      component_(component),
      authorEditor_(this),
      descriptionEditor_(this),
      showHideButton_(new QPushButton(QIcon(":/icons/graphics/triangle_arrow_down.png"),"",this)),
      fileViewer_(new FileViewer(component, parent->getBasePath(), this))
{
    setTitle(tr("General information"));
    setSubTitle(tr("Fill in the general information of the component to create."));
    setFinalPage(true);

     Q_ASSERT(component);

    // Pre-assign username to author field.
    QSettings settings;
    authorEditor_.setText(settings.value("General/Username",getenv("USERNAME")).toString());

    // Hide file browsing.
	showHideButton_->setFlat(true);
	showHideButton_->setMaximumHeight(10);    
    showHideButton_->setVisible(false);
    fileViewer_->setVisible(false);

    connect(showHideButton_, SIGNAL(clicked()), this, SLOT(onShowHide()),Qt::UniqueConnection);

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
    return ComponentWizard::PAGE_FILES;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardGeneralInfoPage::initializePage()
//-----------------------------------------------------------------------------
void ComponentWizardGeneralInfoPage::initializePage() 
{    
    fileViewer_->refresh();
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardGeneralInfoPage::isComplete()
//-----------------------------------------------------------------------------
bool ComponentWizardGeneralInfoPage::isComplete() const {
	return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardGeneralInfoPage::validatePage()
//-----------------------------------------------------------------------------
bool ComponentWizardGeneralInfoPage::validatePage()
{
    component_->setDescription(descriptionEditor_.toPlainText());
    
    //TODO: author assingment
    
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardGeneralInfoPage::onShowHide()
//-----------------------------------------------------------------------------
void ComponentWizardGeneralInfoPage::onShowHide()
{
    if ( fileViewer_->isVisible() )
    {
        fileViewer_->setVisible(false);
        showHideButton_->setIcon(QIcon(":/icons/graphics/triangle_arrow_down.png"));
    }
    else
    {
        fileViewer_->setVisible(true);
        showHideButton_->setIcon(QIcon(":/icons/graphics/triangle_arrow_up.png"));
    }
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
    formLayout->addWidget(showHideButton_);

    QVBoxLayout* viewerLayout = new QVBoxLayout();
    viewerLayout->addWidget(fileViewer_);

    topLayout->addLayout(formLayout);  
    topLayout->addLayout(viewerLayout,1);
}