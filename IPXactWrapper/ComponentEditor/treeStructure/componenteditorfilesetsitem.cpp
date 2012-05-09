/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfilesetsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorfilesetsitem.h"

ComponentEditorFileSetsItem::ComponentEditorFileSetsItem( ComponentEditorTreeModel* model,
														 LibraryInterface* libHandler,
														 QSharedPointer<Component> component, 
														 QWidget* widget, 
														 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent) {

}

ComponentEditorFileSetsItem::~ComponentEditorFileSetsItem() {
}

QString ComponentEditorFileSetsItem::text() const { 
	return tr("File sets");
}

bool ComponentEditorFileSetsItem::isValid() const {
	return true;
}

ItemEditor* ComponentEditorFileSetsItem::editor() {
	return NULL;
}
