/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorcpusitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorcpusitem.h"

ComponentEditorCpusItem::ComponentEditorCpusItem(ComponentEditorTreeModel* model, 
												 LibraryInterface* libHandler,
												 QSharedPointer<Component> component, 
												 QWidget* widget, 
												 ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent) {

}

ComponentEditorCpusItem::~ComponentEditorCpusItem() {
}

QString ComponentEditorCpusItem::text() const {
	return tr("Cpus");
}

bool ComponentEditorCpusItem::isValid() const {
	return true;
}

ItemEditor* ComponentEditorCpusItem::editor() {
	return NULL;
}
