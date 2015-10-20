//-----------------------------------------------------------------------------
// File: CompileConflictDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 15.10.2015
//
// Description:
// Dialog for presenting files that conflict in compilation.
//-----------------------------------------------------------------------------

#include "CompileConflictDialog.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: CompileConflictDialog()
//-----------------------------------------------------------------------------
CompileConflictDialog::CompileConflictDialog(QVector<QSet<QSharedPointer<MakefileParser::MakeObjectData> > >& conflicts, QWidget* parent)
	: QDialog(parent), descLabel_(tr("Conflicting file configurations:"), this), conflictTable_( this )
{
    // Create the layout for the dialog.
    QPushButton* btnOK = new QPushButton(tr("Generate") , this);
    QPushButton* btnCancel = new QPushButton(tr("Cancel"), this);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(btnOK);
    buttonLayout->addWidget(btnCancel);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(&descLabel_);
	mainLayout->addWidget(&conflictTable_, 1);
	mainLayout->addLayout(buttonLayout);

	conflictTable_.setColumnCount( 6 );

	// the headers for the table
	QStringList headers;
	headers.append(tr("File name"));
	headers.append(tr("Instance"));
	headers.append(tr("File set"));
	headers.append(tr("Include file"));
	headers.append(tr("Compiler"));
	headers.append(tr("Flags"));
	conflictTable_.setHorizontalHeaderLabels(headers);

	// Vertical headers are not used.
	conflictTable_.verticalHeader()->hide();

	// Easier to see the different rows from one another.
	conflictTable_.setAlternatingRowColors(true);

	// The last column is stretched take the available space in the widget.
	conflictTable_.horizontalHeader()->setStretchLastSection(true);
	conflictTable_.setMinimumWidth(500);

	// No editing this table.
	conflictTable_.setEditTriggers(QAbstractItemView::NoEditTriggers);

	// Populate the table with conflict participants.
	int i = 0;
	// Every second conflict is highlighted.
	bool highLight = true;

	foreach ( QSet<QSharedPointer<MakefileParser::MakeObjectData> > conflictSet, conflicts )
	{
		conflictTable_.setRowCount( conflictSet.size() + conflictTable_.rowCount() );

		foreach ( QSharedPointer<MakefileParser::MakeObjectData> partisipant, conflictSet )
		{
			conflictTable_.setItem(i, 0, new QTableWidgetItem( partisipant->fileName ) );
			conflictTable_.setItem(i, 1, new QTableWidgetItem( partisipant->instanceName ) );
			conflictTable_.setItem(i, 2, new QTableWidgetItem( partisipant->fileSet->getName() ) );
			conflictTable_.setItem(i, 4, new QTableWidgetItem( partisipant->compiler ) );
			conflictTable_.setItem(i, 5, new QTableWidgetItem( partisipant->flags ) );

			// The boolean variable include file is presented as a check box.
			QTableWidgetItem* defCheckItem = new QTableWidgetItem();

			if ( partisipant->file->getIncludeFile() )
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
			conflictTable_.setItem(i, 3, defCheckItem );

			QBrush color = Qt::lightGray;

			if ( highLight )
			{
				conflictTable_.item(i, 0)->setBackground(color);
				conflictTable_.item(i, 1)->setBackground(color);
				conflictTable_.item(i, 2)->setBackground(color);
				conflictTable_.item(i, 3)->setBackground(color);
				conflictTable_.item(i, 4)->setBackground(color);
				conflictTable_.item(i, 5)->setBackground(color);
			}

			++i;
		}

		// Reverse highlight for next conflict.
		highLight = !highLight;
	}

    connect(btnOK, SIGNAL(clicked()), this, SLOT(accept()));
	connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));

	resize(800, sizeHint().height());
}

//-----------------------------------------------------------------------------
// Function: ~CompileConflictDialog()
//-----------------------------------------------------------------------------
CompileConflictDialog::~CompileConflictDialog()
{
}