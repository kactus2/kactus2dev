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

#include <IPXACTmodels/fileset.h>
#include <IPXACTmodels/component.h>
#include <library/LibraryManager/libraryinterface.h>

#include <wizards/common/ComponentDiffWidget/ComponentDiffWidget.h>

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: ComponentWizardConclusionPage::ComponentWizardConclusionPage()
//-----------------------------------------------------------------------------
ComponentWizardConclusionPage::ComponentWizardConclusionPage(QSharedPointer<Component> component, 
    LibraryInterface* lh,  QWidget* parent)
    : QWizardPage(parent), 
      originalComponent_(component), 
      workingComponent_(),
      handler_(lh), 
      summaryWidget_(this), 
      vendorLabel_(new QLabel(this)), 
      libraryLabel_(new QLabel(this)), 
      nameLabel_(new QLabel(this)), 
      versionLabel_(new QLabel(this)), 
      hierarchyLabel_(new QLabel(this)), 
      firmnessLabel_(new QLabel(this)), 
      directoryLabel_(new QLabel(this)), 
      authorLabel_(new QLabel(this)), 
      filesetsLabel_(new QLabel(this)), 
      parametersLabel_(new QLabel(this)), 
      portsLabel_(new QLabel(this)), 
      viewsLabel_(new QLabel(this)), 
      descriptionLabel_(new QLabel(this)), 
      previewBox_(lh),
      diffView_(new ComponentDiffWidget(this))
{
    setTitle(tr("Summary"));
    setSubTitle(tr("You have successfully completed the component wizard. Verify the choices by clicking Finish."));
    setFinalPage(true);
    
    previewBox_.setFixedWidth(300);

    directoryLabel_->setWordWrap(true);
    descriptionLabel_->setWordWrap(true);
    
    setupLayout();
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
    VLNV* vlnv = workingComponent_->getVlnv();
    vendorLabel_->setText(vlnv->getVendor());
    libraryLabel_->setText(vlnv->getLibrary());
    nameLabel_->setText(vlnv->getName());
    versionLabel_->setText(vlnv->getVersion());    

    if (workingComponent_->getComponentImplementation() == KactusAttribute::KTS_HW)
    {
        hierarchyLabel_->setText(KactusAttribute::valueToString(workingComponent_->getComponentHierarchy()));
        firmnessLabel_->setText(KactusAttribute::valueToString(workingComponent_->getComponentFirmness()));
    }

	QString xmlPath = handler_->getPath(*workingComponent_->getVlnv());
    directoryLabel_->setText(xmlPath);  
    authorLabel_->setText(workingComponent_->getAuthor());

    if (workingComponent_->getFileSets().isEmpty())
    {
        filesetsLabel_->setText("No file sets specified.");
    }
    else
    {
        QString fileSets = "";

        foreach (QSharedPointer<FileSet> fileSet,  workingComponent_->getFileSets())
        {         
            fileSets.append(tr("%1,  %2 file(s)<br>").arg(fileSet->getName()).arg(fileSet->getFiles().count()));
        }

        filesetsLabel_->setText(fileSets.left(fileSets.lastIndexOf("<br")));
    }

    if (workingComponent_->getComponentImplementation() == KactusAttribute::KTS_HW)
    {
        parametersLabel_->setText(tr("%1 model parameter(s) created.").arg(workingComponent_->getModelParameters().size()));

        portsLabel_->setText(tr("%1 port(s) created.").arg(workingComponent_->getPorts().size()));     

        viewsLabel_->setText(tr("%1 view(s) created.").arg(workingComponent_->viewCount()));     
    }

    descriptionLabel_->setText(workingComponent_->getDescription());
    previewBox_.setComponent(workingComponent_);
    diffView_->setComponents(originalComponent_, workingComponent_);
}

