//-----------------------------------------------------------------------------
// File: filenameeditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 5.2.2011
//
// Description:
// FileNameEditor is a widget to display the name element in FileSet.
//-----------------------------------------------------------------------------

#include "filenameeditor.h"

#include <IPXACTmodels/file.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: FileNameEditor::FileNameEditor()
//-----------------------------------------------------------------------------
FileNameEditor::FileNameEditor(QSharedPointer<File> file, QWidget* parent)
    : QGroupBox(tr("File name and path"), parent), 
      fileNameLabel_(file->getName(), this),
      file_(file)
{
    // the layout for the name label and name line edit
	QHBoxLayout* nameLayout = new QHBoxLayout();

	// create the label
	QLabel* nameLabel = new QLabel(tr("Name:"), this);
	nameLabel->setToolTip(tr("The name field contains an absolute or relative\n"
		"path to a file name or directory"));
	nameLayout->addWidget(nameLabel, 0);
	nameLayout->addWidget(&fileNameLabel_, 1);
	nameLayout->setContentsMargins(0, 0, 0, 0);

	// the top layout for the whole widget
	QVBoxLayout* topLayout = new QVBoxLayout(this);

	topLayout->addLayout(nameLayout);
}

//-----------------------------------------------------------------------------
// Function: FileNameEditor::~FileNameEditor()
//-----------------------------------------------------------------------------
FileNameEditor::~FileNameEditor()
{

}

//-----------------------------------------------------------------------------
// Function: FileNameEditor::isValid()
//-----------------------------------------------------------------------------
bool FileNameEditor::isValid() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: FileNameEditor::refresh()
//-----------------------------------------------------------------------------
void FileNameEditor::refresh()
{
    fileNameLabel_.setText(file_->getName());
}
