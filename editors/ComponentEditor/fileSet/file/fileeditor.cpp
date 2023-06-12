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

#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/Component.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <KactusAPI/include/FileInterface.h>

#include <QScrollArea>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: fileeditor::FileEditor()
//-----------------------------------------------------------------------------
FileEditor::FileEditor(LibraryInterface* handler, QSharedPointer<Component> component, std::string const& fileName,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionParser> expressionParser,
    FileInterface* fileInterface, QSharedPointer<QList<QSharedPointer<File>>> files, QWidget *parent):
ItemEditor(component, handler, parent),
fileName_(fileName),
nameEditor_(fileName, fileInterface, this),
fileTypeEditor_(component->getRevision(), this, fileName, fileInterface),
generalEditor_(this, fileName, fileInterface),
exportedNamesEditor_(tr("Exported names"), this),
buildCommand_(fileName, fileInterface, handler->getDirectoryPath(component->getVlnv()), parameterFinder,
    expressionParser, this),
imageTypesEditor_(tr("Image types"), this),
dependenciesEditor_(tr("Dependent directories"), handler->getDirectoryPath(component->getVlnv()), this),
editButton_(new QPushButton(QIcon(":/icons/common/graphics/edit.png"), tr("Edit file"), this)),
runButton_(new QPushButton(QIcon(":/icons/common/graphics/script-run-file.png"), tr("Run file"), this)),
openFolderButton_(new QPushButton(QIcon(":/icons/common/graphics/opened-folder.png"), tr("Open file location"), this)),
fileInterface_(fileInterface),
availableFiles_(files)
{
    fileInterface_->setFiles(availableFiles_);

    editButton_->setFlat(true);
    runButton_->setFlat(true);
    openFolderButton_->setFlat(true);

    exportedNamesEditor_.setFlat(true);
    imageTypesEditor_.setFlat(true);
    dependenciesEditor_.setFlat(true);

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
// Function: fileeditor::refresh()
//-----------------------------------------------------------------------------
void FileEditor::refresh()
{
    fileInterface_->setFiles(availableFiles_);

    nameEditor_.refresh();
    generalEditor_.refresh();
    fileTypeEditor_.restore();
    buildCommand_.refresh();

    std::vector<std::string> fileDependenciesSTD = fileInterface_->getDependencies(fileName_);
    QStringList fileDependencies;
    for (auto dependency : fileDependenciesSTD)
    {
        fileDependencies.append(QString::fromStdString(dependency));
    }
    dependenciesEditor_.setItems(fileDependencies);

    std::vector<std::string> fileExportedNamesSTD = fileInterface_->getExportedNames(fileName_);
    QStringList fileExportedNames;
    for (auto name : fileExportedNamesSTD)
    {
        fileExportedNames.append(QString::fromStdString(name));
    }
    exportedNamesEditor_.setItems(fileExportedNames);

    std::vector<std::string> fileImageTypesSTD = fileInterface_->getImageTypes(fileName_);
    QStringList fileImageTypes;
    for (auto imageType : fileImageTypesSTD)
    {
        fileImageTypes.append(QString::fromStdString(imageType));
    }
    imageTypesEditor_.setItems(fileImageTypes);
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
    fileInterface_->clearFileTypes(fileName_);

    std::vector<std::string> fileTypesInSTD;
    for (auto fileType : fileTypeEditor_.items())
    {
        fileTypesInSTD.push_back(fileType.toStdString());
    }

    fileInterface_->addMultipleFileTypes(fileName_, fileTypesInSTD);

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: fileeditor::onDependenciesChanged()
//-----------------------------------------------------------------------------
void FileEditor::onDependenciesChanged()
{
    fileInterface_->clearDependencies(fileName_);

    std::vector<std::string> dependenciesInSTD;
    for (auto dependency : dependenciesEditor_.items())
    {
        dependenciesInSTD.push_back(dependency.toStdString());
    }

    fileInterface_->addMultipleDependencies(fileName_, dependenciesInSTD);

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: fileeditor::onDependenciesChanged()
//-----------------------------------------------------------------------------
void FileEditor::onExportedNamesChanged()
{
    fileInterface_->clearExportedNames(fileName_);

    std::vector<std::string> exportedNamesInSTD;
    for (auto name : exportedNamesEditor_.items())
    {
        exportedNamesInSTD.push_back(name.toStdString());
    }

    fileInterface_->addMultipleExportedNames(fileName_, exportedNamesInSTD);

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: fileeditor::onDependenciesChanged()
//-----------------------------------------------------------------------------
void FileEditor::onImageTypesChanged()
{
    fileInterface_->clearImageTypes(fileName_);

    std::vector<std::string> imageTypesInSTD;
    for (auto imageType : imageTypesEditor_.items())
    {
        imageTypesInSTD.push_back(imageType.toStdString());
    }

    fileInterface_->addMultipleImageTypes(fileName_, imageTypesInSTD);

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
    buttonLayout->setSpacing(1);
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
    topLayout->addLayout(buttonLayout, 3, 0, 1, 2, Qt::AlignTop);
    topLayout->addWidget(&dependenciesEditor_, 3, 1, 1, 1);

    topLayout->setRowStretch(3, 2);

    scrollArea->setWidget(topWidget);
}

//-----------------------------------------------------------------------------
// Function: fileeditor::fileRenamed()
//-----------------------------------------------------------------------------
void FileEditor::fileRenamed(std::string const& oldName, std::string const& newName)
{
    if (oldName == fileName_)
    {
        fileName_ = newName;

        nameEditor_.fileRenamed(newName);
        fileTypeEditor_.fileRenamed(newName);
        generalEditor_.fileRenamed(newName);
        buildCommand_.fileRenamed(newName);
    }
}
