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

	connect(&logicalName_, SIGNAL(textChanged(const QString&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&logicalDefault_, SIGNAL(clicked(bool)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&includeFile_, SIGNAL(clicked(bool)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&externalDec_, SIGNAL(clicked(bool)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&description_, SIGNAL(textChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

FileGeneralEditor::~FileGeneralEditor() {

}

void FileGeneralEditor::restore() {

	logicalName_.setText(file_->getLogicalName());
	logicalDefault_.setChecked(file_->getLogicalNameDefault());

	includeFile_.setChecked(file_->getIncludeFile());
	externalDec_.setChecked(file_->getExternalDeclarations());

	description_.setPlainText(file_->getDescription());
}

void FileGeneralEditor::apply() {
	file_->setLogicalName(logicalName_.text());
	file_->setLogicalNameDefault(logicalDefault_.isChecked());

	file_->setIncludeFile(includeFile_.isChecked());
	file_->setExternalDeclarations(externalDec_.isChecked());

	file_->setDescription(description_.toPlainText());
}
