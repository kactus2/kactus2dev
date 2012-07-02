/* 
 *  	Created on: 10.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditoraddrspaceitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditoraddrspaceitem.h"

#include <QFont>
#include <QApplication>

ComponentEditorAddrSpaceItem::ComponentEditorAddrSpaceItem(QSharedPointer<AddressSpace> addrSpace,
														   ComponentEditorTreeModel* model,
														   LibraryInterface* libHandler,
														   QSharedPointer<Component> component,
														   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
addrSpace_(addrSpace),
editor_(component, addrSpace) {

	editor_.hide();

	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
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
