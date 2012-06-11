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
logicalDefault_(tr("Only used as default"), this),
includeFile_(tr("File is include file"), this),
externalDec_(tr("File contains external declarations"), this),
description_(this) {

	Q_ASSERT_X(file_, "FileGeneralEditor constructor",
		"Null File-pointer given for constructor");

	QLabel* logicNameLabel = new QLabel(tr("Logical name:"), this);
	logicNameLabel->setToolTip(tr("Logical name for the file or directory.\n"
		"For example VHDL library name for a vhdl-file"));

	QLabel* descLabel = new QLabel(tr("Description of the file:"), this);
	descLabel->setToolTip(tr("Description of the file for the user"));

	QGridLayout* layout = new QGridLayout(this);
	layout->addWidget(logicNameLabel, 0, 0, 1, 1);
	layout->addWidget(&logicalName_, 0, 1, 1, 1);
	layout->addWidget(&logicalDefault_, 0, 2, 1, 1);
	layout->addWidget(&includeFile_, 1, 0, 1, 1);
	layout->addWidget(&externalDec_, 1, 1, 1, 1);
	layout->addWidget(descLabel, 2, 0, 1, 2);
	layout->addWidget(&description_, 3, 0, 2, 3);

	connect(&logicalName_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onLogicalNameChanged()), Qt::UniqueConnection);
	connect(&logicalDefault_, SIGNAL(clicked(bool)),
		this, SLOT(onLogicalNameChanged()), Qt::UniqueConnection);
	connect(&includeFile_, SIGNAL(clicked(bool)),
		this, SLOT(onIncludeFileChanged()), Qt::UniqueConnection);
	connect(&externalDec_, SIGNAL(clicked(bool)),
		this, SLOT(onExternalDecChanged()), Qt::UniqueConnection);
	connect(&description_, SIGNAL(textChanged()),
		this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);

	refresh();
}

FileGeneralEditor::~FileGeneralEditor() {

}

void FileGeneralEditor::refresh() {
	logicalName_.setText(file_->getLogicalName());
	logicalDefault_.setChecked(file_->getLogicalNameDefault());
	includeFile_.setChecked(file_->getIncludeFile());
	externalDec_.setChecked(file_->getExternalDeclarations());
	description_.setPlainText(file_->getDescription());
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

void FileGeneralEditor::onDescriptionChanged() {
	file_->setDescription(description_.toPlainText());
	emit contentChanged();
}
