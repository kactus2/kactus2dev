/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorotherclocksitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorotherclocksitem.h"

ComponentEditorOtherClocksItem::ComponentEditorOtherClocksItem(ComponentEditorTreeModel* model, 
															   LibraryInterface* libHandler,
															   QSharedPointer<Component> component, 
															   QWidget* widget,
															   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent) {

}

ComponentEditorOtherClocksItem::~ComponentEditorOtherClocksItem() {
}

QString ComponentEditorOtherClocksItem::text() const {
	return tr("Other clock drivers");
}

bool ComponentEditorOtherClocksItem::isValid() const {
	return true;
}

ItemEditor* ComponentEditorOtherClocksItem::editor() {
	return NULL;
}
