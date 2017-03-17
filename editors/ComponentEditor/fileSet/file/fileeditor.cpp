//-----------------------------------------------------------------------------
// File: fileeditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 5.2.2011
//
// Description:
// Editor for the details of a file.
//-----------------------------------------------------------------------------

#include "fileeditor.h"

#include <library/LibraryInterface.h>

#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/Component.h>

#include <QScrollArea>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: fileeditor::FileEditor()
//-----------------------------------------------------------------------------
FileEditor::FileEditor(LibraryInterface* handler, QSharedPointer<Component> component, QSharedPointer<File> file,
            QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionParser> expressionParser,
            QWidget *parent):
ItemEditor(component, handler, parent),
    file_(file),
    nameEditor_(file_, this),
    fileTypeEditor_(this, file),
    generalEditor_(this, file),
    exportedNamesEditor_(tr("Exported names"), this),
    buildCommand_(file, handler->getDirectoryPath(component->getVlnv()), parameterFinder, expressionParser, this),
    imageTypesEditor_(tr("Image types"), this),
    dependenciesEditor_(tr("Dependent directories"), handler->getDirectoryPath(component->getVlnv()), this),
    editButton_(new QPushButton(QIcon(":/icons/common/graphics/settings-code_editor.png"), tr("Edit file"), this)),
    runButton_(new QPushButton(QIcon(":/icons/common/graphics/plugin-generator.png"), tr("Run file"), this)),
    openFolderButton_(new QPushButton(QIcon(":/icons/common/graphics/folder-horizontal-open.png"), 
        tr("Open containing folder"), this))
{
    setupLayout();
    
    fileTypeEditor_.initialize();
    exportedNamesEditor_.initialize();
    imageTypesEditor_.initialize();
    dependenciesEditor_.initialize();

    connect(editButton_, SIGNAL(clicked(bool)), this, SIGNAL(editFile()), Qt::UniqueConnection);
    connect(runButton_, SIGNAL(clicked(bool)), this, SIGNAL(runFile()), Qt::UniqueConnection);
    connect(openFolderButton_, SIGNAL(clicked(bool)), this, SIGNAL(openContainingFolder()), Qt::UniqueConnection);

    connect(&generalEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&fileTypeEditor_, SIGNAL(contentChanged()), this, SLOT(onFileTypesChanged()), Qt::UniqueConnection);
    connect(&buildCommand_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&dependenciesEditor_, SIGNAL(contentChanged()),
        this, SLOT(onDependenciesChanged()), Qt::UniqueConnection);
    connect(&exportedNamesEditor_, SIGNAL(contentChanged()),
        this, SLOT(onExportedNamesChanged()), Qt::UniqueConnection);
    connect(&imageTypesEditor_, SIGNAL(contentChanged()), this, SLOT(onImageTypesChanged()), Qt::UniqueConnection);

    connect(&buildCommand_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&buildCommand_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: fileeditor::~FileEditor()
//-----------------------------------------------------------------------------
FileEditor::~FileEditor()
{

}

//-----------------------------------------------------------------------------
// Function: fileeditor::refresh()
//-----------------------------------------------------------------------------
void FileEditor::refresh()
{
    nameEditor_.refresh();
    generalEditor_.refresh();
    fileTypeEditor_.restore();
    buildCommand_.refresh();

    dependenciesEditor_.setItems(*file_->getDependencies());
    exportedNamesEditor_.setItems(*file_->getExportedNames());
    imageTypesEditor_.setItems(*file_->getImageTypes());
}

//-----------------------------------------------------------------------------
// Function: fileeditor::showEvent()
//-----------------------------------------------------------------------------
void FileEditor::showEvent(QShowEvent* event)
{
    ItemEditor::showEvent(event);
    emit helpUrlRequested("componenteditor/filegeneral.html");
}

//-----------------------------------------------------------------------------
// Function: fileeditor::onFileTypesChanged()
//-----------------------------------------------------------------------------
void FileEditor::onFileTypesChanged()
{
    file_->clearFileTypes();
    file_->getFileTypes()->append(fileTypeEditor_.items());

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: fileeditor::onDependenciesChanged()
//-----------------------------------------------------------------------------
void FileEditor::onDependenciesChanged()
{
    file_->getDependencies()->clear();
    file_->getDependencies()->append(dependenciesEditor_.items());

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: fileeditor::onDependenciesChanged()
//-----------------------------------------------------------------------------
void FileEditor::onExportedNamesChanged()
{
    file_->getExportedNames()->clear();
    file_->getExportedNames()->append(exportedNamesEditor_.items());

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: fileeditor::onDependenciesChanged()
//-----------------------------------------------------------------------------
void FileEditor::onImageTypesChanged()
{
    file_->getImageTypes()->clear();
    file_->getImageTypes()->append(imageTypesEditor_.items());

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: fileeditor::setupLayout()
//-----------------------------------------------------------------------------
void FileEditor::setupLayout()
{
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QVBoxLayout* scrollLayout = new QVBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(editButton_);
    buttonLayout->addWidget(runButton_);
    buttonLayout->addWidget(openFolderButton_);
    buttonLayout->addStretch(1);

    QWidget* topWidget = new QWidget(scrollArea);
    topWidget->setContentsMargins(0, 0, 0, 0);

    QGridLayout* topLayout = new QGridLayout(topWidget);
    topLayout->setContentsMargins(0, 0, 0, 0);

    topLayout->addWidget(&nameEditor_, 0, 0, 1, 1);
    topLayout->addWidget(&fileTypeEditor_, 0, 1, 1, 1);
    topLayout->addWidget(&generalEditor_, 1, 0, 1, 1);
    topLayout->addWidget(&exportedNamesEditor_, 1, 1, 1, 1);
    topLayout->addWidget(&buildCommand_, 2, 0, 1, 1);
    topLayout->addWidget(&imageTypesEditor_, 2, 1, 1, 1);
    topLayout->addWidget(&dependenciesEditor_, 3, 0, 1, 1);
    topLayout->addLayout(buttonLayout, 4, 0, 1, 2);

    topLayout->setRowStretch(0, 1);
    topLayout->setRowStretch(1, 1);
    topLayout->setRowStretch(2, 1);
    topLayout->setRowStretch(3, 2);
    topLayout->setRowStretch(4, 1);
    topLayout->setRowStretch(5, 6);    

    scrollArea->setWidget(topWidget);
}
