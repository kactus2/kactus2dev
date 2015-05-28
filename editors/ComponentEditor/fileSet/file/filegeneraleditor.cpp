/* 
 *
 *  Created on: 6.2.2011
 *      Author: Antti Kamppi
 * 		filename: filegeneraleditor.cpp
 */

#include "filegeneraleditor.h"

#include <common/utils.h>

#include <QLabel>
#include <QGridLayout>

FileGeneralEditor::FileGeneralEditor(QWidget *parent, 
									 QSharedPointer<File> file): 
QGroupBox(tr("General options"), parent), 
file_(file),
logicalName_(this), 
logicalDefault_(tr("Generators can override logical name"), this),
includeFile_(tr("File is include file"), this),
externalDec_(tr("File contains external declarations"), this)
{
	Q_ASSERT_X(file_, "FileGeneralEditor constructor",
		"Null File-pointer given for constructor");

	QLabel* logicNameLabel = new QLabel(tr("Logical name (default):"), this);
	logicNameLabel->setToolTip(tr("Logical name for the file or directory.\n"
		"For example VHDL library name for a vhdl-file"));

    QVBoxLayout* topLayout = new QVBoxLayout(this);

    QHBoxLayout* nameLayout = new QHBoxLayout();
	nameLayout->addWidget(logicNameLabel);
	nameLayout->addWidget(&logicalName_);

    topLayout->addLayout(nameLayout);
    topLayout->addWidget(&logicalDefault_);
    topLayout->addWidget(&includeFile_);
	topLayout->addWidget(&externalDec_);

	connect(&logicalName_, SIGNAL(textEdited(const QString&)),
        this, SLOT(onLogicalNameChanged()), Qt::UniqueConnection);
	connect(&logicalDefault_, SIGNAL(clicked(bool)), this, SLOT(onLogicalNameChanged()), Qt::UniqueConnection);
	connect(&includeFile_, SIGNAL(clicked(bool)), this, SLOT(onIncludeFileChanged()), Qt::UniqueConnection);
	connect(&externalDec_, SIGNAL(clicked(bool)), this, SLOT(onExternalDecChanged()), Qt::UniqueConnection);

	refresh();
}

FileGeneralEditor::~FileGeneralEditor() {

}

void FileGeneralEditor::refresh() {
	logicalName_.setText(file_->getLogicalName());
	logicalDefault_.setChecked(file_->getLogicalNameDefault());
	includeFile_.setChecked(file_->getIncludeFile());
	externalDec_.setChecked(file_->getExternalDeclarations());
}

void FileGeneralEditor::onLogicalNameChanged() {
	file_->setLogicalName(logicalName_.text());
	file_->setLogicalNameDefault(logicalDefault_.isChecked());
	emit contentChanged();
}

void FileGeneralEditor::onIncludeFileChanged() {
	file_->setIncludeFile(includeFile_.isChecked());
	emit contentChanged();
}

void FileGeneralEditor::onExternalDecChanged() {
	file_->setExternalDeclarations(externalDec_.isChecked());
	emit contentChanged();
}
