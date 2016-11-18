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

//-----------------------------------------------------------------------------
// Function: MakeParametersDialog()
//-----------------------------------------------------------------------------
MakeParametersDialog::MakeParametersDialog(QSharedPointer<MakeConfiguration> configuration,
	QSharedPointer<QList<QSharedPointer<MakeFileData> > > parsedData, QWidget* parent) :
    configuration_(configuration), fileOutput_(new FileOutputWidget(configuration->getFileOuput())),
    generalWarningLabel_(new QLabel), QDialog(parent)
{
	QScrollArea* scrollArea = new QScrollArea;

	QWidget* widgetList = new QWidget;
	QVBoxLayout* listLayout = new QVBoxLayout(widgetList);

    // Generating launcher script is optional.

    // Optional generation features.
    QGroupBox* optionGroup = new QGroupBox("Optional features:");
    QHBoxLayout* optionLayout = new QHBoxLayout();
    optionGroup->setLayout(optionLayout);

    // Check to generate a launcher script.
    QCheckBox* addCheck = new QCheckBox("Generate launcher script");
    optionLayout->addWidget(addCheck);

    // Connect the check box to a slot.
    connect(addCheck, SIGNAL(stateChanged(int)), 
        this, SLOT(onLauncherGenerationStateChanged(int)), Qt::UniqueConnection);

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
            createConflictTable(instanceLayout, makeData);
        }
    }

    scrollArea->setWidget(widgetList);

    // Add things to main layout in vertical order.
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(new QLabel("Detected software stacks yielding makefiles:"));
    mainLayout->addWidget(scrollArea);
    mainLayout->addWidget(optionGroup);
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
// Function: MakeParametersDialog::createConflictTable()
//-----------------------------------------------------------------------------
void MakeParametersDialog::createConflictTable(QVBoxLayout* instanceLayout, QSharedPointer<MakeFileData> makeData)
{
    instanceLayout->addWidget(new QLabel(tr("File dublicates in the stack:"), this));

    //! Table used to display all detected conflicts and their participants.
    QTableWidget* conflictTable = new QTableWidget;
    instanceLayout->addWidget(conflictTable);

    // Conflict table has 6 columns.
    conflictTable->setColumnCount( 6 );

    // The headers for each of the columns.
    QStringList headers;
    headers.append(tr("File name"));
    headers.append(tr("Instance"));
    headers.append(tr("File set"));
    headers.append(tr("Include file"));
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

    // No editing this table.
    conflictTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Populate the table with conflict participants.
    int row = 0;

    foreach (QSet<QSharedPointer<MakeObjectData> > conflictSet, makeData->conflicts)
    {
        conflictTable->setRowCount(conflictSet.size() + conflictTable->rowCount());

        foreach (QSharedPointer<MakeObjectData> partisipant, conflictSet)
        {
            // Insert item corresponding the column to each cell of the row.
            conflictTable->setItem(row, 0, new QTableWidgetItem( partisipant->fileName ));
            conflictTable->setItem(row, 1, new QTableWidgetItem( partisipant->stackPart->instanceName ));
            conflictTable->setItem(row, 2, new QTableWidgetItem( partisipant->fileSet->name() ));
            conflictTable->setItem(row, 4, new QTableWidgetItem( partisipant->compiler ));
            conflictTable->setItem(row, 5, new QTableWidgetItem( partisipant->flags ));

            // The boolean variable include file is presented as a check box.
            QTableWidgetItem* defCheckItem = new QTableWidgetItem();

            if ( partisipant->file->isIncludeFile() )
            {
                defCheckItem->setCheckState(Qt::Checked);
            }
            else
            {
                defCheckItem->setCheckState(Qt::Unchecked);
            }

            // Disable editing.
            defCheckItem->setFlags( Qt::ItemIsSelectable );

            // Finally, set it to the table.
            conflictTable->setItem(row, 3, defCheckItem );

            // The next one goes to the next row.
            ++row;
        }
    }

    // Refit the columns.
    conflictTable->resizeColumnsToContents();
}

//-----------------------------------------------------------------------------
// Function: MakeParametersDialog::onInterfaceGenerationStateChanged()
//-----------------------------------------------------------------------------
void MakeParametersDialog::onLauncherGenerationStateChanged(int state)
{
    configuration_->setAddLauncher(state == Qt::Checked);
}