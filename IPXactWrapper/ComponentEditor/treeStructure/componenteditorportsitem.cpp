/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorportsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorportsitem.h"

ComponentEditorPortsItem::ComponentEditorPortsItem(ComponentEditorTreeModel* model,
												   LibraryInterface* libHandler,
												   QSharedPointer<Component> component,
												   QWidget* widget, 
												   ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent) {

}

ComponentEditorPortsItem::~ComponentEditorPortsItem() {
}

QString ComponentEditorPortsItem::text() const {
	return tr("Ports");
}

bool ComponentEditorPortsItem::isValid() const {
	return true;
}

ItemEditor* ComponentEditorPortsItem::editor() {
	return NULL;
}
