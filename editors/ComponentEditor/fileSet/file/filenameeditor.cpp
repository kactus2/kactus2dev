//-----------------------------------------------------------------------------
// File: filenameeditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 5.2.2011
//
// Description:
// FileNameEditor is a widget to display the name element in FileSet.
//-----------------------------------------------------------------------------

#include "filenameeditor.h"

#include <IPXACTmodels/Component/File.h>

#include <QFormLayout>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: FileNameEditor::FileNameEditor()
//-----------------------------------------------------------------------------
FileNameEditor::FileNameEditor(QSharedPointer<File> file, QWidget* parent) :
QGroupBox(tr("File name and location"), parent),
    fileNameLabel_(file->name(), this),
    descriptionEditor_(this),
    file_(file)
{
    setMaximumHeight(150);

    fileNameLabel_.setWordWrap(true);

    fileNameLabel_.setToolTip(tr("The name field contains an absolute or relative\n"
        "path to a file name or directory"));

    descriptionEditor_.setToolTip("Set the description for the file");
    descriptionEditor_.setTabChangesFocus(true);

    QFormLayout* topLayout = new QFormLayout(this);
    topLayout->addRow(tr("Name:"), &fileNameLabel_);
    topLayout->addRow(tr("Description:"), &descriptionEditor_);

    connect(&descriptionEditor_, SIGNAL(textChanged()), this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FileNameEditor::~FileNameEditor()
//-----------------------------------------------------------------------------
FileNameEditor::~FileNameEditor()
{

}

//-----------------------------------------------------------------------------
// Function: FileNameEditor::refresh()
//-----------------------------------------------------------------------------
void FileNameEditor::refresh()
{
    fileNameLabel_.setText(file_->name());

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
