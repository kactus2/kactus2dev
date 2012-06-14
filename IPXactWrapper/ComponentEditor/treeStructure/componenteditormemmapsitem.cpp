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
													   ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
memoryMaps_(component->getMemoryMaps()) {

	foreach (QSharedPointer<MemoryMap> memoryMap, memoryMaps_) {
		QSharedPointer<ComponentEditorMemMapItem> memoryMapItem(new ComponentEditorMemMapItem(
			memoryMap, model, libHandler, component, this));
		childItems_.append(memoryMapItem);
	}
}

ComponentEditorMemMapsItem::~ComponentEditorMemMapsItem() {
}

QString ComponentEditorMemMapsItem::text() const {
	return tr("Memory maps");
}

ItemEditor* ComponentEditorMemMapsItem::editor() {
	return NULL;
}

const ItemEditor* ComponentEditorMemMapsItem::editor() const {
	return NULL;
}

QString ComponentEditorMemMapsItem::getTooltip() const {
	return tr("Contains the memory maps of the component");
}
