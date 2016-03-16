//-----------------------------------------------------------------------------
// File: filegeneraleditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 6.2.2011
//
// Description:
// FileGeneralEditor is a widget to edit File's general settings.
//-----------------------------------------------------------------------------

#include "filegeneraleditor.h"

#include <common/utils.h>

#include <IPXACTmodels/Component/File.h>

#include <QLabel>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: filegeneraleditor::FileGeneralEditor()
//-----------------------------------------------------------------------------
FileGeneralEditor::FileGeneralEditor(QWidget *parent, QSharedPointer<File> file):
QGroupBox(tr("General options"), parent), 
file_(file),
logicalName_(this), 
logicalDefault_(tr("Generators can override logical name"), this),
structuralFile_(tr("File is structural RTL"), this),
includeFile_(tr("File is include file"), this),
externalDec_(tr("File contains external declarations"), this)
{
	Q_ASSERT_X(file_, "FileGeneralEditor constructor", "Null File-pointer given for constructor");

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
// Function: filegeneraleditor::~FileGeneralEditor()
//-----------------------------------------------------------------------------
FileGeneralEditor::~FileGeneralEditor()
{

}

//-----------------------------------------------------------------------------
// Function: filegeneraleditor::refresh()
//-----------------------------------------------------------------------------
void FileGeneralEditor::refresh()
{
	logicalName_.setText(file_->getLogicalName());

    logicalDefault_.setChecked(file_->isLogicalNameDefault());
    structuralFile_.setChecked(file_->isStructural());
    includeFile_.setChecked(file_->isIncludeFile());
    externalDec_.setChecked(file_->hasExternalDeclarations());
}

//-----------------------------------------------------------------------------
// Function: filegeneraleditor::onLogicalNameChanged()
//-----------------------------------------------------------------------------
void FileGeneralEditor::onLogicalNameChanged()
{
	file_->setLogicalName(logicalName_.text());
	file_->setLogicalNameDefault(logicalDefault_.isChecked());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: filegeneraleditor::onStructuralFileChanged()
//-----------------------------------------------------------------------------
void FileGeneralEditor::onStructuralFileChanged()
{
    file_->setStructural(structuralFile_.isChecked());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: filegeneraleditor::onIncludeFileChanged()
//-----------------------------------------------------------------------------
void FileGeneralEditor::onIncludeFileChanged()
{
	file_->setIncludeFile(includeFile_.isChecked());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: filegeneraleditor::onExternalDecChanged()
//-----------------------------------------------------------------------------
void FileGeneralEditor::onExternalDecChanged()
{
	file_->setExternalDeclarations(externalDec_.isChecked());
	emit contentChanged();
}
