/* 
 *
 *  Created on: 4.4.2011
 *      Author: Antti Kamppi
 * 		filename: channelinterfacemanager.cpp
 */

#include "channelinterfacemanager.h"

#include <common/dialogs/comboSelector/comboselector.h>
#include <models/channel.h>
#include <models/component.h>

#include <QComboBox>

ChannelInterfaceManager::ChannelInterfaceManager(
	QSharedPointer<Component> component,
	void* dataPointer,
	QWidget *parent): 
ListManager(tr("Bus interface references"), parent), 
channel_(static_cast<Channel*>(dataPointer)),
component_(component) {

	Q_ASSERT_X(component, "ChannelInterfaceManager constructor",
		"Null Component-pointer given as parameter");
	Q_ASSERT_X(dataPointer, "ChannelInterfaceManager constructor",
		"Null dataPointer given as parameter");
}

ChannelInterfaceManager::~ChannelInterfaceManager() {
}

void ChannelInterfaceManager::onAdd() {
	
	QString selected = ComboSelector::selectBusInterface(component_, this);

	// is user clicked cancel.
	if (selected.isEmpty())
		return;

	model_.appendItem(selected);

	emit contentChanged();
}

void ChannelInterfaceManager::onEdit() {
	
	// ask the view for the selected index
	QModelIndex index = view_.currentIndex();

	// if index was invalid (nothing was chosen)
	if (!index.isValid()) {
		return;
	}

	// get the old text that is currently stored in the model
	QString oldText = model_.data(index, Qt::DisplayRole).toString();

	// ask the user to set the new text
	QString newText = ComboSelector::selectBusInterface(component_,
		this, oldText);

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

bool ChannelInterfaceManager::isValid() const {
	// atleast two interfaces must be specified.
	return (model_.rowCount() > 1);
}
