/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditormemmapsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditormemmapsitem.h"
#include "componenteditormemmapitem.h"

ComponentEditorMemMapsItem::ComponentEditorMemMapsItem( ComponentEditorTreeModel* model,
													   LibraryInterface* libHandler,
													   QSharedPointer<Component> component,
													   QWidget* widget,
													   ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
memoryMaps_(component->getMemoryMaps()) {

	foreach (QSharedPointer<MemoryMap> memoryMap, memoryMaps_) {
		ComponentEditorMemMapItem* memoryMapItem = new ComponentEditorMemMapItem(
			memoryMap, model, libHandler, component, widget, this);
		childItems_.append(memoryMapItem);
	}
}

ComponentEditorMemMapsItem::~ComponentEditorMemMapsItem() {
}

QString ComponentEditorMemMapsItem::text() const {
	return tr("Memory maps");
}

bool ComponentEditorMemMapsItem::isValid() const {
	foreach (QSharedPointer<MemoryMap> memoryMap, memoryMaps_) {
		if (!memoryMap->isValid()) {
			return false;
		}
	}
	return true;
}

ItemEditor* ComponentEditorMemMapsItem::editor() {
	return NULL;
}
