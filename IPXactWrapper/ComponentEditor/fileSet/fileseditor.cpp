/* 
 *  	Created on: 1.6.2012
 *      Author: Antti Kamppi
 * 		filename: fileseditor.cpp
 *		Project: Kactus 2
 */

#include "fileseditor.h"

#include <QVBoxLayout>
#include <QIcon>

FilesEditor::FilesEditor( QSharedPointer<Component> component, 
						 QSharedPointer<FileSet> fileSet, 
						 QWidget *parent,
						 const QString& title):
QGroupBox(title, parent),
files_(fileSet->getFiles()),
view_(this),
model_(fileSet->getFiles(), this),
addFilesButton_(QIcon(":/icons/graphics/add.png"), tr("Add Files"), this) {

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);
	layout->addWidget(&addFilesButton_, 0, Qt::AlignLeft);

	view_.setModel(&model_);

	connect(&addFilesButton_, SIGNAL(clicked(bool)),
		this, SLOT(onAddFiles()), Qt::UniqueConnection);
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

}
