/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorcominterfacesitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorcominterfacesitem.h"
#include "componenteditorcominterfaceitem.h"

ComponentEditorComInterfacesItem::ComponentEditorComInterfacesItem(ComponentEditorTreeModel* model, 
																   LibraryInterface* libHandler,
																   QSharedPointer<Component> component,
																   QWidget* widget,
																   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
interfaces_(component->getComInterfaces()) {

	foreach (QSharedPointer<ComInterface> iface, interfaces_) {
		ComponentEditorComInterfaceItem* interfaceItem = new ComponentEditorComInterfaceItem(
			iface, model, libHandler, component, widget, this);
		childItems_.append(interfaceItem);
	}
}

ComponentEditorComInterfacesItem::~ComponentEditorComInterfacesItem() {
}

QString ComponentEditorComInterfacesItem::text() const {
	return tr("COM interfaces");
}

bool ComponentEditorComInterfacesItem::isValid() const {
	foreach (QSharedPointer<ComInterface> iface, interfaces_) {
		if (!iface->isValid()) {
			return false;
		}
	}
	return true;
}

ItemEditor* ComponentEditorComInterfacesItem::editor() {
	return NULL;
}
