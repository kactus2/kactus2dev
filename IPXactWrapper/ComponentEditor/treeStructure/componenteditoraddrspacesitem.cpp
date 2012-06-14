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
															 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
addrSpaces_(component->getAddressSpaces()), 
editor_(component) {

	foreach (QSharedPointer<AddressSpace> addrSpace, addrSpaces_) {
		QSharedPointer<ComponentEditorAddrSpaceItem> addrItem(
			new ComponentEditorAddrSpaceItem(addrSpace, model, libHandler, component, this));	
		childItems_.append(addrItem);
	}

	editor_.hide();

	connect(&editor_, SIGNAL(contentChanged()), 
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childAdded(int)),
		this, SLOT(onAddChild(int)), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childRemoved(int)),
		this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
}

ComponentEditorAddrSpacesItem::~ComponentEditorAddrSpacesItem() {
}

QString ComponentEditorAddrSpacesItem::text() const {
	return tr("Address spaces");
}

ItemEditor* ComponentEditorAddrSpacesItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorAddrSpacesItem::editor() const {
	return &editor_;
}

QString ComponentEditorAddrSpacesItem::getTooltip() const {
	return tr("Contains the address spaces specified for the component");
}

void ComponentEditorAddrSpacesItem::onEditorChanged() {
	// call the base class implementation
	ComponentEditorItem::onEditorChanged();

	// also inform of child changes
	foreach (QSharedPointer<ComponentEditorItem> childItem, childItems_) {
		// tell the model that data has changed for the child
		emit contentChanged(childItem.data());

		// tell the child to update it's editor contents
		childItem->refreshEditor();
	}
}

void ComponentEditorAddrSpacesItem::createChild( int index ) {
	QSharedPointer<ComponentEditorAddrSpaceItem> addrItem(
		new ComponentEditorAddrSpaceItem(addrSpaces_.at(index), model_, libHandler_, component_, this));	
	childItems_.insert(index, addrItem);
}
