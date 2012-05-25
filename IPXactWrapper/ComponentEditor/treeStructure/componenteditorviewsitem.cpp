/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorviewsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorviewsitem.h"
#include "componenteditorviewitem.h"

ComponentEditorViewsItem::ComponentEditorViewsItem(ComponentEditorTreeModel* model,
												   LibraryInterface* libHandler,
												   QSharedPointer<Component> component,
												   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
views_(component->getViews()) {

	setObjectName(tr("ComponentEditorViewsItem"));

	foreach (QSharedPointer<View> view, views_) {

		QSharedPointer<ComponentEditorViewItem> viewItem(new ComponentEditorViewItem(
			view, model, libHandler, component, this));
		childItems_.append(viewItem);
	}
}

ComponentEditorViewsItem::~ComponentEditorViewsItem() {
}

QString ComponentEditorViewsItem::text() const {
	return tr("Views");
}

ItemEditor* ComponentEditorViewsItem::editor() {
	return NULL;
}

QString ComponentEditorViewsItem::getTooltip() const {
	return tr("Contains the views of the component");
}
