/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditormemmapitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditormemmapitem.h"
#include <IPXactWrapper/ComponentEditor/memoryMaps/memorymapeditor.h>
#include "componenteditoraddrblockitem.h"
#include <models/memorymapitem.h>
#include <models/addressblock.h>

#include <QFont>
#include <QApplication>

ComponentEditorMemMapItem::ComponentEditorMemMapItem(QSharedPointer<MemoryMap> memoryMap, 
													 ComponentEditorTreeModel* model,
													 LibraryInterface* libHandler,
													 QSharedPointer<Component> component,
													 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
memoryMap_(memoryMap),
items_(memoryMap->getItems()),
editor_(new MemoryMapEditor(component, memoryMap)) {

	setObjectName(tr("ComponentEditorMemMapItem"));

	foreach (QSharedPointer<MemoryMapItem> memItem, items_) {
		
		// if the item is for address block then create child for it
		QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
		if (addrBlock) {
			QSharedPointer<ComponentEditorAddrBlockItem> addrBlockItem(
				new ComponentEditorAddrBlockItem(addrBlock, model, libHandler, component, this));
			childItems_.append(addrBlockItem);
		}
	}

	editor_->hide();

	connect(editor_, SIGNAL(contentChanged()), 
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(editor_, SIGNAL(childAdded(int)),
		this, SLOT(onAddChild(int)), Qt::UniqueConnection);
	connect(editor_, SIGNAL(childRemoved(int)),
		this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);

	Q_ASSERT(memoryMap_);
}

ComponentEditorMemMapItem::~ComponentEditorMemMapItem() {
	if (editor_) {
		delete editor_;
		editor_ = NULL;
	}
}

QString ComponentEditorMemMapItem::text() const {
	return memoryMap_->getName();
}

bool ComponentEditorMemMapItem::isValid() const {
	return memoryMap_->isValid();
}

ItemEditor* ComponentEditorMemMapItem::editor() {
	return editor_;
}

const ItemEditor* ComponentEditorMemMapItem::editor() const {
	return editor_;
}

QFont ComponentEditorMemMapItem::getFont() const {
	return QApplication::font();
}

QString ComponentEditorMemMapItem::getTooltip() const {
	return tr("Contains the details of a single memory map that can be referenced"
		" by containing component's slave interfaces");
}

void ComponentEditorMemMapItem::createChild( int index ) {

	QSharedPointer<MemoryMapItem> memItem = items_[index];
	QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
	if (addrBlock) {
		QSharedPointer<ComponentEditorAddrBlockItem> addrBlockItem(
			new ComponentEditorAddrBlockItem(addrBlock, model_, libHandler_, component_, this));
		childItems_.insert(index, addrBlockItem);
	}
}
