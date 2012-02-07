/* 
 *
 *  Created on: 4.4.2011
 *      Author: Antti Kamppi
 * 		filename: channelinterfacemanager.cpp
 */

#include "channelinterfacemanager.h"

#include "channelinterfacemanagerdelegate.h"
#include "channelinterfacemodel.h"

#include <common/dialogs/comboSelector/comboselector.h>
#include <models/channel.h>
#include <models/component.h>

#include <QComboBox>
#include <QHBoxLayout>
#include <QLayout>

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

bool ChannelInterfaceManager::isValid() const {
	Q_ASSERT(model_);

	QStringList items = model_->items();

	// at least 2 interfaces must have been referenced.
	if (items.size() < 2) {
		return false;
	}

	foreach (QString item, items) {

		// if the component doesn't contain the referenced interface.
		if (!component_->hasInterface(item)) {
			return false;
		}
	}

	// model was fine so everything is fine.
	return true;
}

void ChannelInterfaceManager::initialize( const QStringList& items /*= QStringList()*/ ) {
	// remove the previous model and view if there are one
	if (model_) {
		delete model_;
		model_ = 0;
	}
	if (view_) {
		delete view_;
		view_ = 0;
	}

	// create new model and view
	model_ = new ChannelInterfaceModel(this, component_, items);

	view_ = new EditableListView(this);

	// the signals from the view
	connect(view_, SIGNAL(removeItem(const QModelIndex&)),
		model_, SLOT(remove(const QModelIndex&)), Qt::UniqueConnection);

	connect(view_, SIGNAL(addItem(const QModelIndex&)),
		model_, SLOT(addItem(const QModelIndex&)), Qt::UniqueConnection);

	connect(view_, SIGNAL(moveItem(const QModelIndex&, const QModelIndex&)),
		model_, SLOT(moveItem(const QModelIndex&, const QModelIndex&)), Qt::UniqueConnection);

	// the signals from the model
	connect(model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	connect(model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	QLayout* topLayout = layout();

	if (!topLayout) {
		// the top layout of the widget
		topLayout = new QHBoxLayout(this);
	}

	// add the view on the left side
	topLayout->addWidget(view_);

	// connect the model to the view
	view_->setModel(model_);

	view_->setItemDelegate(new ChannelInterfaceManagerDelegate(this, component_));

	view_->setProperty("mandatoryField", true);
}
