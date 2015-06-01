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

#include <QApplication>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QProgressBar>
#include <QRadioButton>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: SaveHierarchyDialog::SaveHierarchyDialog()
//-----------------------------------------------------------------------------
SaveHierarchyDialog::SaveHierarchyDialog(QObject* rootObject, LibraryInterface* library, QWidget *parent)
    : QDialog(parent), documentSelectionView_(new QTreeWidget(this)), saveProgressBar_(new QProgressBar(this)), 
    directoryEditor_(new LibrarySelectorWidget(this)), library_(library),
    documentSelectionBuilder_(library, this)
{
    setWindowTitle(tr("Save hierarchy"));

    documentSelectionView_->addTopLevelItem(documentSelectionBuilder_.build(rootObject));
    documentSelectionView_->setColumnCount(2);
    documentSelectionView_->expandAll();
    
    documentSelectionView_->setAlternatingRowColors(true);

    documentSelectionView_->setItemDelegate(new SaveHierarchyDelegate(library, this));

    QStringList selectionHeaders;
    selectionHeaders << tr("VLNV hierarchy") << tr("Save As");
    documentSelectionView_->setHeaderLabels(selectionHeaders);

    documentSelectionView_->setColumnWidth(0, 400);

    directoryEditor_->setEnabled(false);

    resize(800, 600);

    setupLayout();

    connect(&documentSelectionBuilder_, SIGNAL(itemSaved()), this, SLOT(onItemSaved()), Qt::UniqueConnection);
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
    QApplication::setOverrideCursor(Qt::WaitCursor);

    int checkedItems = findNumberOfCheckedItems(documentSelectionView_->topLevelItem(0));
    saveProgressBar_->setMaximum(checkedItems);

    if (directoryEditor_->isEnabled())
    {
        documentSelectionBuilder_.setSavePath(directoryEditor_->getPath());
    }

    library_->beginSave();
    documentSelectionBuilder_.saveItem(documentSelectionView_->topLevelItem(0));
    library_->endSave();

    QDialog::accept();

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: SaveHierarchyDialog::onSaveModeChanged()
//-----------------------------------------------------------------------------
void SaveHierarchyDialog::onSetSaveModeToCurrent(bool enabled)
{
    if (enabled)
    {
        documentSelectionBuilder_.setSaveMode(HierarchicalSaveBuildStrategy::CURRENT_DIRECTORY);
    }
}

//-----------------------------------------------------------------------------
// Function: SaveHierarchyDialog::onSetSaveModeToCommonRoot()
//-----------------------------------------------------------------------------
void SaveHierarchyDialog::onSetSaveModeToCommonRoot(bool enabled)
{
    if (enabled)
    {
        documentSelectionBuilder_.setSaveMode(HierarchicalSaveBuildStrategy::COMMON_ROOT_DIRECTORY);
    }
}

//-----------------------------------------------------------------------------
// Function: SaveHierarchyDialog::onSetSaveModeToSingleDirectory()
//-----------------------------------------------------------------------------
void SaveHierarchyDialog::onSetSaveModeToSingleDirectory(bool enabled)
{
    if (enabled)
    {
        documentSelectionBuilder_.setSaveMode(HierarchicalSaveBuildStrategy::SINGLE_DIRECTORY);
    }
}

//-----------------------------------------------------------------------------
// Function: SaveHierarchyDialog::onItemSaved()
//-----------------------------------------------------------------------------
void SaveHierarchyDialog::onItemSaved()
{
    saveProgressBar_->setValue(saveProgressBar_->value() +1);
}

//-----------------------------------------------------------------------------
// Function: SaveHierarchyDialog::numberOfCheckedItems()
//-----------------------------------------------------------------------------
int SaveHierarchyDialog::findNumberOfCheckedItems(QTreeWidgetItem* item)
{
    int count = 0;

    if (item->checkState(HierarchicalSaveColumns::SAVE_AS_VLNV) == Qt::Checked)
    {
        count++;

        int childCount = item->childCount();
        for (int i = 0; i < childCount; i++)
        {
            count += findNumberOfCheckedItems(item->child(i));
        }
    }	

    return count;
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

    QGroupBox* directorySelectionGroup = new QGroupBox(tr("Select save location"), this);

    QRadioButton* originalButton = new QRadioButton(tr("Save in original directories."), directorySelectionGroup);
    QRadioButton* newRootButton = new QRadioButton(tr("Save in subdirectories under a given root directory."), 
        directorySelectionGroup);
    QRadioButton* singleDirectoryButton = new QRadioButton(tr("Save all in one directory."), 
        directorySelectionGroup);
    originalButton->setChecked(true);

    QVBoxLayout* selectionLayout = new QVBoxLayout(directorySelectionGroup); 
    selectionLayout->addWidget(originalButton);
    selectionLayout->addWidget(newRootButton);
    selectionLayout->addWidget(singleDirectoryButton);
    selectionLayout->addWidget(directoryEditor_);

    saveProgressBar_->setAlignment(Qt::AlignCenter);
    saveProgressBar_->setMinimum(0);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(documentTreeGroup);
    topLayout->addWidget(directorySelectionGroup);
    topLayout->addWidget(saveProgressBar_);
    topLayout->addWidget(dialogControls);

    connect(dialogControls, SIGNAL(accepted()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(dialogControls, SIGNAL(rejected()), this, SLOT(reject()), Qt::UniqueConnection);

    connect(originalButton, SIGNAL(toggled(bool)), directoryEditor_, SLOT(setDisabled(bool)), Qt::UniqueConnection);

    connect(originalButton, SIGNAL(toggled(bool)), 
        this, SLOT(onSetSaveModeToCurrent(bool)), Qt::UniqueConnection);
    connect(newRootButton, SIGNAL(toggled(bool)), 
        this, SLOT(onSetSaveModeToCommonRoot(bool)), Qt::UniqueConnection);
    connect(singleDirectoryButton, SIGNAL(toggled(bool)), 
        this, SLOT(onSetSaveModeToSingleDirectory(bool)), Qt::UniqueConnection);
}
