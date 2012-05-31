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
nameEditor_(fileSet_->getNameGroup(), this),
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
nameEditor_(fileSet->getNameGroup(), this),
groups_(tr("Group identifiers"), this),
dependencies_(tr("Dependent directories"), baseLocation_, this) {

	initialize();
}

void FileSetEditor::initialize() {
	Q_ASSERT(fileSet_);

	QVBoxLayout* layout = new QVBoxLayout(this);

	// connect the signals informing of data changes
	layout->addWidget(&nameEditor_);
	connect(&nameEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	layout->addWidget(&groups_);
	connect(&groups_, SIGNAL(contentChanged()),
		this, SLOT(onGroupsChange()), Qt::UniqueConnection);

	layout->addWidget(&dependencies_);
	connect(&dependencies_, SIGNAL(contentChanged()),
		this, SLOT(onDependenciesChange()), Qt::UniqueConnection);

	refresh();
}

FileSetEditor::~FileSetEditor() {
}

bool FileSetEditor::isValid() const {
	return nameEditor_.isValid();
}

void FileSetEditor::removeModel() {
	QSharedPointer<Component> component = ItemEditor::component();
	component->removeFileSet(fileSet_->getName());
	fileSet_ = 0;
}

void FileSetEditor::makeChanges() {
	// this is empty because all changes are applied to model immediately 
}

void FileSetEditor::refresh() {
	// set the values for the nameGroupBox
	nameEditor_.refresh();

	// initialize groups 
	groups_.initialize(fileSet_->getGroups());

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
