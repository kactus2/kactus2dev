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

#include <common/widgets/summaryLabel/summarylabel.h>

#include <IPXACTmodels/fileset.h>
#include <IPXACTmodels/component.h>
#include <library/LibraryManager/libraryinterface.h>

#include <wizards/common/ComponentDiffWidget/ComponentDiffWidget.h>
#include <wizards/common/ComponentComparator/ComponentComparator.h>
#include <wizards/common/IPXactDiff.h>

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSharedPointer>


//-----------------------------------------------------------------------------
// Function: ComponentWizardConclusionPage::ComponentWizardConclusionPage()
//-----------------------------------------------------------------------------
ComponentWizardConclusionPage::ComponentWizardConclusionPage(QSharedPointer<Component> component, 
    LibraryInterface* lh, QSharedPointer<ExpressionFormatter> expressionFormatter, QWidget* parent)
    : QWizardPage(parent), 
      originalComponent_(component), 
      workingComponent_(component),
      handler_(lh), 
      summaryWidget_(this), 
      vlnvLabel_(new QLabel(this)), 
      
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
      diffView_(new ComponentDiffWidget(expressionFormatter, this))
{
    setTitle(tr("Summary"));
    setSubTitle(tr("You have successfully completed the wizard. Verify the choices by clicking Finish."));
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
    updateComponentDetails();
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
// Function: ComponentWizardConclusionPage::updateComponentDetails()
//-----------------------------------------------------------------------------
void ComponentWizardConclusionPage::updateComponentDetails()
{
    vlnvLabel_->setText(workingComponent_->getVlnv()->toString());        

    if (workingComponent_->getComponentImplementation() == KactusAttribute::HW)
    {
        hierarchyLabel_->setText(KactusAttribute::valueToString(workingComponent_->getComponentHierarchy()));
        firmnessLabel_->setText(KactusAttribute::valueToString(workingComponent_->getComponentFirmness()));
    }

    QString xmlPath = handler_->getPath(*workingComponent_->getVlnv());
    directoryLabel_->setText(xmlPath);  
    authorLabel_->setText(workingComponent_->getAuthor());
    descriptionLabel_->setText(workingComponent_->getDescription());

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

    if (workingComponent_->getComponentImplementation() == KactusAttribute::HW)
    {
        ComponentComparator comparator;
        QList<QSharedPointer<IPXactDiff> > diff = comparator.diff(originalComponent_, workingComponent_);        

        DiffSummary modelParameterSummary = creteSummaryFor("model parameter", diff);
        parametersLabel_->setText(tr("%1 created, %2 removed, %3 modified.").arg(
            QString::number(modelParameterSummary.added),  
            QString::number(modelParameterSummary.removed),  
            QString::number(modelParameterSummary.modified)));             

        DiffSummary portSummary = creteSummaryFor("port", diff);
        portsLabel_->setText(tr("%1 created, %2 removed, %3 modified.").arg(
            QString::number(portSummary.added),  
            QString::number(portSummary.removed),  
            QString::number(portSummary.modified)));     

        DiffSummary viewSummary = creteSummaryFor("view", diff);
        viewsLabel_->setText(tr("%1 created, %2 removed, %3 modified.").arg(
            QString::number(viewSummary.added),  
            QString::number(viewSummary.removed),  
            QString::number(viewSummary.modified)));           
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardConclusionPage::creteSummaryFor()
//-----------------------------------------------------------------------------
ComponentWizardConclusionPage::DiffSummary ComponentWizardConclusionPage::creteSummaryFor(QString const& element,
    QList<QSharedPointer<IPXactDiff> > const& diffs) const
{
    DiffSummary summary;

    foreach(QSharedPointer<IPXactDiff> diff, diffs)
    {
        if (diff->element() == element)
        {
            if (diff->changeType() == IPXactDiff::ADD)
            {
                summary.added++;
            }
            else if (diff->changeType() == IPXactDiff::REMOVE)
            {
                summary.removed++;
            }
            else if(diff->changeType() == IPXactDiff::MODIFICATION)
            {
                summary.modified++;
            }
        }
    }

    return summary;
}

//-----------------------------------------------------------------------------
// Function: ComponentWizardConclusionPage::setupLayout()
//-----------------------------------------------------------------------------
void ComponentWizardConclusionPage::setupLayout()
{
    QGridLayout* layout = new QGridLayout(this);
    int rows = 0;

    SummaryLabel* detailsLabel = new SummaryLabel(QObject::tr("Summary"), this);
    layout->addWidget(detailsLabel, rows, 0, 1, 3, Qt::AlignCenter);    
    rows++;

    QLabel* vendorTitleLabel = new QLabel("<b>VLNV:</b>",  this);
    layout->addWidget(vendorTitleLabel, rows, 0, 1, 1, Qt::AlignTop);    
    layout->addWidget(vlnvLabel_, rows, 1, 1, 1);
    rows++;

    if (originalComponent_->getComponentImplementation() == KactusAttribute::HW)
    {
        QLabel* hierarchyTitleLabel = new QLabel("<b>Product Hierarchy:</b>",  this);
        layout->addWidget(hierarchyTitleLabel, rows, 0, 1, 1, Qt::AlignTop);    
        layout->addWidget(hierarchyLabel_, rows, 1, 1, 1);
        rows++;

        QLabel* firmnessTitleLabel = new QLabel("<b>Firmness:</b>",  this);
        layout->addWidget(firmnessTitleLabel, rows, 0, 1, 1, Qt::AlignTop);    
        layout->addWidget(firmnessLabel_, rows, 1, 1, 1);
        rows++;
    }

    QLabel* directoryTitleLabel = new QLabel("<b>File location:</b>",  this);
    layout->addWidget(directoryTitleLabel, rows, 0, 1, 1, Qt::AlignTop);    
    layout->addWidget(directoryLabel_, rows, 1, 1, 1);
    rows++;

    QLabel* authorTitleLabel = new QLabel("<b>Author:</b>",  this);
    layout->addWidget(authorTitleLabel, rows, 0, 1, 1, Qt::AlignTop);  
    layout->addWidget(authorLabel_, rows, 1, 1, 1);
    rows++;

    QLabel* descriptionTitleLabel = new QLabel("<b>Description:</b>",  this);
    layout->addWidget(descriptionTitleLabel, rows, 0, 1, 1, Qt::AlignTop);    
    layout->addWidget(descriptionLabel_, rows, 1, 1, 1);
    rows++;

    QLabel* filesetTitleLabel = new QLabel("<b>File sets:</b>",  this);
    layout->addWidget(filesetTitleLabel, rows, 0, 1, 1, Qt::AlignTop);    
    layout->addWidget(filesetsLabel_, rows, 1, 1, 1);
    rows++;

    if (originalComponent_->getComponentImplementation() == KactusAttribute::HW)
    {
        QLabel* parameterTitleLabel = new QLabel("<b>Model Parameters:</b>",  this);
        layout->addWidget(parameterTitleLabel, rows, 0, 1, 1, Qt::AlignTop);    
        layout->addWidget(parametersLabel_, rows, 1, 1, 1);
        rows++;

        QLabel* portTitleLabel = new QLabel("<b>Ports:</b>",  this);
        layout->addWidget(portTitleLabel, rows, 0, 1, 1, Qt::AlignTop);    
        layout->addWidget(portsLabel_, rows, 1, 1, 1);   
        rows++;

        QLabel* viewTitleLabel = new QLabel("<b>Views:</b>",  this);
        layout->addWidget(viewTitleLabel,  rows, 0, 1, 1, Qt::AlignTop);    
        layout->addWidget(viewsLabel_, rows, 1, 1, 1);   
        rows++;        
    }

    if (originalComponent_->getComponentImplementation() == KactusAttribute::HW)
    {    
        SummaryLabel* diffLabel = new SummaryLabel(QObject::tr("Details"), this);
        layout->addWidget(diffLabel, rows, 0, 1, 3, Qt::AlignCenter);    
        rows++;
        layout->addWidget(diffView_, rows, 0, 1, 3);
    }
    else
    {        
        diffView_->hide();
    }

    layout->addWidget(&previewBox_, 1, 2, rows - 2, 1);

    layout->setRowStretch(rows, 1);
    layout->setColumnStretch(1, 1);        
}
