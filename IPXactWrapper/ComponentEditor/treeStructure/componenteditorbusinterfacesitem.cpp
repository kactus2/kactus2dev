/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorbusinterfacesitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorbusinterfacesitem.h"

ComponentEditorBusInterfacesItem::ComponentEditorBusInterfacesItem(ComponentEditorTreeModel* model,
																   LibraryInterface* libHandler,
																   QSharedPointer<Component> component,
																   QWidget* widget,
																   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent) {

}

ComponentEditorBusInterfacesItem::~ComponentEditorBusInterfacesItem() {
}

QString ComponentEditorBusInterfacesItem::text() const {
	return tr("Bus interfaces");
}

bool ComponentEditorBusInterfacesItem::isValid() const {
	return true;
}

ItemEditor* ComponentEditorBusInterfacesItem::editor() {
	return NULL;
}
