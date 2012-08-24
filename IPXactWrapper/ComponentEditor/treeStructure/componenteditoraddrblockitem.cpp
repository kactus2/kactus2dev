/* 
 *  	Created on: 23.8.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditoraddrblockitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditoraddrblockitem.h"
#include <IPXactWrapper/ComponentEditor/memoryMaps/addressblockeditor.h>
#include <models/register.h>
#include "componenteditorregisteritem.h"

#include <QApplication>

ComponentEditorAddrBlockItem::ComponentEditorAddrBlockItem(QSharedPointer<AddressBlock> addrBlock,
														   ComponentEditorTreeModel* model,
														   LibraryInterface* libHandler,
														   QSharedPointer<Component> component,
														   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
addrBlock_(addrBlock),
regItems_(addrBlock->getRegisterData()),
editor_(new AddressBlockEditor(addrBlock, component)) {

	setObjectName(tr("ComponentEditorAddrBlockItem"));

	foreach (QSharedPointer<RegisterModel> regModel, regItems_) {
		QSharedPointer<Register> reg = regModel.dynamicCast<Register>();
		
		// if the item was a register 
		if (reg) {
			QSharedPointer<ComponentEditorRegisterItem> regItem(new ComponentEditorRegisterItem(
				reg, model, libHandler, component, this));
			childItems_.append(regItem);
		}
	}

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

void ComponentEditorAddrBlockItem::createChild( int index ) {
	QSharedPointer<RegisterModel> regmodel = regItems_[index];
	QSharedPointer<Register> reg = regmodel.dynamicCast<Register>();
	if (reg) {
		QSharedPointer<ComponentEditorRegisterItem> regItem(
			new ComponentEditorRegisterItem(reg, model_, libHandler_, component_, this));
		childItems_.insert(index, regItem);
	}
}
