//-----------------------------------------------------------------------------
// File: filegeneraleditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 6.2.2011
//
// Description:
// FileGeneralEditor is a widget to edit File's general settings.
//-----------------------------------------------------------------------------

#include "filegeneraleditor.h"

#include <KactusAPI/include/utils.h>

#include <IPXACTmodels/Component/File.h>

#include <KactusAPI/include/FileInterface.h>

#include <QLabel>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: filegeneraleditor::FileGeneralEditor()
//-----------------------------------------------------------------------------
FileGeneralEditor::FileGeneralEditor(QWidget *parent, std::string const& fileName, FileInterface* fileInterface):
QGroupBox(tr("General options"), parent), 
fileName_(fileName),
logicalName_(this), 
logicalDefault_(tr("Generators can override logical name"), this),
structuralFile_(tr("File is structural RTL"), this),
includeFile_(tr("File is include file"), this),
externalDec_(tr("File contains external declarations"), this),
fileInterface_(fileInterface)
{
	QLabel* logicNameLabel = new QLabel(tr("Logical name:"), this);
	logicNameLabel->setToolTip(tr("Logical name for the file or directory.\n"
		"For example VHDL library name for a vhdl-file"));

    QVBoxLayout* topLayout = new QVBoxLayout(this);

    QHBoxLayout* nameLayout = new QHBoxLayout();
	nameLayout->addWidget(logicNameLabel);
	nameLayout->addWidget(&logicalName_);

    topLayout->addLayout(nameLayout);
    topLayout->addWidget(&logicalDefault_);
    topLayout->addWidget(&structuralFile_);
    topLayout->addWidget(&includeFile_);
	topLayout->addWidget(&externalDec_);

	connect(&logicalName_, SIGNAL(textEdited(const QString&)),
        this, SLOT(onLogicalNameChanged()), Qt::UniqueConnection);
	connect(&logicalDefault_, SIGNAL(clicked(bool)), this, SLOT(onLogicalNameChanged()), Qt::UniqueConnection);
    connect(&structuralFile_, SIGNAL(clicked(bool)), this, SLOT(onStructuralFileChanged()), Qt::UniqueConnection);
	connect(&includeFile_, SIGNAL(clicked(bool)), this, SLOT(onIncludeFileChanged()), Qt::UniqueConnection);
	connect(&externalDec_, SIGNAL(clicked(bool)), this, SLOT(onExternalDecChanged()), Qt::UniqueConnection);

	refresh();
}

//-----------------------------------------------------------------------------
// Function: filegeneraleditor::refresh()
//-----------------------------------------------------------------------------
void FileGeneralEditor::refresh()
{
    logicalName_.setText(QString::fromStdString(fileInterface_->getLogicalName(fileName_)));

    logicalDefault_.setChecked(fileInterface_->isLogicalNameDefault(fileName_));
    structuralFile_.setChecked(fileInterface_->isStructural(fileName_));
    includeFile_.setChecked(fileInterface_->isIncludeFile(fileName_));
    externalDec_.setChecked(fileInterface_->hasExternalDeclarations(fileName_));
}

//-----------------------------------------------------------------------------
// Function: filegeneraleditor::onLogicalNameChanged()
//-----------------------------------------------------------------------------
void FileGeneralEditor::onLogicalNameChanged()
{
    fileInterface_->setLogicalName(fileName_, logicalName_.text().toStdString());
    fileInterface_->setLogicalNameDefault(fileName_, logicalDefault_.isChecked());

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: filegeneraleditor::onStructuralFileChanged()
//-----------------------------------------------------------------------------
void FileGeneralEditor::onStructuralFileChanged()
{
    fileInterface_->setStructural(fileName_, structuralFile_.isChecked());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: filegeneraleditor::onIncludeFileChanged()
//-----------------------------------------------------------------------------
void FileGeneralEditor::onIncludeFileChanged()
{
    fileInterface_->setIncludeFile(fileName_, includeFile_.isChecked());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: filegeneraleditor::onExternalDecChanged()
//-----------------------------------------------------------------------------
void FileGeneralEditor::onExternalDecChanged()
{
    fileInterface_->setExternalDeclarations(fileName_, externalDec_.isChecked());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FileGeneralEditor::fileRenamed()
//-----------------------------------------------------------------------------
void FileGeneralEditor::fileRenamed(std::string const& newName)
{
    fileName_ = newName;
}
