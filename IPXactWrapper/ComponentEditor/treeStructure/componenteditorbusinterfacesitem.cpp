/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorbusinterfacesitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorbusinterfacesitem.h"
#include "componenteditorbusinterfaceitem.h"

ComponentEditorBusInterfacesItem::ComponentEditorBusInterfacesItem(ComponentEditorTreeModel* model,
																   LibraryInterface* libHandler,
																   QSharedPointer<Component> component,
																   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
busifs_(component->getBusInterfaces()),
editor_(libHandler, component) {

	foreach (QSharedPointer<BusInterface> busif, busifs_) {
		QSharedPointer<ComponentEditorBusInterfaceItem> busifItem(
			new ComponentEditorBusInterfaceItem(busif, model, libHandler, component, this));
		childItems_.append(busifItem);
	}

	editor_.hide();

	connect(&editor_, SIGNAL(contentChanged()), 
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childAdded(int)),
		this, SLOT(onAddChild(int)), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childRemoved(int)),
		this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
}

ComponentEditorBusInterfacesItem::~ComponentEditorBusInterfacesItem() {
}

QString ComponentEditorBusInterfacesItem::text() const {
	return tr("Bus interfaces");
}

ItemEditor* ComponentEditorBusInterfacesItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorBusInterfacesItem::editor() const {
	return &editor_;
}

QString ComponentEditorBusInterfacesItem::getTooltip() const {
	return tr("Contains the bus interfaces specified for a component");
}

void ComponentEditorBusInterfacesItem::createChild( int index ) {
	QSharedPointer<ComponentEditorBusInterfaceItem> busifItem(
		new ComponentEditorBusInterfaceItem(busifs_.at(index),
		model_, libHandler_, component_, this));

	childItems_.insert(index, busifItem);
}
