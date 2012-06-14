/* 
 *  	Created on: 24.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorswviewsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorswviewsitem.h"
#include "componenteditorswviewitem.h"

ComponentEditorSWViewsItem::ComponentEditorSWViewsItem(
	ComponentEditorTreeModel* model, 
	LibraryInterface* libHandler,
	QSharedPointer<Component> component,
	ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
swViews_(component->getSWViews()) {

	foreach (QSharedPointer<SWView> swView, swViews_) {

		QSharedPointer<ComponentEditorSWViewItem> swViewItem(
			new ComponentEditorSWViewItem(swView, model, libHandler, component, this)); 
		childItems_.append(swViewItem);
	}
}

ComponentEditorSWViewsItem::~ComponentEditorSWViewsItem() {
}

QString ComponentEditorSWViewsItem::getTooltip() const {
	return tr("Contains the software views of the component");
}

QString ComponentEditorSWViewsItem::text() const {
	return tr("Software views");
}

ItemEditor* ComponentEditorSWViewsItem::editor() {
	return NULL;
}

const ItemEditor* ComponentEditorSWViewsItem::editor() const {
	return NULL;
}
