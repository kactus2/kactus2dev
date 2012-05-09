/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorsoftmapsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorsoftmapsitem.h"

ComponentEditorSoftMapsItem::ComponentEditorSoftMapsItem(ComponentEditorTreeModel* model,
														 LibraryInterface* libHandler, 
														 QSharedPointer<Component> component,
														 QWidget* widget,
														 ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent) {

}

ComponentEditorSoftMapsItem::~ComponentEditorSoftMapsItem() {
}

QString ComponentEditorSoftMapsItem::text() const {
	return tr("Software mappings");
}

bool ComponentEditorSoftMapsItem::isValid() const {
	return true;
}

ItemEditor* ComponentEditorSoftMapsItem::editor() {
	return NULL;
}
