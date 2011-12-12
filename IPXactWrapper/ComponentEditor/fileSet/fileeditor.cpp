/* 
 *
 *  Created on: 5.2.2011
 *      Author: Antti Kamppi
 * 		filename: fileeditor.cpp
 */

#include "fileeditor.h"
#include "filegeneraltab.h"
#include "fileextratab.h"
#include "filebuildcommand.h"

#include <models/component.h>
#include <models/file.h>

#include <QVBoxLayout>

FileEditor::FileEditor( const QFileInfo& baseLocation, 
					   QSharedPointer<Component> component,
					   void* dataPointer,
					   QWidget *parent ):
ItemEditor(component, parent),
tabs_(this), 
generalTab_(NULL),
extraTab_(NULL),
buildCommand_(NULL),
file_(NULL) {

	file_ = static_cast<File*>(dataPointer);
	Q_ASSERT_X(file_, "FileEditor::setDataPointer",
		"static_cast failed to give valid File-pointer");

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(&tabs_);

	generalTab_ = new FileGeneralTab(baseLocation, file_, &tabs_);
	tabs_.addTab(generalTab_, tr("General settings"));

	buildCommand_ = new FileBuildCommand(&tabs_, file_, baseLocation);
	tabs_.addTab(buildCommand_, tr("Build command"));

	extraTab_ = new FileExtraTab(baseLocation, file_, &tabs_);
	tabs_.addTab(extraTab_, tr("External dependencies and defines"));

	// connect the signals informing that widgets have changed their status
	connect(generalTab_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(generalTab_, SIGNAL(nameChanged(const QString&)),
		this, SIGNAL(nameChanged(const QString&)), Qt::UniqueConnection);
	connect(extraTab_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(buildCommand_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	// fetch the data from the model and disable the discard button
	generalTab_->restore();
	extraTab_->restore();
	buildCommand_->restore();
}

FileEditor::~FileEditor() {
}

bool FileEditor::isValid() const {

	// general tab contains the only mandatory fields.
	return generalTab_->isValid();
}

void FileEditor::removeModel() {
	file_->remove();
	file_ = 0;
}

void FileEditor::makeChanges() {
	if (file_) {
		generalTab_->apply();
		extraTab_->apply();
		buildCommand_->apply();
	}
}
