/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditormemmapitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditormemmapitem.h"

#include <QFont>
#include <QApplication>

ComponentEditorMemMapItem::ComponentEditorMemMapItem(QSharedPointer<MemoryMap> memoryMap, 
													 ComponentEditorTreeModel* model,
													 LibraryInterface* libHandler,
													 QSharedPointer<Component> component,
													 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
memoryMap_(memoryMap) {

	Q_ASSERT(memoryMap_);
}

ComponentEditorMemMapItem::~ComponentEditorMemMapItem() {
}

QString ComponentEditorMemMapItem::text() const {
	return memoryMap_->getName();
}

bool ComponentEditorMemMapItem::isValid() const {
	return memoryMap_->isValid();
}

ItemEditor* ComponentEditorMemMapItem::editor() {
	return NULL;
}

const ItemEditor* ComponentEditorMemMapItem::editor() const {
	return NULL;
}

QFont ComponentEditorMemMapItem::getFont() const {
	return QApplication::font();
}

QString ComponentEditorMemMapItem::getTooltip() const {
	return tr("Contains the details of a single memory map that can be referenced"
		" by containing component's slave interfaces");
}
