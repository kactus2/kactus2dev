/* 
 *
 *  Created on: 7.2.2011
 *      Author: Antti Kamppi
 * 		filename: filetypeeditor.cpp
 */

#include "filetypeeditor.h"

#include <common/dialogs/comboSelector/comboselector.h>

FileTypeEditor::FileTypeEditor(QWidget *parent,
							   File* file,
							   const QStringList& items /*= QStringList()*/ ):
ListManager(tr("Specified file types"), parent, items), file_(file) {

	Q_ASSERT_X(file_, "FileTypeEditor constructor",
		"Null File-pointer given as parameter");

	view_.setProperty("mandatoryField", true);

	restore();
}

FileTypeEditor::~FileTypeEditor() {
}

void FileTypeEditor::onAdd() {

	QString item = ComboSelector::selectFileType(this);

	// if user wrote an empty string or if clicked cancel
	if (item.isEmpty()) {
		return;
	}

	model_.appendItem(item);

	// inform that content of the widget has changed
	emit contentChanged();
	return;
}

void FileTypeEditor::onEdit() {
	// ask the view for the selected index
	QModelIndex index = view_.currentIndex();

	// if index was invalid (nothing was chosen)
	if (!index.isValid()) {
		return;
	}

	// get the old text that is currently stored in the model
	QString oldText = model_.data(index, Qt::DisplayRole).toString();

	// ask the user to set the new text
	QString newText = ComboSelector::selectFileType(this, oldText);

	// if empty string was given or cancel was pressed
	if (newText.isEmpty()) {
		return;
	}

	// if user changed the text
	else if (oldText != newText) {
		model_.replace(index, newText);

		// inform that content of the widget has changed
		emit contentChanged();
	}
}

void FileTypeEditor::apply() {
	// get all items from the model
	QStringList items = model_.items();

	// remove all previous file types and userFileTypes from the model
	file_->clearFileTypes();

	foreach (const QString item, items) {
		
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

	model_.setItems(fileTypes);
	model_.appendItems(userFileTypes);
}

bool FileTypeEditor::isValid() const {
	
	// atleast one file type has to be specified
	if (model_.rowCount() <= 0) 
		return false;

	return true;
}
