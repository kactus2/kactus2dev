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

FileGeneralTab::FileGeneralTab(LibraryInterface* handler,
							   QSharedPointer<Component> component,
							   QSharedPointer<File> file, 
							   QWidget *parent ):
QWidget(parent), 
file_(file),
nameEditor_(this, handler, component, file),
generalEditor_(this, file),
fileTypeEditor_(this, file),
buildCommand_(this, handler, component, file) {

	fileTypeEditor_.initialize();

	// the group box that contains the build command so it is clearly separated
	QGroupBox* buildBox = new QGroupBox(tr("Build command"), this);
	QVBoxLayout* buildLayout = new QVBoxLayout(buildBox);
	buildLayout->addWidget(&buildCommand_);
	buildLayout->setContentsMargins(0, 0, 0, 0);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&nameEditor_);
	layout->addWidget(&fileTypeEditor_);
	layout->addWidget(&generalEditor_);
	layout->addWidget(buildBox);

	connect(&nameEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&generalEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&fileTypeEditor_, SIGNAL(contentChanged()),
		this, SLOT(onFileTypesChanged()), Qt::UniqueConnection);
	connect(&buildCommand_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	setLayout(layout);
}

FileGeneralTab::~FileGeneralTab() {

}

bool FileGeneralTab::isValid() const {
	
	// at least one file type has to be defined and the file name must be 
	// specified.
	return (fileTypeEditor_.isValid() && nameEditor_.isValid());
}

void FileGeneralTab::refresh() {
	nameEditor_.refresh();

	generalEditor_.refresh();
	fileTypeEditor_.restore();
	buildCommand_.refresh();
}

void FileGeneralTab::onFileTypesChanged() {
	// get the file types from the editor to file model
	QStringList fileTypes = fileTypeEditor_.items();
	file_->setAllFileTypes(fileTypes);
	emit contentChanged();
}
