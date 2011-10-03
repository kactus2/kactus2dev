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
fileSet_(0), nameBox_(this), 
groups_(tr("Group identifiers"), this, QStringList()),
dependencies_(tr("Dependent directories"), baseLocation_, this, QStringList()) {

	fileSet_ = static_cast<FileSet*>(dataPointer);
	Q_ASSERT_X(fileSet_, "FileSetEditor::setDataPointer",
		"static_cast failed to give valid FileSet-pointer");

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

	// set the values for the nameGroupBox
	nameBox_.setName(fileSet_->getName());
	nameBox_.setDisplayName(fileSet_->getDisplayName());
	nameBox_.setDescription(fileSet_->getDescription());

	// set the items for the groups list
	groups_.setItems(fileSet_->getGroups());

	// set the items for the dependencies list
	dependencies_.setItems(fileSet_->getDependencies());
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
	// if widget is in invalid state then don't apply the changes
	if (!isValid()) {
		return;
	}

	fileSet_->setName(nameBox_.getName());
	fileSet_->setDisplayName(nameBox_.getDisplayName());
	fileSet_->setDescription(nameBox_.getDescription());

	fileSet_->setGroups(groups_.items());

	fileSet_->setDependencies(dependencies_.items());
}