//-----------------------------------------------------------------------------
// Function: ImportWizardConclusionPage::onComponentChanged()
//-----------------------------------------------------------------------------
void ComponentWizardConclusionPage::onComponentChanged(QSharedPointer<Component> component)
{
    workingComponent_ = component;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardConclusionPage::setupLayout()
//-----------------------------------------------------------------------------
void ComponentWizardConclusionPage::setupLayout()
{
    QGridLayout* layout = new QGridLayout();
    int row = 0;

    QLabel* vendorTitleLabel = new QLabel("<b>Vendor:</b>",  this);
    layout->addWidget(vendorTitleLabel, row, 0, 1, 1, Qt::AlignTop);    
    layout->addWidget(vendorLabel_, row, 1, 1, 1);
    row++;

    QLabel* libraryTitleLabel = new QLabel("<b>Library:</b>",  this);
    layout->addWidget(libraryTitleLabel, row, 0, 1, 1, Qt::AlignTop);    
    layout->addWidget(libraryLabel_, row, 1, 1, 1);
    row++;

    QLabel* nameTitleLabel = new QLabel("<b>Name:</b>",  this);
    layout->addWidget(nameTitleLabel, row, 0, 1, 1, Qt::AlignTop);    
    layout->addWidget(nameLabel_, row, 1, 1, 1);
    row++;

    QLabel* versionTitleLabel = new QLabel("<b>Version:</b>",  this);
    layout->addWidget(versionTitleLabel, row, 0, 1, 1, Qt::AlignTop);    
    layout->addWidget(versionLabel_, row, 1, 1, 1);
    row++;

    if (originalComponent_->getComponentImplementation() == KactusAttribute::KTS_HW)
    {
        QLabel* hierarchyTitleLabel = new QLabel("<b>Product Hierarchy:</b>",  this);
        layout->addWidget(hierarchyTitleLabel, row, 0, 1, 1, Qt::AlignTop);    
        layout->addWidget(hierarchyLabel_, row, 1, 1, 1);
        row++;

        QLabel* firmnessTitleLabel = new QLabel("<b>Firmness:</b>",  this);
        layout->addWidget(firmnessTitleLabel, row, 0, 1, 1, Qt::AlignTop);    
        layout->addWidget(firmnessLabel_, row, 1, 1, 1);
        row++;
    }

    QLabel* directoryTitleLabel = new QLabel("<b>Directory:</b>",  this);
    layout->addWidget(directoryTitleLabel, row, 0, 1, 1, Qt::AlignTop);    
    layout->addWidget(directoryLabel_, row, 1, 1, 1);
    row++;

    QLabel* authorTitleLabel = new QLabel("<b>Author:</b>",  this);
    layout->addWidget(authorTitleLabel, row, 0, 1, 1, Qt::AlignTop);  
    layout->addWidget(authorLabel_, row, 1, 1, 1);
    row++;

    QLabel* descriptionTitleLabel = new QLabel("<b>Description:</b>",  this);
    layout->addWidget(descriptionTitleLabel, row, 0, 1, 1, Qt::AlignTop);    
    layout->addWidget(descriptionLabel_, row, 1, 1, 1);
    row++;

    QLabel* filesetTitleLabel = new QLabel("<b>File sets:</b>",  this);
    layout->addWidget(filesetTitleLabel, row, 0, 1, 1, Qt::AlignTop);    
    layout->addWidget(filesetsLabel_, row, 1, 1, 1);
    row++;

    if (originalComponent_->getComponentImplementation() == KactusAttribute::KTS_HW)
    {
        QLabel* parameterTitleLabel = new QLabel("<b>Model Parameters:</b>",  this);
        layout->addWidget(parameterTitleLabel, row, 0, 1, 1, Qt::AlignTop);    
        layout->addWidget(parametersLabel_, row, 1, 1, 1);
        row++;

        QLabel* portTitleLabel = new QLabel("<b>Ports:</b>",  this);
        layout->addWidget(portTitleLabel, row, 0, 1, 1, Qt::AlignTop);    
        layout->addWidget(portsLabel_, row, 1, 1, 1);   
        row++;

        QLabel* viewTitleLabel = new QLabel("<b>Views:</b>",  this);
        layout->addWidget(viewTitleLabel,  row, 0, 1, 1, Qt::AlignTop);    
        layout->addWidget(viewsLabel_, row, 1, 1, 1);   
        row++;        
    }

    layout->addWidget(diffView_, row, 0, 1, 3);

    layout->setRowStretch(row, 1);
    layout->setColumnStretch(1, 1);

    layout->addWidget(&previewBox_, 0, 2, row, 1);

    QHBoxLayout* topLayout = new QHBoxLayout(this);
    topLayout->addLayout(layout);

}
