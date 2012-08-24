/* 
 *  	Created on: 23.8.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditoraddrblockitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditoraddrblockitem.h"
#include <IPXactWrapper/ComponentEditor/memoryMaps/addressblockeditor.h>

#include <QApplication>

ComponentEditorAddrBlockItem::ComponentEditorAddrBlockItem(QSharedPointer<AddressBlock> addrBlock,
														   ComponentEditorTreeModel* model,
														   LibraryInterface* libHandler,
														   QSharedPointer<Component> component,
														   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
addrBlock_(addrBlock),
editor_(new AddressBlockEditor(addrBlock, component)) {

	setObjectName(tr("ComponentEditorAddrBlockItem"));

// 	foreach (QSharedPointer<MemoryMapItem> memItem, items_) {
// 
// 		// if the item is for address block then create child for it
// 		QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
// 		if (addrBlock) {
// 			QSharedPointer<ComponentEditorAddrBlockItem> addrBlockItem(
// 				new ComponentEditorAddrBlockItem(addrBlock, model, libHandler, component, this));
// 			childItems_.append(addrBlockItem);
// 		}
// 	}

	editor_->hide();

	connect(editor_, SIGNAL(contentChanged()), 
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(editor_, SIGNAL(childAdded(int)),
		this, SLOT(onAddChild(int)), Qt::UniqueConnection);
	connect(editor_, SIGNAL(childRemoved(int)),
		this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);

	Q_ASSERT(addrBlock_);
}

ComponentEditorAddrBlockItem::~ComponentEditorAddrBlockItem() {
	if (editor_) {
		delete editor_;
		editor_ = NULL;
	}
}

QString ComponentEditorAddrBlockItem::getTooltip() const {
	return tr("Contains details of a single address block within a memory map.");
}

QString ComponentEditorAddrBlockItem::text() const {
	return addrBlock_->getName();
}

bool ComponentEditorAddrBlockItem::isValid() const {
	return addrBlock_->isValid();
}

ItemEditor* ComponentEditorAddrBlockItem::editor() {
	return editor_;
}

const ItemEditor* ComponentEditorAddrBlockItem::editor() const {
	return editor_;
}

QFont ComponentEditorAddrBlockItem::getFont() const {
	return QApplication::font();
}
