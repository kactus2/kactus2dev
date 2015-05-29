//-----------------------------------------------------------------------------
// File: SaveHierarchyDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 21.05.2015
//
// Description:
// Dialog for hierarchical save setup.
//-----------------------------------------------------------------------------

#include "SaveHierarchyDialog.h"
#include "SaveHierarchyDelegate.h"

#include "HierarchicalSaveBuildStrategy.h"
#include "HierarchicalSaveColumns.h"

#include <library/LibraryManager/libraryinterface.h>

#include <common/widgets/LibrarySelectorWidget/LibrarySelectorWidget.h>

#include <IPXACTmodels/view.h>
#include <IPXACTmodels/vlnv.h>

#include <QVBoxLayout>
#include <QHeaderView>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: SaveHierarchyDialog::SaveHierarchyDialog()
//-----------------------------------------------------------------------------
SaveHierarchyDialog::SaveHierarchyDialog(QObject* rootObject, LibraryInterface* library, QWidget *parent)
    : QDialog(parent), documentSelectionView_(new QTreeWidget(this)), 
    directoryEditor_(new LibrarySelectorWidget(this)), 
    documentSelectionBuilder_(library)
{
    setWindowTitle(tr("Save hierarchy"));

    //directoryEditor_->layout()->setContentsMargins(0,0,0,0);
    directoryEditor_->setEnabled(false);

    documentSelectionView_->addTopLevelItem(documentSelectionBuilder_.build(rootObject));
    documentSelectionView_->setColumnCount(2);
    documentSelectionView_->expandAll();
    
    documentSelectionView_->setAlternatingRowColors(true);

    documentSelectionView_->setItemDelegate(new SaveHierarchyDelegate(library, this));

    QStringList selectionHeaders;
    selectionHeaders << tr("VLNV hierarchy") << tr("Save As");
    documentSelectionView_->setHeaderLabels(selectionHeaders);

    documentSelectionView_->setColumnWidth(0, 400);
    resize(800, 600);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: SaveHierarchyDialog::~SaveHierarchyDialog()
//-----------------------------------------------------------------------------
SaveHierarchyDialog::~SaveHierarchyDialog()
{

}

//-----------------------------------------------------------------------------
// Function: SaveHierarchyDialog::accept()
//-----------------------------------------------------------------------------
void SaveHierarchyDialog::accept()
{
    documentSelectionBuilder_.saveItem(documentSelectionView_->topLevelItem(0));

    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: SaveHierarchyDialog::setupLayout()
//-----------------------------------------------------------------------------
void SaveHierarchyDialog::setupLayout()
{
    QDialogButtonBox* dialogControls = new QDialogButtonBox(this);
    dialogControls->addButton(tr("Save"), QDialogButtonBox::AcceptRole);
    dialogControls->addButton(QDialogButtonBox::Cancel);

    QGroupBox* documentTreeGroup = new QGroupBox(tr("Select documents to save"), this);
    QVBoxLayout* documentLayout = new QVBoxLayout(documentTreeGroup); 
    documentLayout->addWidget(documentSelectionView_);

    // TODO: Implement different save scenarios before enabling selection.
    QGroupBox* directorySelectionGroup = new QGroupBox(tr("Select save location"), this);
    directorySelectionGroup->setEnabled(false);

    QRadioButton* currentButton = new QRadioButton(tr("Save in current directories."), directorySelectionGroup);
    currentButton->setChecked(true);

    QRadioButton* newRootButton = new QRadioButton(tr("Save under new root directory."), directorySelectionGroup);
 
    QVBoxLayout* selectionLayout = new QVBoxLayout(directorySelectionGroup); 
    selectionLayout->addWidget(currentButton);
    selectionLayout->addWidget(newRootButton);
    selectionLayout->addWidget(directoryEditor_);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(documentTreeGroup);
    topLayout->addWidget(directorySelectionGroup);
    topLayout->addWidget(dialogControls);

    connect(dialogControls, SIGNAL(accepted()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(dialogControls, SIGNAL(rejected()), this, SLOT(reject()), Qt::UniqueConnection);

    connect(newRootButton, SIGNAL(toggled(bool)), directoryEditor_, SLOT(setEnabled(bool)), Qt::UniqueConnection);
}
