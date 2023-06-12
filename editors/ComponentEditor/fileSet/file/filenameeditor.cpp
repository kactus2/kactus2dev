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

#include <KactusAPI/include/FileInterface.h>

#include <IPXACTmodels/Component/File.h>

#include <QFormLayout>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: FileNameEditor::FileNameEditor()
//-----------------------------------------------------------------------------
FileNameEditor::FileNameEditor(std::string const& fileName, FileInterface* fileInterface, QWidget *parent):
QGroupBox(tr("File name and location"), parent),
fileNameLabel_(QString::fromStdString(fileName)),
descriptionEditor_(this),
fileName_(fileName),
fileInterface_(fileInterface)
{
    setFlat(true);

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
// Function: FileNameEditor::refresh()
//-----------------------------------------------------------------------------
void FileNameEditor::refresh()
{
    fileNameLabel_.setText(QString::fromStdString(fileName_));

    disconnect(&descriptionEditor_, SIGNAL(textChanged()), this, SLOT(onDescriptionChanged()));

    descriptionEditor_.setPlainText(
        QString::fromStdString(fileInterface_->getDescription(fileName_)));

    connect(&descriptionEditor_, SIGNAL(textChanged()), this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: filenameeditor::onDescriptionChanged()
//-----------------------------------------------------------------------------
void FileNameEditor::onDescriptionChanged()
{
    fileInterface_->setDescription(fileName_, descriptionEditor_.toPlainText().toStdString());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: filenameeditor::fileRenamed()
//-----------------------------------------------------------------------------
void FileNameEditor::fileRenamed(std::string const& newName)
{
    fileName_ = newName;
}
