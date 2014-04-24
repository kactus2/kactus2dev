/* 
 *
 *  Created on: 7.2.2011
 *      Author: Antti Kamppi
 * 		filename: filetypeeditor.cpp
 */

#include "filetypeeditor.h"

#include <common/dialogs/comboSelector/comboselector.h>

#include "filetypeeditordelegate.h"

FileTypeEditor::FileTypeEditor(QWidget *parent,
							  QSharedPointer<File> file):
ListManager(tr("Specified file types"), parent), 
file_(file) {

	Q_ASSERT_X(file_, "FileTypeEditor constructor",
		"Null File-pointer given as parameter");
}

FileTypeEditor::~FileTypeEditor() {
}

void FileTypeEditor::apply() {
	// get all items from the model
	QStringList items = model_->items();

	// remove all previous file types and userFileTypes from the model
	file_->clearFileTypes();
	file_->setAllFileTypes(items);
}

void FileTypeEditor::restore() {
	QStringList fileTypes = file_->getAllFileTypes();
	model_->setItems(fileTypes);
}

bool FileTypeEditor::isValid() const {
	
	// at least one file type has to be specified
	if (model_->rowCount() <= 0) {
		return false;
	}

	// get all items from the model
	QStringList items = model_->items();
	foreach (QString item, items) {
		if (item.isEmpty()) {
			return false;
		}
	}

	return true;
}

void FileTypeEditor::initialize( const QStringList& items /*= QStringList()*/ ) {
	ListManager::initialize(items);

	view_->setProperty("mandatoryField", true);

	view_->setItemDelegate(new FileTypeEditorDelegate(this));

	restore();
}
