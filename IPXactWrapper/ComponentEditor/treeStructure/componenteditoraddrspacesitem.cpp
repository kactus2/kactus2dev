/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditoraddrspacesitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditoraddrspacesitem.h"

ComponentEditorAddrSpacesItem::ComponentEditorAddrSpacesItem(ComponentEditorTreeModel* model,
															 LibraryInterface* libHandler,
															 QSharedPointer<Component> component,
															 QWidget* widget, 
															 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
addSpaces_(component->getAddressSpaces()) {

}

ComponentEditorAddrSpacesItem::~ComponentEditorAddrSpacesItem() {
}

QString ComponentEditorAddrSpacesItem::text() const {
	return tr("Address spaces");
}

bool ComponentEditorAddrSpacesItem::isValid() const {
	return true;
}

ItemEditor* ComponentEditorAddrSpacesItem::editor() {
	return NULL;
}
