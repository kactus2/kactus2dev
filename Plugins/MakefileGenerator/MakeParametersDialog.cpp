//-----------------------------------------------------------------------------
// File: MakeParametersDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 15.10.2015
//
// Description:
// Dialog for presenting parameters, which may be chosen for makefile generation.
//-----------------------------------------------------------------------------

#include "MakeParametersDialog.h"
#include "MakeConfiguration.h"

#include <QFormLayout>
#include <QCheckBox>
#include <QHeaderView>
#include <QDialogButtonBox>
#include <QGroupBox>

#define COLUMN_CHOOSE 0
#define COLUMN_INSTANCE 1
#define COLUMN_FILESET 2
#define COLUMN_COMPILER 3
#define COLUMN_FLAGS 4

//-----------------------------------------------------------------------------
// Function: MakeParametersDialog()
//-----------------------------------------------------------------------------
MakeParametersDialog::MakeParametersDialog(QSharedPointer<MakeConfiguration> configuration,
	QSharedPointer<QList<QSharedPointer<MakeFileData> > > parsedData, QWidget* parent) :
    configuration_(configuration), fileOutput_(new FileOutputWidget(configuration->getFileOuput())),
    generalWarningLabel_(new QLabel), QDialog(parent)
{
    // Create tree widget to display conflicts.
    QTreeWidget* conflictTree = createConflictTree(parsedData);

    // Expand it right away.
    conflictTree->expandAll();

    // Layout for things coming to the bottom part of the dialog.
    QHBoxLayout* bottomLayout = new QHBoxLayout();

    // Add Ok and cancel give the dialog results.
    QDialogButtonBox* dialogButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, 
        Qt::Horizontal);

    // Add things in order.
    bottomLayout->addWidget(generalWarningLabel_);
    bottomLayout->addWidget(dialogButtons);

    // Connect the buttons to correct slots.
    connect(dialogButtons, SIGNAL(accepted()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(dialogButtons, SIGNAL(rejected()), this, SLOT(reject()), Qt::UniqueConnection);

    // Signal the file output to react to the files.
    fileOutput_->onOutputFilesChanged();

    // Add things in order.
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(new QLabel("Detected stacks with redundant source files:"));
    mainLayout->addWidget(conflictTree);
    mainLayout->addWidget(fileOutput_);
    mainLayout->addLayout(bottomLayout);
}

//-----------------------------------------------------------------------------
// Function: ~MakeParametersDialog()
//-----------------------------------------------------------------------------
MakeParametersDialog::~MakeParametersDialog()
{
}

//-----------------------------------------------------------------------------
// Function: MakeParametersDialog::accept()
//-----------------------------------------------------------------------------
void MakeParametersDialog::accept()
{
    // Check it is sane.
    QString warning;
    if (!configuration_->validSelections(warning))
    {
        generalWarningLabel_->setText(warning);
        return;
    }

    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: MakeParametersDialog::onItemChanged()
//-----------------------------------------------------------------------------
void MakeParametersDialog::onItemChanged(QTreeWidgetItem *item, int column)
{
    // Must belong to the correct column.
    if (column != COLUMN_CHOOSE)
    {
        return;
    }

    // Must be able to obtain the corresponding data.
    QMap<QTreeWidgetItem*,QSharedPointer<MakeObjectData> >::iterator iter = objectMapping_.find(item);

    if (iter == objectMapping_.end())
    {
        return;
    }

    QSharedPointer<MakeObjectData> object = *iter;

    if (object)
    {
        // If found, set the corresponding state.
        object->isChosen = item->checkState(COLUMN_CHOOSE) == Qt::Checked;
    }
}

//-----------------------------------------------------------------------------
// Function: MakeParametersDialog::createConflictTree()
//-----------------------------------------------------------------------------
QTreeWidget* MakeParametersDialog::createConflictTree(QSharedPointer<QList<QSharedPointer<MakeFileData> > > parsedData)
{
    QTreeWidget* conflictTree = new QTreeWidget;

    // Conflict tree has 5 columns.
    conflictTree->setColumnCount(5);

    // The headers for each of the columns.
    QStringList headers;
    headers.append(tr("Choose"));
    headers.append(tr("Instance"));
    headers.append(tr("File set"));
    headers.append(tr("Compiler"));
    headers.append(tr("Flags"));

    // Put the headers in to effect.
    conflictTree->setHeaderLabels(headers);

    // Easier to see the different rows from one another.
    conflictTree->setAlternatingRowColors(true);

    // Connect the conflict tree, so that we may react to changes in it.
    connect(conflictTree, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
        this, SLOT(onItemChanged(QTreeWidgetItem*, int)), Qt::UniqueConnection);

    // Add eligible makefiles to the tree.
    foreach (QSharedPointer<MakeFileData> makeData, *parsedData)
    {
        // Must have at least one conflict. Else it takes needlessly space from tree.
        if (makeData->conflicts.size() < 1)
        {
            continue;
        }

        // The upmost level shall have the topmost instance of stack.
        QString topInstance = makeData->name;
        QTreeWidgetItem* instanceItem = new QTreeWidgetItem;
        // Set the name as the text, append to the tree structure.
        instanceItem->setText(0,topInstance);
        conflictTree->addTopLevelItem(instanceItem);
        // There shall be no other columns, so lets use the whole row.
        instanceItem->setFirstColumnSpanned(true);

        foreach (QSet<QSharedPointer<MakeObjectData> > conflictSet, makeData->conflicts)
        {
            // Sanity check: Must have at least two items in conflict.
            if (conflictSet.size() < 2)
            {
                continue;
            }

            // Individual fiels shall be the next level.
            QTreeWidgetItem* conflictItem = new QTreeWidgetItem;
            // Set the path as the text, append to the tree structure.
            conflictItem->setText(0, (*conflictSet.begin())->path + "/" + (*conflictSet.begin())->fileName);
            instanceItem->addChild(conflictItem);
            // There shall be no other columns, so lets use the whole row.
            conflictItem->setFirstColumnSpanned(true);

            // Append all interpretations of the file.
            foreach (QSharedPointer<MakeObjectData> partisipant, conflictSet)
            {
                // Append to the tree structure.
                QTreeWidgetItem* participantItem = new QTreeWidgetItem;
                conflictItem->addChild(participantItem);
                // Must map the object file data to the item.
                objectMapping_[participantItem] = partisipant;

                // Column about letting user to choose this specific object file for generation.
                if (partisipant->isChosen)
                {
                    participantItem->setCheckState(COLUMN_CHOOSE, Qt::Checked);
                }
                else
                {
                    participantItem->setCheckState(COLUMN_CHOOSE, Qt::Unchecked);
                }

                // Column telling the instance where the object file comes from.
                participantItem->setText(COLUMN_INSTANCE, partisipant->stackPart->instanceName);
                participantItem->setToolTip(COLUMN_INSTANCE, partisipant->stackPart->instanceName);

                // Column telling the file set where the object file comes from.
                participantItem->setText(COLUMN_FILESET, partisipant->fileSet->name());
                participantItem->setToolTip(COLUMN_FILESET, partisipant->fileSet->name());

                // Column telling the deducted compiler for the object file.
                participantItem->setText(COLUMN_COMPILER, partisipant->compiler);
                participantItem->setToolTip(COLUMN_COMPILER, partisipant->compiler);

                // Column telling the deducted compilation flags for the object file.
                participantItem->setText(COLUMN_FLAGS, partisipant->flags);
                participantItem->setToolTip(COLUMN_FLAGS, partisipant->flags);
            }
        }
    }

    // Refit the columns.
    conflictTree->resizeColumnToContents(COLUMN_CHOOSE);
    conflictTree->resizeColumnToContents(COLUMN_INSTANCE);
    conflictTree->resizeColumnToContents(COLUMN_FILESET);
    conflictTree->resizeColumnToContents(COLUMN_COMPILER);
    conflictTree->resizeColumnToContents(COLUMN_FLAGS);

    return conflictTree;
}