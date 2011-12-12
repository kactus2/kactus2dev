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

FileNameEditor::FileNameEditor(QWidget *parent,
							   const QFileInfo& baseLocation,
							   const QString fileName,
							   const QMap<QString, QString>& attributes): 
QGroupBox(tr("File name and path"), parent), 
nameEdit_(this, baseLocation, fileName) 
//attributes_(this)
{

	// the layout for the name label and name line edit
	QHBoxLayout* nameLayout = new QHBoxLayout();

	// create the label
	QLabel* nameLabel = new QLabel(tr("Name:"), this);
	nameLabel->setToolTip(tr("The name field contains an absolute or relative\n"
		"path to a file name or directory"));
	nameLayout->addWidget(nameLabel, 0);
	nameLayout->addWidget(&nameEdit_, 1);

	// the top layout for the whole widget
	QVBoxLayout* topLayout = new QVBoxLayout(this);

	topLayout->addLayout(nameLayout);
	//topLayout->addWidget(&attributes_);

	// set the attributes for the attribute widget
	//attributes_.setAttributes(attributes);

	nameEdit_.setProperty("mandatoryField", true);

	// connect the signals informing of content changes.
	connect(&nameEdit_, SIGNAL(textChanged(const QString&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&nameEdit_, SIGNAL(textChanged(const QString&)),
		this, SIGNAL(nameChanged(const QString&)), Qt::UniqueConnection);
// 	connect(&attributes_, SIGNAL(contentChanged()),
// 		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

FileNameEditor::~FileNameEditor() {
}

QString FileNameEditor::getFileName() const {
	return nameEdit_.text();
}

// const QMap<QString, QString> FileNameEditor::getAttributes() const {
// 	return attributes_.getAttributes();
// }

void FileNameEditor::setFileName( const QString& fileName ) {
	nameEdit_.setText(fileName);
}

// void FileNameEditor::setAttributes( const QMap<QString, QString>& attributes ) {
// 	attributes_.setAttributes(attributes);
// }

bool FileNameEditor::isValid() const {
	// both editor widgets must be in valid state
	return (nameEdit_.isValid() /*&& attributes_.isValid()*/);
}
