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

FileEditor::FileEditor( const QString& baseLocation,
					   QSharedPointer<Component> component, 
					   QSharedPointer<File> file, 
					   QWidget *parent ):
ItemEditor(component, parent),
tabs_(this), 
generalTab_(NULL),
extraTab_(NULL),
file_(file.data()),
baseLocation_(baseLocation) {

	Q_ASSERT(component);
	Q_ASSERT(file);

	initialize();
}

FileEditor::~FileEditor() {
}

void FileEditor::initialize() {
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(&tabs_);

	generalTab_ = new FileGeneralTab(baseLocation_, file_, &tabs_);
	tabs_.addTab(generalTab_, tr("General settings"));

	extraTab_ = new FileExtraTab(baseLocation_, file_, &tabs_);
	tabs_.addTab(extraTab_, tr("External dependencies and defines"));

	// connect the signals informing that widgets have changed their status
	connect(generalTab_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(generalTab_, SIGNAL(nameChanged(const QString&)),
		this, SIGNAL(nameChanged(const QString&)), Qt::UniqueConnection);
	connect(extraTab_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	refresh();
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
	}
}

void FileEditor::refresh() {
	// fetch the data from the model
	generalTab_->restore();
	extraTab_->restore();
}
