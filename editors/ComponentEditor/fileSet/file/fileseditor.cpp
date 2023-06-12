//-----------------------------------------------------------------------------
// File: fileseditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 1.6.2012
//
// Description:
// The editor to add/remove/edit files of a file set.
//-----------------------------------------------------------------------------

#include "fileseditor.h"
#include "filesdelegate.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <KactusAPI/include/FileInterface.h>

#include <IPXACTmodels/Component/Component.h>

#include <QVBoxLayout>

#include <QStringList>
#include <QFileDialog>

//-----------------------------------------------------------------------------
// Function: fileseditor::FilesEditor()
//-----------------------------------------------------------------------------
FilesEditor::FilesEditor(QSharedPointer<QList<QSharedPointer<File>>> availableFiles, FileInterface* fileInterface,
    QSharedPointer<Component> component, LibraryInterface* handler,
    QWidget *parent, const QString& title):
QGroupBox(title, parent),
handler_(handler),
component_(component),
view_(handler, component, this),
model_(handler, component, fileInterface, this),
fileInterface_(fileInterface),
availableFiles_(availableFiles)
{
	setFlat(true);

    fileInterface_->setFiles(availableFiles_);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);

    view_.setAlternatingRowColors(false);

    view_.setAcceptDrops(true);
    view_.setDropIndicatorShown(true);
    
	view_.setAllowImportExport(true);

	view_.setModel(&model_);

	// the order of files must be maintained
	view_.setSortingEnabled(false);

	view_.setItemsDraggable(true);

	// set the delegate to provide editors
	view_.setItemDelegate(new FilesDelegate(component->getRevision(), this));

	connect(&view_, SIGNAL(addItemByBrowsing()), this, SLOT(onAddFiles()), Qt::UniqueConnection);
    connect(&view_, SIGNAL(addItem(const QModelIndex&)),
        &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
        &model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(moveItem(const QModelIndex&, const QModelIndex&)),
        &model_, SLOT(onMoveItem(const QModelIndex&, const QModelIndex&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	// the signals from model informing that a child is added/removed
	connect(&model_, SIGNAL(fileAdded(int)), this, SIGNAL(fileAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(fileRemoved(int)), this, SIGNAL(fileRemoved(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(fileMoved(int, int)), this, SIGNAL(fileMoved(int, int)), Qt::UniqueConnection);

    connect(&model_, SIGNAL(fileRenamed(std::string const&, std::string const&)),
        this, SIGNAL(fileRenamed(std::string const&, std::string const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: fileseditor::~FilesEditor()
//-----------------------------------------------------------------------------
FilesEditor::~FilesEditor()
{

}

//-----------------------------------------------------------------------------
// Function: fileseditor::refresh()
//-----------------------------------------------------------------------------
void FilesEditor::refresh()
{
    fileInterface_->setFiles(availableFiles_);

	view_.update();
}

//-----------------------------------------------------------------------------
// Function: fileseditor::onAddFiles()
//-----------------------------------------------------------------------------
void FilesEditor::onAddFiles()
{
	QStringList filePaths = QFileDialog::getOpenFileNames(this, tr("Select files to add."),
		handler_->getPath(component_->getVlnv()));

	for (QString const& file : filePaths)
    {
		model_.onAddItem(QModelIndex(), file);
	}
}
