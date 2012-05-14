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
												   QWidget* widget, 
												   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
views_(component->getViews()) {

	foreach (QSharedPointer<View> view, views_) {

		ComponentEditorViewItem* viewItem(new ComponentEditorViewItem(
			view, model, libHandler, component, widget, this));
		childItems_.append(viewItem);
	}
}

ComponentEditorViewsItem::~ComponentEditorViewsItem() {
}

QString ComponentEditorViewsItem::text() const {
	return tr("Views");
}

bool ComponentEditorViewsItem::isValid() const {
	foreach (QSharedPointer<View> view, views_) {
		if (!view->isValid(component_->getFileSetNames())) {
			return false;
		}
	}
	return true;
}

ItemEditor* ComponentEditorViewsItem::editor() {
	return NULL;
}
