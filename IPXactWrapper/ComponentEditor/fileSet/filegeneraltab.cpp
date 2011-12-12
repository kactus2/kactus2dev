/* 
 *
 *  Created on: 7.2.2011
 *      Author: Antti Kamppi
 * 		filename: filegeneraltab.cpp
 */

#include "filegeneraltab.h"

#include <QVBoxLayout>
#include <QStringList>

FileGeneralTab::FileGeneralTab(const QFileInfo& baseLocation,  
							   File* file, 
							   QWidget *parent ):
QWidget(parent), file_(file),
nameEditor_(this, baseLocation),
generalEditor_(this, file),
fileTypeEditor_(this, file, QStringList()) {

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&nameEditor_);
	layout->addWidget(&fileTypeEditor_);
	layout->addWidget(&generalEditor_);

	connect(&nameEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&nameEditor_, SIGNAL(nameChanged(const QString&)),
		this, SIGNAL(nameChanged(const QString&)), Qt::UniqueConnection);
	connect(&generalEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&fileTypeEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	setLayout(layout);
}

FileGeneralTab::~FileGeneralTab() {

}

bool FileGeneralTab::isValid() const {
	
	// at least one file type has to be defined and the file name must be 
	// specified.
	return (fileTypeEditor_.isValid() && !nameEditor_.getFileName().isEmpty());
}

void FileGeneralTab::apply() {
	file_->setName(nameEditor_.getFileName());
	//file_->setNameAttributes(nameEditor_.getAttributes());

	generalEditor_.apply();
	fileTypeEditor_.apply();
}

void FileGeneralTab::restore() {
	nameEditor_.setFileName(file_->getName());
	//nameEditor_.setAttributes(file_->getNameAttributes());

	generalEditor_.restore();
	fileTypeEditor_.restore();
}
