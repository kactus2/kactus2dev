/* 
 *
 *  Created on: 4.2.2011
 *      Author: Antti Kamppi
 * 		filename: groupmanager.cpp
 */

#include "groupmanager.h"
#include <common/dialogs/comboSelector/comboselector.h>


GroupManager::GroupManager(const QString title /*= tr("Group identifiers")*/, 
						   QWidget *parent /*= 0*/, 
						   const QStringList& items /*= QStringList()*/ ):
ListManager(title, parent, items) {
}

GroupManager::~GroupManager() {
}

void GroupManager::onAdd() {


	// ask user to select a value or create own
	QString item = ComboSelector::selectGroupType(this);

	// if user wrote an empty string or if clicked cancel
	if (item.isEmpty()) {
		return;
	}

	model_.appendItem(item);

	// inform that content of the widget has changed
	emit contentChanged();
	return;
}

void GroupManager::onEdit() {

	// ask the view for the selected index
	QModelIndex index = view_.currentIndex();

	// if index was invalid (nothing was chosen)
	if (!index.isValid()) {
		return;
	}

	// get the old text that is currently stored in the model
	QString oldText = model_.data(index, Qt::DisplayRole).toString();

	// ask the user to set the new text
	QString newText = ComboSelector::selectGroupType(this, oldText);

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
