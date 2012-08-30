/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorregisteritem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorregisteritem.h"
#include <IPXactWrapper/ComponentEditor/memoryMaps/registereditor.h>
#include "componenteditorfielditem.h"

#include <QFont>
#include <QApplication>

ComponentEditorRegisterItem::ComponentEditorRegisterItem(QSharedPointer<Register> reg,
														 ComponentEditorTreeModel* model,
														 LibraryInterface* libHandler, 
														 QSharedPointer<Component> component,
														 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
reg_(reg),
fields_(reg->getFields()),
editor_(new RegisterEditor(reg, component)) {

	setObjectName(tr("ComponentEditorRegisterItem"));

	foreach(QSharedPointer<Field> field, fields_) {
		if (field) {
			QSharedPointer<ComponentEditorFieldItem> fieldItem(new ComponentEditorFieldItem(
				field, model, libHandler, component, this));
			childItems_.append(fieldItem);
		}
	}

	editor_->hide();

	connect(editor_, SIGNAL(contentChanged()), 
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(editor_, SIGNAL(childAdded(int)),
		this, SLOT(onAddChild(int)), Qt::UniqueConnection);
	connect(editor_, SIGNAL(childRemoved(int)),
		this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);

	Q_ASSERT(reg_);
}

ComponentEditorRegisterItem::~ComponentEditorRegisterItem() {
	if (editor_) {
		delete editor_;
		editor_ = NULL;
	}
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
	return editor_;
}

const ItemEditor* ComponentEditorRegisterItem::editor() const {
	return editor_;
}

QFont ComponentEditorRegisterItem::getFont() const {
	return QApplication::font();
}

void ComponentEditorRegisterItem::createChild( int index ) {
	QSharedPointer<ComponentEditorFieldItem> fieldItem(new ComponentEditorFieldItem(
		fields_.at(index), model_, libHandler_, component_, this));
	childItems_.insert(index, fieldItem);
}

void ComponentEditorRegisterItem::onEditorChanged() {

	// on register also the grand parent must be updated
	if (parent() && parent()->parent()) {
		emit contentChanged(parent()->parent());

		// on register also the grand grand parent must be updated
		if (parent()->parent()->parent()) {
			emit contentChanged(parent()->parent()->parent());
		}
	}

	// call the base class to update this and parent
	ComponentEditorItem::onEditorChanged();
}
