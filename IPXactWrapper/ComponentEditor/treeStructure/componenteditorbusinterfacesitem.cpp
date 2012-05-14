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
																   QWidget* widget,
																   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
busifs_(component->getBusInterfaces()) {

	foreach (QSharedPointer<BusInterface> busif, busifs_) {
		ComponentEditorBusInterfaceItem* busifItem(
			new ComponentEditorBusInterfaceItem(busif, model, libHandler, component, widget, this));
		childItems_.append(busifItem);
	}
}

ComponentEditorBusInterfacesItem::~ComponentEditorBusInterfacesItem() {
}

QString ComponentEditorBusInterfacesItem::text() const {
	return tr("Bus interfaces");
}

bool ComponentEditorBusInterfacesItem::isValid() const {
	foreach (QSharedPointer<BusInterface> busif, busifs_) {
		if (!busif->isValid(component_->getPortBounds())) {
			return false;
		}
	}
	return true;
}

ItemEditor* ComponentEditorBusInterfacesItem::editor() {
	return NULL;
}
