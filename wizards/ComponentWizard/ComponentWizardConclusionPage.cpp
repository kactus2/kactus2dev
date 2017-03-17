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

#include <library/LibraryInterface.h>

#include <wizards/common/ComponentDiffWidget/ComponentDiffWidget.h>
#include <wizards/common/ComponentComparator/ComponentComparator.h>
#include <wizards/common/IPXactDiff.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSharedPointer>


//-----------------------------------------------------------------------------
// Function: ComponentWizardConclusionPage::ComponentWizardConclusionPage()
//-----------------------------------------------------------------------------
ComponentWizardConclusionPage::ComponentWizardConclusionPage(QSharedPointer<Component> component, 
    LibraryInterface* lh, QSharedPointer<ExpressionFormatter> expressionFormatter, QWidget* parent):
QWizardPage(parent),
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
previewBox_(lh, this),
diffView_(new ComponentDiffWidget(expressionFormatter, this))
{
    setTitle(tr("Summary"));
    setSubTitle(tr("You have successfully completed the wizard. Verify the choices by clicking Finish."));
    setFinalPage(true);
    
    //previewBox_.setFixedWidth(300);

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
    vlnvLabel_->setText(workingComponent_->getVlnv().toString());        

    if (workingComponent_->getImplementation() == KactusAttribute::HW)
    {
        hierarchyLabel_->setText(KactusAttribute::hierarchyToString(workingComponent_->getHierarchy()));
        firmnessLabel_->setText(KactusAttribute::firmnessToString(workingComponent_->getFirmness()));
    }

    QString xmlPath = handler_->getPath(workingComponent_->getVlnv());
    directoryLabel_->setText(xmlPath);  
    authorLabel_->setText(workingComponent_->getAuthor());
    descriptionLabel_->setText(workingComponent_->getDescription());

    if (workingComponent_->getFileSets()->isEmpty())
    {
        filesetsLabel_->setText("No file sets specified.");
    }
    else
    {
        QString fileSets = "";

        foreach (QSharedPointer<FileSet> fileSet,  *workingComponent_->getFileSets())
        {         
            fileSets.append(tr("%1,  %2 file(s)<br>").arg(fileSet->name()).arg(fileSet->getFiles()->count()));
        }

        filesetsLabel_->setText(fileSets.left(fileSets.lastIndexOf("<br")));
    }

    if (workingComponent_->getImplementation() == KactusAttribute::HW)
    {
        ComponentComparator comparator;
        QList<QSharedPointer<IPXactDiff> > diff = comparator.diff(originalComponent_, workingComponent_);        

        DiffSummary parameterSummary = creteSummaryFor("parameter", diff);
        parametersLabel_->setText(tr("%1 created, %2 removed, %3 modified.").arg(
            QString::number(parameterSummary.added), QString::number(parameterSummary.removed),
            QString::number(parameterSummary.modified)));     

        DiffSummary portSummary = creteSummaryFor("port", diff);
        portsLabel_->setText(tr("%1 created, %2 removed, %3 modified.").arg(QString::number(portSummary.added),
            QString::number(portSummary.removed), QString::number(portSummary.modified)));     

        DiffSummary viewSummary = creteSummaryFor("view", diff);
        viewsLabel_->setText(tr("%1 created, %2 removed, %3 modified.").arg(QString::number(viewSummary.added),
            QString::number(viewSummary.removed), QString::number(viewSummary.modified)));           
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
    QFormLayout* leftLayout = new QFormLayout();

    leftLayout->addRow(tr("<b>VLNV:</b>"), vlnvLabel_);

    if (originalComponent_->getImplementation() == KactusAttribute::HW)
    {
        leftLayout->addRow(tr("<b>Product Hierarchy:</b>"), hierarchyLabel_);
        leftLayout->addRow(tr("<b>Firmness:</b>"), firmnessLabel_);
    }

    leftLayout->addRow(tr("<b>File location:</b>"), directoryLabel_);
    leftLayout->addRow(tr("<b>Author:</b>"), authorLabel_);
    leftLayout->addRow(tr("<b>Description:</b>"), descriptionLabel_);
    leftLayout->addRow(tr("<b>File sets:</b>"), filesetsLabel_);

    if (originalComponent_->getImplementation() == KactusAttribute::HW)
    {
        leftLayout->addRow(tr("<b>Parameters:</b>"), parametersLabel_);
        leftLayout->addRow(tr("<b>Ports:</b>"), portsLabel_);
        leftLayout->addRow(tr("<b>Views:</b>"), viewsLabel_);
    }

    QGridLayout* layout = new QGridLayout(this);

    SummaryLabel* summaryLabel = new SummaryLabel(tr("Summary"), this);
    layout->addWidget(summaryLabel, 0, 0, 1, 2, Qt::AlignCenter);    

    layout->addLayout(leftLayout, 1, 0, 1, 1);
    layout->addWidget(&previewBox_, 1, 1, 1, 1);

    QWidget* detailsLabel = new SummaryLabel(tr("Details"), this);
    layout->addWidget(detailsLabel, 2, 0, 1, 2, Qt::AlignCenter);
    layout->addWidget(diffView_, 3, 0, 1, 2);
          
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);

    bool showDetails = originalComponent_->getImplementation() == KactusAttribute::HW;
    detailsLabel->setVisible(showDetails);
    diffView_->setVisible(showDetails);  
}
