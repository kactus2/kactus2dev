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
apiInterfaces_(component->getApiInterfaces()) {

	foreach (QSharedPointer<ApiInterface> apiInterface, apiInterfaces_) {
		QSharedPointer<ComponentEditorAPIInterfaceItem> apiItem(
			new ComponentEditorAPIInterfaceItem(apiInterface, model, libHandler, component, this));
		childItems_.append(apiItem);
	}
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
	return NULL;
}
