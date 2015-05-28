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
#include <QGridLayout>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: FileNameEditor::FileNameEditor()
//-----------------------------------------------------------------------------
FileNameEditor::FileNameEditor(QSharedPointer<File> file, QWidget* parent)
    : QGroupBox(tr("File name and path"), parent), 
      fileNameLabel_(file->getName(), this),
      descriptionEditor_(this),
      file_(file)
{
    setMaximumHeight(100);
    
    QLabel* nameLabel = new QLabel(tr("Name:"), this);
    nameLabel->setToolTip(tr("The name field contains an absolute or relative\n"
        "path to a file name or directory"));

    QLabel* descriptionLabel = new QLabel(tr("Description:"), this);

    descriptionEditor_.setToolTip("Set the description for the file");

    QGridLayout* topLayout = new QGridLayout(this);
    topLayout->addWidget(nameLabel, 0, 0, 1, 1);
    topLayout->addWidget(&fileNameLabel_, 0, 1, 1, 1);
    topLayout->addWidget(descriptionLabel, 1, 0, 1, 1, Qt::AlignTop);
    topLayout->addWidget(&descriptionEditor_, 1, 1, 1, 1);

    connect(&descriptionEditor_, SIGNAL(textChanged()), this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);
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

    disconnect(&descriptionEditor_, SIGNAL(textChanged()), this, SLOT(onDescriptionChanged()));

    descriptionEditor_.setPlainText(file_->getDescription());

    connect(&descriptionEditor_, SIGNAL(textChanged()), this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: filenameeditor::onDescriptionChanged()
//-----------------------------------------------------------------------------
void FileNameEditor::onDescriptionChanged()
{
    file_->setDescription(descriptionEditor_.toPlainText());
    emit contentChanged();
}
