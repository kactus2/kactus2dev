/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorcominterfacesitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorcominterfacesitem.h"

ComponentEditorComInterfacesItem::ComponentEditorComInterfacesItem(ComponentEditorTreeModel* model, 
																   LibraryInterface* libHandler,
																   QSharedPointer<Component> component,
																   QWidget* widget,
																   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent) {

}

ComponentEditorComInterfacesItem::~ComponentEditorComInterfacesItem() {
}

QString ComponentEditorComInterfacesItem::text() const {
	return tr("COM interfaces");
}

bool ComponentEditorComInterfacesItem::isValid() const {
	return true;
}

ItemEditor* ComponentEditorComInterfacesItem::editor() {
	return NULL;
}
