/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditoraddrspacesitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditoraddrspacesitem.h"
#include "componenteditoraddrspaceitem.h"

ComponentEditorAddrSpacesItem::ComponentEditorAddrSpacesItem(ComponentEditorTreeModel* model,
															 LibraryInterface* libHandler,
															 QSharedPointer<Component> component,
															 QWidget* widget, 
															 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
addrSpaces_(component->getAddressSpaces()) {

	foreach (QSharedPointer<AddressSpace> addrSpace, addrSpaces_) {
		ComponentEditorAddrSpaceItem* addrItem = 
			new ComponentEditorAddrSpaceItem(addrSpace, model, libHandler, component, widget, this);	
		childItems_.append(addrItem);
	}
}

ComponentEditorAddrSpacesItem::~ComponentEditorAddrSpacesItem() {
}

QString ComponentEditorAddrSpacesItem::text() const {
	return tr("Address spaces");
}

bool ComponentEditorAddrSpacesItem::isValid() const {
	foreach (QSharedPointer<AddressSpace> addrSpace, addrSpaces_) {
		if (!addrSpace->isValid()) {
			return false;
		}
	}
	return true;
}

ItemEditor* ComponentEditorAddrSpacesItem::editor() {
	return NULL;
}
