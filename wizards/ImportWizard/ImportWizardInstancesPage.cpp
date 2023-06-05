//-----------------------------------------------------------------------------
// File: ImportWizardInstancesPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 02.04.2019
//
// Description:
// Import wizard page for importing instance elements from a file.
//-----------------------------------------------------------------------------

#include "ImportWizardInstancesPage.h"

#include <wizards/ComponentWizard/InstanceData.h>
#include <wizards/ImportWizard/ImportWizardPages.h>
#include <wizards/ImportWizard/ImportInstancesEditor/ImportInstancesEditor.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ImportWizardInstancesPage::ImportWizardInstancesPage()
//-----------------------------------------------------------------------------
ImportWizardInstancesPage::ImportWizardInstancesPage(QSharedPointer<Component> workingComponent,
    LibraryInterface* libraryAccess, QWidget* parent):
QWizardPage(parent),
editor_(new ImportInstancesEditor(workingComponent, libraryAccess, this))
{
    setTitle(tr("Design from the imported source file"));
    setSubTitle(tr("Finds the implemented component instances in the imported source file."));
    setFinalPage(false);

    QVBoxLayout* pageLayout = new QVBoxLayout(this);
    pageLayout->addWidget(editor_);
}

//-----------------------------------------------------------------------------
// Function: ImportWizardInstancesPage::initializePage()
//-----------------------------------------------------------------------------
void ImportWizardInstancesPage::initializePage()
{
    QWizardPage::initializePage();

    QVariant instancesVariant = field(InstanceData::VERILOGINSTANCES);
    if (instancesVariant.canConvert<QVector<InstanceData::instanceData> >())
    {
        QVector<InstanceData::instanceData> instances =
            instancesVariant.value<QVector<InstanceData::instanceData> >();

        editor_->setupInstances(instances);
    }
}

//-----------------------------------------------------------------------------
// Function: ImportWizardInstancesPage::nextId()
//-----------------------------------------------------------------------------
int ImportWizardInstancesPage::nextId() const
{
    return ImportWizardPages::CONCLUSION;
}
