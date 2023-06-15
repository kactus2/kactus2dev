//-----------------------------------------------------------------------------
// File: fileseteditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 03.02.2011
//
// Description:
// FileSetEditor is an editor used to edit the details of a FileSet.
//-----------------------------------------------------------------------------

#include "fileseteditor.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <KactusAPI/include/FileSetInterface.h>

#include <QGridLayout>
#include <QScrollArea>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: FileSetEditor::FileSetEditor()
//-----------------------------------------------------------------------------
FileSetEditor::FileSetEditor(LibraryInterface* handler, QSharedPointer<Component> component,
    QSharedPointer<FileSet> fileSet, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter,
    FileSetInterface* fileSetInterface, QWidget *parent):
ItemEditor(component, handler, parent),
baseLocation_(handler->getPath(component->getVlnv())),
nameEditor_(fileSet, component->getRevision(), this),
groupsEditor_(tr("Group identifiers"), this),
fileBuilderEditor_(fileSetInterface->getFileBuilderInterface(), parameterFinder, expressionParser,
    expressionFormatter, fileSet->getDefaultFileBuilders(), this),
files_(fileSet->getFiles(), fileSetInterface->getFileInterface(), component, handler, this),
dependencies_(tr("Dependent directories"), handler->getDirectoryPath(component->getVlnv()), this),
fileSetInterface_(fileSetInterface),
availableFileSets_(component->getFileSets())
{
    nameEditor_.setTitle("File set name and description");

    groupsEditor_.setFlat(true);
    fileBuilderEditor_.setFlat(true);
    dependencies_.setFlat(true);

    setupLayout();

    connectSignals();
}

//-----------------------------------------------------------------------------
// Function: FileSetEditor::~FileSetEditor()
//-----------------------------------------------------------------------------
FileSetEditor::~FileSetEditor()
{

}

//-----------------------------------------------------------------------------
// Function: FileSetEditor::refresh()
//-----------------------------------------------------------------------------
void FileSetEditor::refresh()
{
    fileSetInterface_->setFileSets(availableFileSets_);

	// set the values for the nameGroupBox
	nameEditor_.refresh();

    QStringList fileSetGroups;
    for (auto singleGroup : fileSetInterface_->getGroups(nameEditor_.name().toStdString()))
    {
        fileSetGroups.append(QString::fromStdString(singleGroup));
    }

	// initialize groups 
    groupsEditor_.initialize(fileSetGroups);

	files_.refresh();

	fileBuilderEditor_.refresh();

	// initialize dependencies
    QStringList fileSetDependencies;
    for (auto singleDependency : fileSetInterface_->getDependencies(nameEditor_.name().toStdString()))
    {
        fileSetDependencies.append(QString::fromStdString(singleDependency));
    }

    dependencies_.initialize(fileSetDependencies);
}

//-----------------------------------------------------------------------------
// Function: FileSetEditor::onGroupsChange()
//-----------------------------------------------------------------------------
void FileSetEditor::onGroupsChange()
{
    std::vector<std::string> newGroups;
    for (auto group : groupsEditor_.items())
    {
        newGroups.push_back(group.toStdString());
    }

    fileSetInterface_->setGroups(nameEditor_.name().toStdString(), newGroups);

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FileSetEditor::onDependenciesChange()
//-----------------------------------------------------------------------------
void FileSetEditor::onDependenciesChange()
{
    std::vector<std::string> newDependencies;
    for (auto dependency : dependencies_.items())
    {
        newDependencies.push_back(dependency.toStdString());
    }

    fileSetInterface_->setDependencies(nameEditor_.name().toStdString(), newDependencies);

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FileSetEditor::showEvent()
//-----------------------------------------------------------------------------
void FileSetEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/fileset.html");
}

//-----------------------------------------------------------------------------
// Function: FileSetEditor::setupLayout()
//-----------------------------------------------------------------------------
void FileSetEditor::setupLayout()
{
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout* scrollLayout = new QVBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* topWidget = new QWidget(scrollArea);

    QGridLayout* topLayout = new QGridLayout(topWidget);
    topLayout->setContentsMargins(0, 0, 0, 0);

    topLayout->addWidget(&nameEditor_, 0, 0, 1, 1);
    topLayout->addWidget(&groupsEditor_, 0, 1, 1, 1);
    topLayout->addWidget(&files_, 1, 0, 1, 2);    
    topLayout->addWidget(&fileBuilderEditor_, 2, 0, 1, 2);
    topLayout->addWidget(&dependencies_, 3, 0, 1, 1);
    topLayout->setRowStretch(0, 1);
    topLayout->setRowStretch(1, 4);
    topLayout->setRowStretch(2, 2);
    topLayout->setRowStretch(3, 1);

    scrollArea->setWidget(topWidget);
}

//-----------------------------------------------------------------------------
// Function: fileseteditor::connectSignals()
//-----------------------------------------------------------------------------
void FileSetEditor::connectSignals()
{
    connect(&nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&files_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&files_, SIGNAL(fileAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
    connect(&files_, SIGNAL(fileRemoved(int)),this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);
    connect(&files_, SIGNAL(fileMoved(int, int)), this, SIGNAL(childMoved(int, int)), Qt::UniqueConnection);

    connect(&fileBuilderEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&groupsEditor_, SIGNAL(contentChanged()),	this, SLOT(onGroupsChange()), Qt::UniqueConnection);

    connect(&dependencies_, SIGNAL(contentChanged()), this, SLOT(onDependenciesChange()), Qt::UniqueConnection);

    connect(&fileBuilderEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&fileBuilderEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(&files_, SIGNAL(fileRenamed(std::string const&, std::string const&)),
        this, SIGNAL(fileRenamed(std::string const&, std::string const&)), Qt::UniqueConnection);
}
