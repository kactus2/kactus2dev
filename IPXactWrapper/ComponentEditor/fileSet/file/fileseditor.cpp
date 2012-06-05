/* 
 *  	Created on: 1.6.2012
 *      Author: Antti Kamppi
 * 		filename: fileseditor.cpp
 *		Project: Kactus 2
 */

#include "fileseditor.h"
#include <LibraryManager/libraryinterface.h>

#include <QVBoxLayout>
#include <QIcon>
#include <QStringList>
#include <QFileDialog>

FilesEditor::FilesEditor( QSharedPointer<Component> component, 
						 QSharedPointer<FileSet> fileSet, 
						 LibraryInterface* handler,
						 QWidget *parent,
						 const QString& title):
QGroupBox(title, parent),
handler_(handler),
component_(component),
fileSet_(fileSet),
files_(fileSet->getFiles()),
view_(handler, component, this),
model_(handler, component, fileSet, this),
addFilesButton_(QIcon(":/icons/graphics/add.png"), tr("Add Files"), this) {

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);
	layout->addWidget(&addFilesButton_, 0, Qt::AlignLeft);

	view_.setModel(&model_);

	view_.setColumnWidth(0, 200);
	view_.setColumnWidth(1, 300);
	view_.setColumnWidth(2, 170);

	// the order of files must be maintained
	view_.setSortingEnabled(false);

	// TODO change this
	view_.setItemsDraggable(true);

	// set the delegate to provide editors
	//view_.setItemDelegate(new FileBuildersDelegate(this));

	connect(&addFilesButton_, SIGNAL(clicked(bool)),
		this, SLOT(onAddFiles()), Qt::UniqueConnection);
	connect(&view_, SIGNAL(addItem(const QModelIndex&, const QString&)),
		&model_, SLOT(onAddItem(const QModelIndex&, const QString&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(moveItem(const QModelIndex&, const QModelIndex&)),
		&model_, SLOT(onMoveItem(const QModelIndex&, const QModelIndex&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	// the signals from model informing that a child is added/removed
	connect(&model_, SIGNAL(fileAdded(int)),
		this, SIGNAL(fileAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(fileRemoved(int)),
		this, SIGNAL(fileRemoved(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(fileMoved(int, int)),
		this, SIGNAL(fileMoved(int, int)), Qt::UniqueConnection);
}

FilesEditor::~FilesEditor() {
}

bool FilesEditor::isValid() const {
	return true;
}

void FilesEditor::refresh() {
	view_.update();
}

void FilesEditor::onAddFiles() {

	QStringList files = QFileDialog::getOpenFileNames(this, tr("Select files to add."),
		handler_->getPath(*component_->getVlnv()));

	if (files.isEmpty()) {
		return;
	}

	// add all files to the model
	foreach (QString file, files) {
		model_.onAddItem(QModelIndex(), file);
	}
}
