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
							   File* file):
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

	foreach (const QString item, items) {

		// empty strings are not added
		if (item.isEmpty()) {
			continue;
		}
		
		// if the string was one of the types that is defined in IP-Xact standard
		// then it is added to the fileTypes list.
		if (item == QString("unknown") ||
			item == QString("asmSource") ||
			item == QString("cSource") ||
			item == QString("cppSource") ||
			item == QString("eSource") ||
			item == QString("OVASource") ||
			item == QString("perlSource") ||
			item == QString("pslSource") ||
			item == QString("SVASource") ||
			item == QString("tclSource") ||
			item == QString("veraSource") ||
			item == QString("systemCSource") ||
			item == QString("systemCSource-2.0") ||
			item == QString("systemCSource-2.0.1") ||
			item == QString("systemCSource-2.1") ||
			item == QString("systemCSource-2.2") ||
			item == QString("systemVerilogSource") ||
			item == QString("systemVerilogSource-3.0") ||
			item == QString("systemVerilogSource-3.1") ||
			item == QString("systemVerilogSource-3.1a") ||
			item == QString("verilogSource") ||
			item == QString("verilogSource-95") ||
			item == QString("verilogSource-2001") ||
			item == QString("vhdlSource") ||
			item == QString("vhdlSource-87") ||
			item == QString("vhdlSource-93") ||
			item == QString("swObject") ||
			item == QString("swObjectLibrary") ||
			item == QString("vhdlBinaryLibrary") ||
			item == QString("verilogBinaryLibrary") ||
			item == QString("executableHdl") ||
			item == QString("unelaboratedHdl") ||
			item == QString("SDC")) {

				file_->addFileType(item);
		}

		// if the file type was defined by user then it is added to the 
		// userFileTypes list
		else {
			file_->addUserFileType(item);
		}
	}
}

void FileTypeEditor::restore() {
	QStringList fileTypes = file_->getFileTypes();
	QStringList userFileTypes = file_->getUserFileTypes();

	model_->setItems(fileTypes);
	model_->appendItems(userFileTypes);
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
