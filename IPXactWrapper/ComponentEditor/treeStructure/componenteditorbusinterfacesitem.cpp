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
busifs_(component->getBusInterfaces()) {

	foreach (QSharedPointer<BusInterface> busif, busifs_) {
		QSharedPointer<ComponentEditorBusInterfaceItem> busifItem(
			new ComponentEditorBusInterfaceItem(busif, model, libHandler, component, this));
		childItems_.append(busifItem);
	}
}

ComponentEditorBusInterfacesItem::~ComponentEditorBusInterfacesItem() {
}

QString ComponentEditorBusInterfacesItem::text() const {
	return tr("Bus interfaces");
}

ItemEditor* ComponentEditorBusInterfacesItem::editor() {
	return NULL;
}

const ItemEditor* ComponentEditorBusInterfacesItem::editor() const {
	return NULL;
}

QString ComponentEditorBusInterfacesItem::getTooltip() const {
	return tr("Contains the bus interfaces specified for a component");
}
