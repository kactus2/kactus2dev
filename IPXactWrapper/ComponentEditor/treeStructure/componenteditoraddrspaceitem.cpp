/* 
 *  	Created on: 10.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditoraddrspaceitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditoraddrspaceitem.h"

ComponentEditorAddrSpaceItem::ComponentEditorAddrSpaceItem(QSharedPointer<AddressSpace> addrSpace,
														   ComponentEditorTreeModel* model,
														   LibraryInterface* libHandler,
														   QSharedPointer<Component> component,
														   QWidget* widget, 
														   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
addrSpace_(addrSpace),
editor_(component, addrSpace.data(), widget) {
}

ComponentEditorAddrSpaceItem::~ComponentEditorAddrSpaceItem() {
}

QString ComponentEditorAddrSpaceItem::text() const {
	return addrSpace_->getName();
}

bool ComponentEditorAddrSpaceItem::isValid() const {
	if (!addrSpace_->isValid()) {
		return false;
	}
	return editor_.isValid();
}

ItemEditor* ComponentEditorAddrSpaceItem::editor() {
	return &editor_;
}
