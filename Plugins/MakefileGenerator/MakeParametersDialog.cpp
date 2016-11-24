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

#include <QScrollArea>
#include <QFormLayout>
#include <QCheckBox>
#include <QHeaderView>
#include <QDialogButtonBox>
#include <QGroupBox>

#define COLUMN_CHOOSE 0

//-----------------------------------------------------------------------------
// Function: MakeParametersDialog()
//-----------------------------------------------------------------------------
MakeParametersDialog::MakeParametersDialog(QSharedPointer<MakeConfiguration> configuration,
	QSharedPointer<QList<QSharedPointer<MakeFileData> > > parsedData, QWidget* parent) :
    configuration_(configuration), fileOutput_(new FileOutputWidget(configuration->getFileOuput())),
    generalWarningLabel_(new QLabel), QDialog(parent)
{
	QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QWidget* widgetList = new QWidget;
	QVBoxLayout* listLayout = new QVBoxLayout(widgetList);

    // Layout for things coming to the bottom part of the dialog.
    QHBoxLayout* bottomLayout = new QHBoxLayout();

    // Add Ok and cancel give the dialog results.
    QDialogButtonBox* dialogButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, 
        Qt::Horizontal);

    bottomLayout->addWidget(generalWarningLabel_);
    bottomLayout->addWidget(dialogButtons);

    // Connect the buttons to correct slots.
    connect(dialogButtons, SIGNAL(accepted()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(dialogButtons, SIGNAL(rejected()), this, SLOT(reject()), Qt::UniqueConnection);

	foreach (QSharedPointer<MakeFileData> makeData, *parsedData)
	{
		QWidget* instanceWidget = new QWidget;
		listLayout->addWidget(instanceWidget);
		QVBoxLayout* instanceLayout = new QVBoxLayout(instanceWidget);

        QString topInstance = "Topmost instance: " + makeData->name;
		instanceLayout->addWidget(new QLabel(topInstance));

        if (makeData->conflicts.size() > 0)
        {
            createConflictTables(instanceLayout, makeData);
        }
    }

    scrollArea->setWidget(widgetList);

    fileOutput_->onOutputFilesChanged();

    // Add things in vertical order to two columns.
    QVBoxLayout* leftLayout = new QVBoxLayout();
    QVBoxLayout* rightLayout = new QVBoxLayout();
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    leftLayout->addWidget(new QLabel("Detected software stacks yielding makefiles:"));
    leftLayout->addWidget(scrollArea);

    rightLayout->addWidget(fileOutput_);
    rightLayout->addLayout(bottomLayout);

    mainLayout->addLayout(leftLayout);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(rightLayout);
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
void MakeParametersDialog::onItemChanged(QTableWidgetItem *item)
{
    // Must belong to the correct column.
    if (item->column() != COLUMN_CHOOSE)
    {
        return;
    }

    // Must be able to obtain the corresponding data.
    QMap<QTableWidgetItem*,QSharedPointer<MakeObjectData> >::iterator iter = objectMapping_.find(item);

    if (iter == objectMapping_.end())
    {
        return;
    }

    QSharedPointer<MakeObjectData> object = *iter;

    if (object)
    {
        // If found, set the corresponding state.
        object->isChosen = item->checkState() == Qt::Checked;
    }
}

//-----------------------------------------------------------------------------
// Function: MakeParametersDialog::createConflictTables()
//-----------------------------------------------------------------------------
void MakeParametersDialog::createConflictTables(QVBoxLayout* instanceLayout, QSharedPointer<MakeFileData> makeData)
{
    instanceLayout->addWidget(new QLabel(tr("File duplicates in the stack:"), this));

    foreach (QSet<QSharedPointer<MakeObjectData> > conflictSet, makeData->conflicts)
    {
        // Sanity check: Must have at least two items in conflict.
        if (conflictSet.size() < 2)
        {
            continue;
        }

        instanceLayout->addWidget(new QLabel((*conflictSet.begin())->fileName));

        //! Table used to display all detected conflicts and their participants.
        QTableWidget* conflictTable = new QTableWidget;
        instanceLayout->addWidget(conflictTable);

        // Conflict table has 5 columns.
        conflictTable->setColumnCount(5);

        // The headers for each of the columns.
        QStringList headers;
        headers.append(tr("Choose"));
        headers.append(tr("Instance"));
        headers.append(tr("File set"));
        headers.append(tr("Compiler"));
        headers.append(tr("Flags"));

        // Put the headers in to effect.
        conflictTable->setHorizontalHeaderLabels(headers);

        // Vertical headers are not used.
        conflictTable->verticalHeader()->hide();

        // Easier to see the different rows from one another.
        conflictTable->setAlternatingRowColors(true);

        // The last column is stretched take the available space in the widget.
        conflictTable->horizontalHeader()->setStretchLastSection(true);

        // Populate the table with conflict participants.
        int row = 0;
        int rowCount = conflictSet.size();

        conflictTable->setRowCount(rowCount);

        connect(conflictTable, SIGNAL(itemChanged(QTableWidgetItem*)),
            this, SLOT(onItemChanged(QTableWidgetItem*)), Qt::UniqueConnection);

        foreach (QSharedPointer<MakeObjectData> partisipant, conflictSet)
        {
            // Insert item corresponding the column to each cell of the row.

            // The user may choose here which of the items comes to the build.
            QTableWidgetItem* chooseItem = new QTableWidgetItem();

            if (partisipant->isChosen)
            {
                chooseItem->setCheckState(Qt::Checked);
            }
            else
            {
                chooseItem->setCheckState(Qt::Unchecked);
            }

            // Finally, set it to the table, and the mapping.
            conflictTable->setItem(row, COLUMN_CHOOSE, chooseItem);
            objectMapping_[chooseItem] = partisipant;

            QTableWidgetItem* instanceNameItem = new QTableWidgetItem(partisipant->stackPart->instanceName);
            instanceNameItem->setToolTip(partisipant->stackPart->instanceName);
            conflictTable->setItem(row, 1, instanceNameItem);
            instanceNameItem->setFlags(Qt::ItemIsSelectable);

            QTableWidgetItem* fileSetNameItem = new QTableWidgetItem(partisipant->fileSet->name());
            fileSetNameItem->setToolTip(partisipant->fileSet->name());
            conflictTable->setItem(row, 2, fileSetNameItem);
            fileSetNameItem->setFlags(Qt::ItemIsSelectable);

            QTableWidgetItem* compilerItem = new QTableWidgetItem(partisipant->compiler);
            conflictTable->setItem(row, 3, compilerItem);
            compilerItem->setFlags(Qt::ItemIsSelectable);

            QTableWidgetItem* flagsItem = new QTableWidgetItem(partisipant->flags);
            flagsItem->setToolTip(partisipant->flags);
            conflictTable->setItem(row, 4, flagsItem);
            flagsItem->setFlags(Qt::ItemIsSelectable);

            // The next one goes to the next row.
            ++row;
        }

        // Resize the height to match the contents: This table is not intended to be vertically scrolled.
        int nRowHeight = conflictTable->rowHeight(0);
        int nTableHeight = (rowCount * nRowHeight) + conflictTable->horizontalHeader()->height() +
            2 * conflictTable->frameWidth() + 20;
        conflictTable->setMinimumHeight(nTableHeight);
        conflictTable->setMaximumHeight(nTableHeight);

        // Refit the columns.
        conflictTable->resizeColumnsToContents();
    }
}