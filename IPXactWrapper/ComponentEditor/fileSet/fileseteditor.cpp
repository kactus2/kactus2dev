/* 
 *
 *  Created on: 3.2.2011
 *      Author: Antti Kamppi
 * 		filename: fileseteditor.cpp
 */

#include "fileseteditor.h"
#include <LibraryManager/libraryinterface.h>
#include <models/component.h>

#include <QStringList>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>

FileSetEditor::FileSetEditor(LibraryInterface* handler,
							 QSharedPointer<Component> component,
							 QSharedPointer<FileSet> fileSet, 
							 QWidget *parent ):
ItemEditor(component, parent),
baseLocation_(handler->getPath(*component->getVlnv())),
fileSet_(fileSet), 
nameEditor_(fileSet->getNameGroup(), this),
groups_(tr("Group identifiers"), this),
fileBuilders_(fileSet->getDefaultFileBuilders(), this),
files_(component, fileSet, handler, this),
dependencies_(tr("Dependent directories"), handler, component, this) {

	Q_ASSERT(fileSet_);

	// set the maximum heights to give more space for files editor
	fileBuilders_.setMaximumHeight(150);
	groups_.setMaximumHeight(100);
	dependencies_.setMaximumHeight(100);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);

	// connect the signals informing of data changes
	layout->addWidget(&nameEditor_);
	connect(&nameEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	layout->addWidget(&files_);
	connect(&files_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&files_, SIGNAL(fileAdded(int)),
		this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&files_, SIGNAL(fileRemoved(int)),
		this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);
	connect(&files_, SIGNAL(fileMoved(int, int)),
		this, SIGNAL(childMoved(int, int)), Qt::UniqueConnection);

	layout->addWidget(&fileBuilders_);
	connect(&fileBuilders_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	// set the groups and dependencies side by side
	QHBoxLayout* horizontalLayout = new QHBoxLayout();
	horizontalLayout->addWidget(&groups_);
	horizontalLayout->addWidget(&dependencies_);

	layout->addLayout(horizontalLayout);
	
	connect(&groups_, SIGNAL(contentChanged()),
		this, SLOT(onGroupsChange()), Qt::UniqueConnection);
	
	connect(&dependencies_, SIGNAL(contentChanged()),
		this, SLOT(onDependenciesChange()), Qt::UniqueConnection);

	refresh();
}

FileSetEditor::~FileSetEditor() {
}

bool FileSetEditor::isValid() const {
	if (!nameEditor_.isValid()) {
		return false;
	}
	else if (!fileBuilders_.isValid()) {
		return false;
	}
	else if (!files_.isValid()) {
		return false;
	}
	else {
		return true;
	}
}

void FileSetEditor::refresh() {
	// set the values for the nameGroupBox
	nameEditor_.refresh();

	// initialize groups 
	groups_.initialize(fileSet_->getGroups());

	files_.refresh();

	fileBuilders_.refresh();

	// initialize dependencies
	dependencies_.initialize(fileSet_->getDependencies());
}

void FileSetEditor::onGroupsChange() {
	fileSet_->setGroups(groups_.items());
	emit contentChanged();
}

void FileSetEditor::onDependenciesChange() {
	fileSet_->setDependencies(dependencies_.items());
	emit contentChanged();
}
