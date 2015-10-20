//-----------------------------------------------------------------------------
// File: fileseteditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 03.02.2011
//
// Description:
// FileSetEditor is an editor used to edit the details of a FileSet.
//-----------------------------------------------------------------------------

#include "fileseteditor.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <QStringList>
#include <QVBoxLayout>
#include <QHBoxLayout>

//-----------------------------------------------------------------------------
// Function: FileSetEditor::FileSetEditor()
//-----------------------------------------------------------------------------
FileSetEditor::FileSetEditor(LibraryInterface* handler, QSharedPointer<Component> component,
                             QSharedPointer<FileSet> fileSet, QWidget *parent ):
ItemEditor(component, handler, parent),
baseLocation_(handler->getPath(component->getVlnv())),
fileSet_(fileSet), 
nameEditor_(fileSet, this),
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
	groupsEditor_.initialize(*fileSet_->getGroups().data());

	files_.refresh();

	fileBuilderEditor_.refresh();

	// initialize dependencies
	dependencies_.initialize(*fileSet_->getDependencies().data());
}

//-----------------------------------------------------------------------------
// Function: FileSetEditor::onGroupsChange()
//-----------------------------------------------------------------------------
void FileSetEditor::onGroupsChange()
{
    QStringList groupList = groupsEditor_.items();
    fileSet_->getGroups()->clear();
    foreach (QString group, groupList)
    {
        fileSet_->getGroups()->append(group);
    }

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FileSetEditor::onDependenciesChange()
//-----------------------------------------------------------------------------
void FileSetEditor::onDependenciesChange()
{
    QStringList dependencies = dependencies_.items();
    fileSet_->getDependencies()->clear();
    foreach (QString dependency, dependencies)
    {
        fileSet_->getDependencies()->append(dependency);
    }
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
QSharedPointer<FileSet> FileSetEditor::getFileSet()
{
    return fileSet_;
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
