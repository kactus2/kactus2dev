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
#include <QtWidgets/QScrollArea>

//-----------------------------------------------------------------------------
// Function: CompileConflictDialog()
//-----------------------------------------------------------------------------
MakeParametersDialog::MakeParametersDialog(QStringList replacedFiles,
	QSharedPointer<QList<QSharedPointer<MakeFileData> > > parsedData, QWidget* parent) :
	QDialog(parent,Qt::CustomizeWindowHint)
{

	QScrollArea* scrollArea = new QScrollArea(this);

	QWidget* widgetList = new QWidget;
	widgetList->resize(750, 800);
	QVBoxLayout* listLayout = new QVBoxLayout(widgetList);
	scrollArea->setWidget(widgetList);

	// The list displaying all files that will be replaced in the generation.
	QListWidget* replaceList = new QListWidget( this );

	// Add to the list the files that will be replaced.
	foreach ( QString file, replacedFiles )
	{
		replaceList->addItem( file );
	}

	// Add "ok" and "cancel" button in neat horizontal order.
	QPushButton* btnOK = new QPushButton(tr("Generate"), this);
	QPushButton* btnCancel = new QPushButton(tr("Cancel"), this);
	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->addStretch(1);
	buttonLayout->addWidget(btnOK);
	buttonLayout->addWidget(btnCancel);

	connect(btnOK, SIGNAL(clicked()), this, SLOT(accept()));
	connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));

	// Add things to main layout in vertical order.
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(new QLabel(tr("Following top component files will be overwritten in the generation:"),
		this));
	mainLayout->addWidget(replaceList, Qt::AlignTop);
	mainLayout->addLayout(buttonLayout);
	mainLayout->addWidget(scrollArea);

	foreach ( QSharedPointer<MakeFileData> makeData, *parsedData )
	{
		QWidget* instanceWidget = new QWidget;
		listLayout->addWidget(instanceWidget);
		QVBoxLayout* instanceLayout = new QVBoxLayout(instanceWidget);

		instanceLayout->addWidget(new QLabel(makeData->name, this));
		instanceLayout->addWidget(new QLabel(tr("Conflicting file configurations:"), this));

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
		conflictTable->setMinimumWidth(500);

		// No editing this table.
		conflictTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

		// Populate the table with conflict participants.
		int row = 0;

		foreach ( QSet<QSharedPointer<MakeObjectData> > conflictSet, makeData->conflicts )
		{
			conflictTable->setRowCount( conflictSet.size() + conflictTable->rowCount() );

			foreach ( QSharedPointer<MakeObjectData> partisipant, conflictSet )
			{
				// Insert item corresponding the column to each cell of the row.
				conflictTable->setItem(row, 0, new QTableWidgetItem( partisipant->fileName ) );
				conflictTable->setItem(row, 1, new QTableWidgetItem( partisipant->stackPart->instanceName ) );
				conflictTable->setItem(row, 2, new QTableWidgetItem( partisipant->fileSet->name() ) );
				conflictTable->setItem(row, 4, new QTableWidgetItem( partisipant->compiler ) );
				conflictTable->setItem(row, 5, new QTableWidgetItem( partisipant->flags ) );

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
	}

	resize(800, sizeHint().height());
}

//-----------------------------------------------------------------------------
// Function: ~CompileConflictDialog()
//-----------------------------------------------------------------------------
MakeParametersDialog::~MakeParametersDialog()
{
}
