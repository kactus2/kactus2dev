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
editor_(libHandler, component),
interfaces_(component->getComInterfaces()) {

	foreach (QSharedPointer<ComInterface> iface, interfaces_) {
		QSharedPointer<ComponentEditorComInterfaceItem> interfaceItem(new ComponentEditorComInterfaceItem(
			iface, model, libHandler, component, this));
		childItems_.append(interfaceItem);
	}
	editor_.hide();

	connect(&editor_, SIGNAL(contentChanged()), 
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childAdded(int)),
		this, SLOT(onAddChild(int)), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childRemoved(int)),
		this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
}

ComponentEditorComInterfacesItem::~ComponentEditorComInterfacesItem() {
}

QString ComponentEditorComInterfacesItem::text() const {
	return tr("COM interfaces");
}

ItemEditor* ComponentEditorComInterfacesItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorComInterfacesItem::editor() const {
	return &editor_;
}

QString ComponentEditorComInterfacesItem::getTooltip() const {
	return tr("Contains the COM interfaces of a component");
}

void ComponentEditorComInterfacesItem::createChild( int index ) {
	QSharedPointer<ComponentEditorComInterfaceItem> interfaceItem(new ComponentEditorComInterfaceItem(
		interfaces_.at(index), model_, libHandler_, component_, this));
	childItems_.insert(index, interfaceItem);
}

void ComponentEditorComInterfacesItem::onEditorChanged() {
	
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
