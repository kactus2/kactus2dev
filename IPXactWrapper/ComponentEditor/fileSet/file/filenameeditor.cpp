/* 
 *
 *  Created on: 5.2.2011
 *      Author: Antti Kamppi
 * 		filename: filenameeditor.cpp
 */

#include "filenameeditor.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

FileNameEditor::FileNameEditor(QWidget *parent, QSharedPointer<File> file)
    : QGroupBox(tr("File name and path"), parent), 
      nameEdit_(file->getName(), this),
      file_(file)
{
    // the layout for the name label and name line edit
	QHBoxLayout* nameLayout = new QHBoxLayout();

	// create the label
	QLabel* nameLabel = new QLabel(tr("Name:"), this);
	nameLabel->setToolTip(tr("The name field contains an absolute or relative\n"
		"path to a file name or directory"));
	nameLayout->addWidget(nameLabel, 0);
	nameLayout->addWidget(&nameEdit_, 1);
	nameLayout->setContentsMargins(0, 0, 0, 0);

	// the top layout for the whole widget
	QVBoxLayout* topLayout = new QVBoxLayout(this);

	topLayout->addLayout(nameLayout);

	// connect the signals informing of content changes.
// 	connect(&nameEdit_, SIGNAL(contentChanged()),
// 		    this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

FileNameEditor::~FileNameEditor()
{
}

bool FileNameEditor::isValid() const
{
    return true;
}

void FileNameEditor::refresh()
{
    nameEdit_.setText(file_->getName());
}
