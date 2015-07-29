//-----------------------------------------------------------------------------
// File: filesetseditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 25.5.2012
//
// Description:
// The editor to add/remove/edit file sets of a component.
//-----------------------------------------------------------------------------

#include "filesetseditor.h"
#include "filesetsdelegate.h"

#include <common/widgets/summaryLabel/summarylabel.h>
#include <library/LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: FileSetsEditor::FileSetsEditor()
//-----------------------------------------------------------------------------
FileSetsEditor::FileSetsEditor(QSharedPointer<Component> component, LibraryInterface* libInterface, 
    PluginManager& pluginMgr):
ItemEditor(component, libInterface),
    splitter_(Qt::Vertical, this),
    view_(&splitter_),
    model_(component, this),
    proxy_(this),
    dependencyEditor_(component, QFileInfo(libInterface->getPath(*component->getVlnv())).path(), pluginMgr, 
        &splitter_),
    firstShow_(true)
{
    splitter_.addWidget(&view_);
    splitter_.addWidget(&dependencyEditor_);

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("File sets summary"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&splitter_, 1);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_.setSourceModel(&model_);

    view_.setModel(&proxy_);
    view_.setItemDelegate(new FileSetsDelegate(this));
    view_.setItemsDraggable(false);
	view_.setAllowImportExport(true);

	const QString compPath = ItemEditor::handler()->getDirectoryPath(*ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/fileSetList.csv").arg(compPath);
	view_.setDefaultImportExportPath(defPath);

	connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(fileSetAdded(int)),	this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(fileSetRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

    connect(&model_, SIGNAL(contentChanged()), &dependencyEditor_, SLOT(refresh()), Qt::UniqueConnection);

    connect(&dependencyEditor_, SIGNAL(contentChanged()),
            this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&dependencyEditor_, SIGNAL(dependenciesChanged()),
            this, SIGNAL(dependenciesChanged()), Qt::UniqueConnection);
    connect(&dependencyEditor_, SIGNAL(fileSetAdded(FileSet*)),
            &model_, SLOT(onFileSetAdded(FileSet*)), Qt::UniqueConnection);
    connect(&dependencyEditor_, SIGNAL(fileAdded(File*)),
            this, SIGNAL(fileAdded(File*)), Qt::UniqueConnection);
    connect(&dependencyEditor_, SIGNAL(filesUpdated()),
            this, SIGNAL(filesUpdated()), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
        &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FileSetsEditor::~FileSetsEditor()
//-----------------------------------------------------------------------------
FileSetsEditor::~FileSetsEditor()
{

}

//-----------------------------------------------------------------------------
// Function: FileSetsEditor::isValid()
//-----------------------------------------------------------------------------
bool FileSetsEditor::isValid() const
{
	return model_.isValid();
}

//-----------------------------------------------------------------------------
// Function: FileSetsEditor::refresh()
//-----------------------------------------------------------------------------
void FileSetsEditor::refresh()
{
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
