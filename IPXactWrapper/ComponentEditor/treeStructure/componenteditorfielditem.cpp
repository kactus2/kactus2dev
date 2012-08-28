/* 
 *  	Created on: 28.8.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfielditem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorfielditem.h"

#include <QApplication>

ComponentEditorFieldItem::ComponentEditorFieldItem(QSharedPointer<Field> field,
												   ComponentEditorTreeModel* model,
												   LibraryInterface* libHandler,
												   QSharedPointer<Component> component,
												   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
field_(field),
editor_(NULL) {

	Q_ASSERT(field_);
}

ComponentEditorFieldItem::~ComponentEditorFieldItem() {
// 	if (editor_) {
// 		delete editor_;
// 		editor_ = NULL;
// 	}
}

QFont ComponentEditorFieldItem::getFont() const {
	return QApplication::font();
}

QString ComponentEditorFieldItem::getTooltip() const {
	return tr("Contains details of a single field within a register.");
}

QString ComponentEditorFieldItem::text() const {
	return field_->getName();
}

bool ComponentEditorFieldItem::isValid() const {
	return field_->isValid();
}

ItemEditor* ComponentEditorFieldItem::editor() {
	return NULL;
}

const ItemEditor* ComponentEditorFieldItem::editor() const {
	return NULL;
}
