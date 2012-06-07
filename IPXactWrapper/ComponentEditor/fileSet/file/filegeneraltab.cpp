/* 
 *
 *  Created on: 7.2.2011
 *      Author: Antti Kamppi
 * 		filename: filegeneraltab.cpp
 */

#include "filegeneraltab.h"

#include <QVBoxLayout>
#include <QStringList>
#include <QGroupBox>

FileGeneralTab::FileGeneralTab(const QFileInfo& baseLocation,  
							   QSharedPointer<File> file, 
							   QWidget *parent ):
QWidget(parent), 
file_(file),
nameEditor_(this, baseLocation),
generalEditor_(this, file.data()),
fileTypeEditor_(this, file.data()),
buildCommand_(this, file.data(), baseLocation) {

	fileTypeEditor_.initialize();

	// the group box that contains the build command so it is clearly separated
	QGroupBox* buildBox = new QGroupBox(tr("Build command"), this);
	QVBoxLayout* buildLayout = new QVBoxLayout(buildBox);
	buildLayout->addWidget(&buildCommand_);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&nameEditor_);
	layout->addWidget(&fileTypeEditor_);
	layout->addWidget(&generalEditor_);
	layout->addWidget(buildBox);

	connect(&nameEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&nameEditor_, SIGNAL(nameChanged(const QString&)),
		this, SIGNAL(nameChanged(const QString&)), Qt::UniqueConnection);
	connect(&generalEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&fileTypeEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&buildCommand_, SIGNAL(contentChanged()),
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

	generalEditor_.apply();
	fileTypeEditor_.apply();
	buildCommand_.apply();
}

void FileGeneralTab::restore() {
	nameEditor_.setFileName(file_->getName());

	generalEditor_.restore();
	fileTypeEditor_.restore();
	buildCommand_.restore();
}
