/* 
 *  	Created on: 28.8.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfielditem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorfielditem.h"
#include <IPXactWrapper/ComponentEditor/memoryMaps/fieldeditor.h>

#include <QApplication>

ComponentEditorFieldItem::ComponentEditorFieldItem(QSharedPointer<Field> field,
												   ComponentEditorTreeModel* model,
												   LibraryInterface* libHandler,
												   QSharedPointer<Component> component,
												   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
field_(field),
editor_(new FieldEditor(field, component)) {

	Q_ASSERT(field_);

	setObjectName(tr("ComponentEditorFieldItem"));

	editor_->hide();

	connect(editor_, SIGNAL(contentChanged()), 
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
}

ComponentEditorFieldItem::~ComponentEditorFieldItem() {
	if (editor_) {
		delete editor_;
		editor_ = NULL;
	}
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
	return editor_;
}

const ItemEditor* ComponentEditorFieldItem::editor() const {
	return editor_;
}

void ComponentEditorFieldItem::onEditorChanged() {
	// on field also the grand parent must be updated
	if (parent() && parent()->parent()) {
		emit contentChanged(parent()->parent());

		// on field also the grand grand parent must be updated
		if (parent()->parent()->parent()) {
			emit contentChanged(parent()->parent()->parent());

			// on field also the grand grand grand parent must be updated
			if (parent()->parent()->parent()->parent()) {
				emit contentChanged(parent()->parent()->parent()->parent());
			}
		}
	}

	// call the base class to update this and parent
	ComponentEditorItem::onEditorChanged();
}
