/* 
 *
 *  Created on: 5.2.2011
 *      Author: Antti Kamppi
 * 		filename: fileeditor.cpp
 */

#include "fileeditor.h"

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/file.h>
#include <library/LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: fileeditor::FileEditor()
//-----------------------------------------------------------------------------
FileEditor::FileEditor( LibraryInterface* handler,
					   QSharedPointer<Component> component, 
					   QSharedPointer<File> file, 
					   QWidget *parent ):
ItemEditor(component, handler, parent),
file_(file),
nameEditor_(file_, this),
editButton_(new QPushButton(QIcon(":/icons/common/graphics/settings-code_editor.png"), tr("Edit file"), this)),
runButton_(new QPushButton(QIcon(":/icons/common/graphics/plugin-generator.png"), tr("Run file"), this)),
generalEditor_(this, file),
fileTypeEditor_(this, file),
buildCommand_(this, handler, component, file),
dependenciesEditor_(tr("Dependencies"), handler, component, this),
exportedNamesEditor_(tr("Exported names"), this),
imageTypesEditor_(tr("Image types"), this)
{
    dependenciesEditor_.initialize();
    exportedNamesEditor_.initialize();
    imageTypesEditor_.initialize();
    fileTypeEditor_.initialize();

    editButton_->setIconSize(QSize(16, 16));
    runButton_->setIconSize(QSize(16, 16));

    setupLayout();

    connect(editButton_, SIGNAL(clicked(bool)), this, SIGNAL(editFile()), Qt::UniqueConnection);
    connect(runButton_, SIGNAL(clicked(bool)), this, SIGNAL(runFile()), Qt::UniqueConnection);
    connect(&generalEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&fileTypeEditor_, SIGNAL(contentChanged()), this, SLOT(onFileTypesChanged()), Qt::UniqueConnection);
    connect(&buildCommand_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&dependenciesEditor_, SIGNAL(contentChanged()),
        this, SLOT(onDependenciesChanged()), Qt::UniqueConnection);
    connect(&exportedNamesEditor_, SIGNAL(contentChanged()),
        this, SLOT(onExportedNamesChanged()), Qt::UniqueConnection);
    connect(&imageTypesEditor_, SIGNAL(contentChanged()), this, SLOT(onImageTypesChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: fileeditor::~FileEditor()
//-----------------------------------------------------------------------------
FileEditor::~FileEditor()
{

}

//-----------------------------------------------------------------------------
// Function: fileeditor::isValid()
//-----------------------------------------------------------------------------
bool FileEditor::isValid() const
{
    return fileTypeEditor_.isValid() && nameEditor_.isValid();
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
    dependenciesEditor_.setItems(file_->getDependencies());
    exportedNamesEditor_.setItems(file_->getExportedNames());
    imageTypesEditor_.setItems(file_->getImageTypes());
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
    QStringList fileTypes = fileTypeEditor_.items();
    file_->setAllFileTypes(fileTypes);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: fileeditor::onDependenciesChanged()
//-----------------------------------------------------------------------------
void FileEditor::onDependenciesChanged()
{
    file_->setDependencies(dependenciesEditor_.items());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: fileeditor::onDependenciesChanged()
//-----------------------------------------------------------------------------
void FileEditor::onExportedNamesChanged()
{
    file_->setExportedNames(exportedNamesEditor_.items());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: fileeditor::onDependenciesChanged()
//-----------------------------------------------------------------------------
void FileEditor::onImageTypesChanged()
{
    file_->setImageTypes(imageTypesEditor_.items());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: fileeditor::setupLayout()
//-----------------------------------------------------------------------------
void FileEditor::setupLayout()
{
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(editButton_, 0, Qt::AlignTop);
    buttonLayout->addWidget(runButton_, 0, Qt::AlignTop);
    buttonLayout->addStretch(1);

    nameEditor_.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QVBoxLayout* leftSideLayout = new QVBoxLayout();
    leftSideLayout->addWidget(&nameEditor_, 0, Qt::AlignTop);
    leftSideLayout->addLayout(buttonLayout);
    leftSideLayout->addWidget(&generalEditor_, 0, Qt::AlignTop);
    leftSideLayout->addWidget(&buildCommand_, 0, Qt::AlignTop);

    QHBoxLayout* topOfPageLayout = new QHBoxLayout();
    topOfPageLayout->addLayout(leftSideLayout);
    topOfPageLayout->addWidget(&fileTypeEditor_);

    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(&dependenciesEditor_);
    bottomLayout->addWidget(&exportedNamesEditor_);
    bottomLayout->addWidget(&imageTypesEditor_);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addLayout(topOfPageLayout);
    topLayout->addLayout(bottomLayout);
    topLayout->addStretch(1);
}