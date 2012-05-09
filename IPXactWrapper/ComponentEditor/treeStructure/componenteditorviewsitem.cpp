/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorviewsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorviewsitem.h"

ComponentEditorViewsItem::ComponentEditorViewsItem(ComponentEditorTreeModel* model,
												   LibraryInterface* libHandler,
												   QSharedPointer<Component> component,
												   QWidget* widget, 
												   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent) {

}

ComponentEditorViewsItem::~ComponentEditorViewsItem() {
}

QString ComponentEditorViewsItem::text() const {
	return tr("Views");
}

bool ComponentEditorViewsItem::isValid() const {
	return true;
}

ItemEditor* ComponentEditorViewsItem::editor() {
	return NULL;
}
