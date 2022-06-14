//-----------------------------------------------------------------------------
// File: filesetseditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 25.5.2012
//
// Description:
// The editor to add/remove/edit file sets of a component.
//-----------------------------------------------------------------------------

#include "filesetseditor.h"
#include "filesetsdelegate.h"

#include <common/widgets/summaryLabel/summarylabel.h>

#include <KactusAPI/include/FileSetInterface.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <KactusAPI/include/PluginManager.h>

#include <IPXACTmodels/Component/Component.h>

#include <QVBoxLayout>
#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: FileSetsEditor::FileSetsEditor()
//-----------------------------------------------------------------------------
FileSetsEditor::FileSetsEditor(QSharedPointer<Component> component, LibraryInterface* libInterface,
    QSharedPointer<ParameterFinder> parameterFinder, FileSetInterface* fileSetInterface):
ItemEditor(component, libInterface),
splitter_(Qt::Vertical, this),
view_(&splitter_),
model_(fileSetInterface, parameterFinder, this),
proxy_(this),
dependencyEditor_(component, QFileInfo(libInterface->getPath(component->getVlnv())).path(), &splitter_),
firstShow_(true),
fileSetInterface_(fileSetInterface),
availableFileSets_(component->getFileSets())
{
    fileSetInterface_->setFileSets(availableFileSets_);

    proxy_.setSourceModel(&model_);

    view_.setModel(&proxy_);
    view_.setItemDelegate(new FileSetsDelegate(this));
    view_.setItemsDraggable(false);
    view_.setAllowImportExport(true);

    QString defPath = libInterface->getDirectoryPath(ItemEditor::component()->getVlnv()) + 
        QStringLiteral("/fileSetList.csv");

    view_.setDefaultImportExportPath(defPath);

    setupLayout();

    connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&model_, SIGNAL(fileSetAdded(int)),    this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
    connect(&model_, SIGNAL(fileSetRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

    connect(&model_, SIGNAL(contentChanged()), &dependencyEditor_, SLOT(refresh()), Qt::UniqueConnection);

    connect(&dependencyEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&dependencyEditor_, SIGNAL(dependenciesChanged()),
        this, SIGNAL(dependenciesChanged()), Qt::UniqueConnection);
    connect(&dependencyEditor_, SIGNAL(fileSetAdded(FileSet*)),
        &model_, SLOT(onFileSetAdded(FileSet*)), Qt::UniqueConnection);
    connect(&dependencyEditor_, SIGNAL(fileAdded(File*)), this, SIGNAL(fileAdded(File*)), Qt::UniqueConnection);
    connect(&dependencyEditor_, SIGNAL(filesUpdated()), this, SIGNAL(filesUpdated()), Qt::UniqueConnection);

    connect(&view_, SIGNAL(addItem(const QModelIndex&)),
        &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
        &model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

    connect(&model_, SIGNAL(decreaseReferences(QString)), this,
        SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FileSetsEditor::refresh()
//-----------------------------------------------------------------------------
void FileSetsEditor::refresh()
{
    fileSetInterface_->setFileSets(availableFileSets_);

    view_.update();
}

//-----------------------------------------------------------------------------
// Function: FileSetsEditor::showEvent()
//-----------------------------------------------------------------------------
void FileSetsEditor::showEvent( QShowEvent* event )
{
    QWidget::showEvent(event);
    emit helpUrlRequested("componenteditor/filesets.html");

    if (firstShow_)
    {
        dependencyEditor_.scan();
        firstShow_ = false;
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetsEditor::refreshDependencyModel()
//-----------------------------------------------------------------------------
void FileSetsEditor::refreshDependencyModel()
{
    dependencyEditor_.refresh();
}

//-----------------------------------------------------------------------------
// Function: FileSetsEditor::setupLayout()
//-----------------------------------------------------------------------------
void FileSetsEditor::setupLayout()
{
    SummaryLabel* summaryLabel = new SummaryLabel(tr("File sets summary"), this);

    QGroupBox* dependencyWidget = new QGroupBox(tr("File dependencies"), this);

    QVBoxLayout* dependencyLayout = new QVBoxLayout(dependencyWidget);
    dependencyLayout->addWidget(&dependencyEditor_);

    splitter_.addWidget(&view_);
    splitter_.addWidget(dependencyWidget);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
    layout->addWidget(&splitter_, 1);
    layout->setContentsMargins(0, 0, 0, 0);
}
