/* 
 *  	Created on: 24.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorapiinterfacesitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorapiinterfacesitem.h"
#include "componenteditorapiinterfaceitem.h"

ComponentEditorAPIInterfacesItem::ComponentEditorAPIInterfacesItem(
	ComponentEditorTreeModel* model,
	LibraryInterface* libHandler,
	QSharedPointer<Component> component,
	ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
apiInterfaces_(component->getApiInterfaces()),
editor_(component) {

	foreach (QSharedPointer<ApiInterface> apiInterface, apiInterfaces_) {
		QSharedPointer<ComponentEditorAPIInterfaceItem> apiItem(
			new ComponentEditorAPIInterfaceItem(apiInterface, model, libHandler, component, this));
		childItems_.append(apiItem);
	}
	editor_.hide();

	connect(&editor_, SIGNAL(contentChanged()), 
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childAdded(int)),
		this, SLOT(onAddChild(int)), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childRemoved(int)),
		this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
}

ComponentEditorAPIInterfacesItem::~ComponentEditorAPIInterfacesItem() {
}

QString ComponentEditorAPIInterfacesItem::getTooltip() const {
	return tr("Contains the API interfaces of the component");
}

QString ComponentEditorAPIInterfacesItem::text() const {
	return tr("API interfaces");
}

ItemEditor* ComponentEditorAPIInterfacesItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorAPIInterfacesItem::editor() const {
	return &editor_;
}

void ComponentEditorAPIInterfacesItem::createChild( int index ) {
	QSharedPointer<ComponentEditorAPIInterfaceItem> apiItem(
		new ComponentEditorAPIInterfaceItem(apiInterfaces_.at(index), model_, libHandler_, component_, this));
	childItems_.insert(index, apiItem);
}
