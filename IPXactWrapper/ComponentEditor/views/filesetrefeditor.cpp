/* 
 *
 *  Created on: 15.4.2011
 *      Author: Antti Kamppi
 * 		filename: filesetrefeditor.cpp
 */

#include "filesetrefeditor.h"

#include <models/component.h>
#include <common/dialogs/comboSelector/comboselector.h>

FileSetRefEditor::FileSetRefEditor(QSharedPointer<Component> component,
								   const QString title /*= tr("List")*/, 
								   QWidget *parent /*= 0*/, 
								   const QStringList& items /*= QStringList()*/ ):
ListManager(title, parent, items),
component_(component) {
}

FileSetRefEditor::~FileSetRefEditor() {
}

void FileSetRefEditor::onAdd() {
	QString item = ComboSelector::selectFileSet(component_, this);

	// if user wrote an empty string or if clicked cancel
	if (item.isEmpty()) {
		return;
	}

	model_.appendItem(item);

	// inform that content of the widget has changed
	emit contentChanged();
	return;
}

void FileSetRefEditor::onEdit() {
	// ask the view for the selected index
	QModelIndex index = view_.currentIndex();

	// if index was invalid (nothing was chosen)
	if (!index.isValid()) {
		return;
	}

	// get the old text that is currently stored in the model
	QString oldText = model_.data(index, Qt::DisplayRole).toString();

	// ask the user to set the new text
	QString newText = ComboSelector::selectFileSet(component_, this, oldText);

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
