/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditormemmapsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditormemmapsitem.h"

ComponentEditorMemMapsItem::ComponentEditorMemMapsItem( ComponentEditorTreeModel* model,
													   LibraryInterface* libHandler,
													   QSharedPointer<Component> component,
													   QWidget* widget,
													   ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent) {

}

ComponentEditorMemMapsItem::~ComponentEditorMemMapsItem() {
}

QString ComponentEditorMemMapsItem::text() const {
	return tr("Memory maps");
}

bool ComponentEditorMemMapsItem::isValid() const {
	return true;
}

ItemEditor* ComponentEditorMemMapsItem::editor() {
	return NULL;
}
