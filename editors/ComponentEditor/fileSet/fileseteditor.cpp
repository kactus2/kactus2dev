/* 
 *
 *  Created on: 3.2.2011
 *      Author: Antti Kamppi
 * 		filename: fileseteditor.cpp
 */

#include "fileseteditor.h"
#include <library/LibraryManager/libraryinterface.h>
#include <IPXACTmodels/component.h>

#include <QStringList>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>

//-----------------------------------------------------------------------------
// Function: FileSetEditor::FileSetEditor()
//-----------------------------------------------------------------------------
FileSetEditor::FileSetEditor(LibraryInterface* handler,
							 QSharedPointer<Component> component,
							 QSharedPointer<FileSet> fileSet, 
							 QWidget *parent ):
ItemEditor(component, handler, parent),
baseLocation_(handler->getPath(*component->getVlnv())),
fileSet_(fileSet), 
nameEditor_(fileSet->getNameGroup(), this),
groupsEditor_(tr("Group identifiers"), this),
fileBuilderEditor_(fileSet->getDefaultFileBuilders(), this),
files_(component, fileSet, handler, this),
dependencies_(tr("Dependent directories"), handler, component, this)
{
    nameEditor_.setTitle("File set name and description");

    setupLayout();

    connect(&nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&files_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&files_, SIGNAL(fileAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
    connect(&files_, SIGNAL(fileRemoved(int)),this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);
    connect(&files_, SIGNAL(fileMoved(int, int)), this, SIGNAL(childMoved(int, int)), Qt::UniqueConnection);

    connect(&fileBuilderEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	connect(&groupsEditor_, SIGNAL(contentChanged()),	this, SLOT(onGroupsChange()), Qt::UniqueConnection);
	
	connect(&dependencies_, SIGNAL(contentChanged()), this, SLOT(onDependenciesChange()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FileSetEditor::~FileSetEditor()
//-----------------------------------------------------------------------------
FileSetEditor::~FileSetEditor()
{
}

//-----------------------------------------------------------------------------
// Function: FileSetEditor::isValid()
//-----------------------------------------------------------------------------
bool FileSetEditor::isValid() const
{
	return nameEditor_.isValid() && fileBuilderEditor_.isValid() && files_.isValid();
}

//-----------------------------------------------------------------------------
// Function: FileSetEditor::refresh()
//-----------------------------------------------------------------------------
void FileSetEditor::refresh()
{
	// set the values for the nameGroupBox
	nameEditor_.refresh();

	// initialize groups 
	groupsEditor_.initialize(fileSet_->getGroups());

	files_.refresh();

	fileBuilderEditor_.refresh();

	// initialize dependencies
	dependencies_.initialize(fileSet_->getDependencies());
}

//-----------------------------------------------------------------------------
// Function: FileSetEditor::onGroupsChange()
//-----------------------------------------------------------------------------
void FileSetEditor::onGroupsChange()
{
	fileSet_->setGroups(groupsEditor_.items());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FileSetEditor::onDependenciesChange()
//-----------------------------------------------------------------------------
void FileSetEditor::onDependenciesChange()
{
	fileSet_->setDependencies(dependencies_.items());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FileSetEditor::showEvent()
//-----------------------------------------------------------------------------
void FileSetEditor::showEvent( QShowEvent* event )
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/fileset.html");
}

//-----------------------------------------------------------------------------
// Function: FileSetEditor::getFileSet()
//-----------------------------------------------------------------------------
FileSet* FileSetEditor::getFileSet()
{
    return fileSet_.data();
}

//-----------------------------------------------------------------------------
// Function: FileSetEditor::setupLayout()
//-----------------------------------------------------------------------------
void FileSetEditor::setupLayout()
{
    // set the maximum heights to give more space for files editor
    fileBuilderEditor_.setMaximumHeight(150);
    groupsEditor_.setMaximumHeight(nameEditor_.maximumHeight());
    dependencies_.setMaximumHeight(150);

    QHBoxLayout* nameAndGroupLayout = new QHBoxLayout();
    nameAndGroupLayout->addWidget(&nameEditor_);
    nameAndGroupLayout->addWidget(&groupsEditor_);

    QHBoxLayout* buildCommandsAndDependenciesLayout = new QHBoxLayout();
    buildCommandsAndDependenciesLayout->addWidget(&fileBuilderEditor_);
    buildCommandsAndDependenciesLayout->addWidget(&dependencies_);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->setContentsMargins(0, 0, 0, 0);

    topLayout->addLayout(nameAndGroupLayout);
    topLayout->addWidget(&files_);
    topLayout->addLayout(buildCommandsAndDependenciesLayout);
}
