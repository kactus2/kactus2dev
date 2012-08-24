/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorregisteritem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorregisteritem.h"

#include <QFont>
#include <QApplication>

ComponentEditorRegisterItem::ComponentEditorRegisterItem(QSharedPointer<Register> reg,
														 ComponentEditorTreeModel* model,
														 LibraryInterface* libHandler, 
														 QSharedPointer<Component> component,
														 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
reg_(reg),
editor_(NULL) {

}

ComponentEditorRegisterItem::~ComponentEditorRegisterItem() {
// 	if (editor_) {
// 		delete editor_;
// 		editor_ = NULL;
// 	}
}

QString ComponentEditorRegisterItem::getTooltip() const {
	return tr("Contains details of a single register within an address block.");
}

QString ComponentEditorRegisterItem::text() const {
	return reg_->getName();
}

bool ComponentEditorRegisterItem::isValid() const {
	return reg_->isValid();
}

ItemEditor* ComponentEditorRegisterItem::editor() {
	return NULL;
}

const ItemEditor* ComponentEditorRegisterItem::editor() const {
	return NULL;
}

QFont ComponentEditorRegisterItem::getFont() const {
	return QApplication::font();
}
