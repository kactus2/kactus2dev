/* 
 *  	Created on: 10.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditoraddrspaceitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditoraddrspaceitem.h"
#include <models/addressblock.h>
#include "componenteditoraddrblockitem.h"

#include <QFont>
#include <QApplication>

ComponentEditorAddrSpaceItem::ComponentEditorAddrSpaceItem(QSharedPointer<AddressSpace> addrSpace,
														   ComponentEditorTreeModel* model,
														   LibraryInterface* libHandler,
														   QSharedPointer<Component> component,
														   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
addrSpace_(addrSpace),
items_(addrSpace->getLocalMemoryMap()->getItems()),
editor_(component, addrSpace) {

	setObjectName(tr("ComponentEditorAddrSpaceItem"));

	editor_.hide();

	foreach (QSharedPointer<MemoryMapItem> memItem, items_) {

		// if the item is for address block then create child for it
		QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
		if (addrBlock) {
			QSharedPointer<ComponentEditorAddrBlockItem> addrBlockItem(
				new ComponentEditorAddrBlockItem(addrBlock, model, libHandler, component, this));
			childItems_.append(addrBlockItem);
		}
	}

	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childAdded(int)),
		this, SLOT(onAddChild(int)), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childRemoved(int)),
		this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(helpUrlRequested(QString const&)),
		this, SIGNAL(helpUrlRequested(QString const&)));
}

ComponentEditorAddrSpaceItem::~ComponentEditorAddrSpaceItem() {
}

QString ComponentEditorAddrSpaceItem::text() const {
	return addrSpace_->getName();
}

bool ComponentEditorAddrSpaceItem::isValid() const {
	return addrSpace_->isValid();
}

ItemEditor* ComponentEditorAddrSpaceItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorAddrSpaceItem::editor() const {
	return &editor_;
}

QFont ComponentEditorAddrSpaceItem::getFont() const {
	return QApplication::font();
}

QString ComponentEditorAddrSpaceItem::getTooltip() const {
	return tr("Address space defines a logical address space seen by a master"
		" bus interface");
}

void ComponentEditorAddrSpaceItem::createChild( int index ) {
	QSharedPointer<MemoryMapItem> memItem = items_[index];
	QSharedPointer<AddressBlock> addrBlock = memItem.dynamicCast<AddressBlock>();
	if (addrBlock) {
		QSharedPointer<ComponentEditorAddrBlockItem> addrBlockItem(
			new ComponentEditorAddrBlockItem(addrBlock, model_, libHandler_, component_, this));
		childItems_.insert(index, addrBlockItem);
	}
}
