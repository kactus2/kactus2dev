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
																   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
interfaces_(component->getComInterfaces()) {

	foreach (QSharedPointer<ComInterface> iface, interfaces_) {
		QSharedPointer<ComponentEditorComInterfaceItem> interfaceItem(new ComponentEditorComInterfaceItem(
			iface, model, libHandler, component, this));
		childItems_.append(interfaceItem);
	}
}

ComponentEditorComInterfacesItem::~ComponentEditorComInterfacesItem() {
}

QString ComponentEditorComInterfacesItem::text() const {
	return tr("COM interfaces");
}

ItemEditor* ComponentEditorComInterfacesItem::editor() {
	return NULL;
}

const ItemEditor* ComponentEditorComInterfacesItem::editor() const {
	return NULL;
}

QString ComponentEditorComInterfacesItem::getTooltip() const {
	return tr("Contains the COM interfaces of a component");
}
