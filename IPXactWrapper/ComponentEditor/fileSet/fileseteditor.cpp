/* 
 *
 *  Created on: 3.2.2011
 *      Author: Antti Kamppi
 * 		filename: fileseteditor.cpp
 */

#include "fileseteditor.h"

#include <models/component.h>

#include <QStringList>
#include <QVBoxLayout>

#include <QDebug>

FileSetEditor::FileSetEditor(const QFileInfo& baseLocation,
							 QSharedPointer<Component> component, 
							 void* dataPointer,
							 QWidget *parent):
ItemEditor(component, parent),
baseLocation_(baseLocation),
fileSet_(static_cast<FileSet*>(dataPointer)), 
nameBox_(this), 
groups_(tr("Group identifiers"), this),
dependencies_(tr("Dependent directories"), baseLocation_, this) {

	initialize();
}

FileSetEditor::FileSetEditor( const QString& baseLocation,
							 QSharedPointer<Component> component,
							 FileSet* fileSet, 
							 QWidget *parent ):
ItemEditor(component, parent),
baseLocation_(baseLocation),
fileSet_(fileSet), 
nameBox_(this), 
groups_(tr("Group identifiers"), this),
dependencies_(tr("Dependent directories"), baseLocation_, this) {

	initialize();
}

void FileSetEditor::initialize() {
	Q_ASSERT(fileSet_);

	QVBoxLayout* layout = new QVBoxLayout(this);

	// connect the signals informing of data changes
	layout->addWidget(&nameBox_);
	connect(&nameBox_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&nameBox_, SIGNAL(nameChanged(const QString&)),
		this, SIGNAL(nameChanged(const QString&)), Qt::UniqueConnection);

	layout->addWidget(&groups_);
	connect(&groups_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	layout->addWidget(&dependencies_);
	connect(&dependencies_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	refresh();
}

FileSetEditor::~FileSetEditor() {
}

bool FileSetEditor::isValid() const {
	return nameBox_.isValid();
}

void FileSetEditor::removeModel() {
	QSharedPointer<Component> component = ItemEditor::component();
	component->removeFileSet(fileSet_->getName());
	fileSet_ = 0;
}

void FileSetEditor::makeChanges() {
	fileSet_->setName(nameBox_.getName());
	fileSet_->setDisplayName(nameBox_.getDisplayName());
	fileSet_->setDescription(nameBox_.getDescription());

	fileSet_->setGroups(groups_.items());

	fileSet_->setDependencies(dependencies_.items());
}

void FileSetEditor::refresh() {
	// set the values for the nameGroupBox
	nameBox_.setName(fileSet_->getName());
	nameBox_.setDisplayName(fileSet_->getDisplayName());
	nameBox_.setDescription(fileSet_->getDescription());
	
	// initialize groups 
	groups_.initialize(fileSet_->getGroups());

	// initialize dependencies
	dependencies_.initialize(fileSet_->getDependencies());
}
