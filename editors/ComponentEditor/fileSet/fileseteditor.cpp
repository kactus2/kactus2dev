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

#include <QGridLayout>
#include <QScrollArea>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: FileSetEditor::FileSetEditor()
//-----------------------------------------------------------------------------
FileSetEditor::FileSetEditor(LibraryInterface* handler, QSharedPointer<Component> component,
                             QSharedPointer<FileSet> fileSet, QSharedPointer<ParameterFinder> parameterFinder,
                             QSharedPointer<ExpressionParser> expressionParser,
                             QSharedPointer<ExpressionFormatter> expressionFormatter, QWidget *parent):
ItemEditor(component, handler, parent),
baseLocation_(handler->getPath(component->getVlnv())),
fileSet_(fileSet),
nameEditor_(fileSet, this),
groupsEditor_(tr("Group identifiers"), this),
fileBuilderEditor_(fileSet->getDefaultFileBuilders(), parameterFinder, expressionParser, expressionFormatter, this),
files_(component, fileSet, handler, this),
dependencies_(tr("Dependent directories"), handler, component, this)
{
    nameEditor_.setTitle("File set name and description");

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
	// set the values for the nameGroupBox
	nameEditor_.refresh();

	// initialize groups 
	groupsEditor_.initialize(*fileSet_->getGroups());

	files_.refresh();

	fileBuilderEditor_.refresh();

	// initialize dependencies
	dependencies_.initialize(*fileSet_->getDependencies());
}

//-----------------------------------------------------------------------------
// Function: FileSetEditor::onGroupsChange()
//-----------------------------------------------------------------------------
void FileSetEditor::onGroupsChange()
{
    fileSet_->getGroups()->clear();
    fileSet_->getGroups()->append(groupsEditor_.items());

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FileSetEditor::onDependenciesChange()
//-----------------------------------------------------------------------------
void FileSetEditor::onDependenciesChange()
{
    fileSet_->getDependencies()->clear();
    fileSet_->getDependencies()->append(dependencies_.items());
    
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
    topLayout->addWidget(&fileBuilderEditor_, 2, 0, 1, 1);
    topLayout->addWidget(&dependencies_, 2, 1, 1, 1);
    topLayout->setRowStretch(0, 1);
    topLayout->setRowStretch(1, 10);
    topLayout->setRowStretch(2, 2);

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
}
