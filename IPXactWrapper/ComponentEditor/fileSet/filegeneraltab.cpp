/* 
 *
 *  Created on: 7.2.2011
 *      Author: Antti Kamppi
 * 		filename: filegeneraltab.cpp
 */

#include "filegeneraltab.h"

#include <QGridLayout>
#include <QStringList>

FileGeneralTab::FileGeneralTab(const QFileInfo& baseLocation,  
							   File* file, 
							   QWidget *parent ):
QWidget(parent), file_(file),
nameEditor_(this, baseLocation),
attributes_(this), 
generalEditor_(this, file),
fileTypeEditor_(this, file, QStringList()) {

	QGridLayout* layout = new QGridLayout(this);
	layout->addWidget(&nameEditor_, 0, 0, 1, 1);
	layout->addWidget(&attributes_, 0, 1, 1, 1);
	layout->addWidget(&generalEditor_, 1, 0, 1, 2);
	layout->addWidget(&fileTypeEditor_, 2, 0, 1, 2);

	connect(&nameEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&nameEditor_, SIGNAL(nameChanged(const QString&)),
		this, SIGNAL(nameChanged(const QString&)), Qt::UniqueConnection);
	connect(&attributes_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
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
	file_->setNameAttributes(nameEditor_.getAttributes());

	file_->setAttributes(attributes_.getAttributes());

	generalEditor_.apply();
	fileTypeEditor_.apply();
}

void FileGeneralTab::restore() {
	nameEditor_.setFileName(file_->getName());
	nameEditor_.setAttributes(file_->getNameAttributes());

	attributes_.setAttributes(file_->getAttributes());

	generalEditor_.restore();
	fileTypeEditor_.restore();
}
