/* 
 *  	Created on: 14.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorviewitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorviewitem.h"

ComponentEditorViewItem::ComponentEditorViewItem(QSharedPointer<View> view,
												 ComponentEditorTreeModel* model,
												 LibraryInterface* libHandler,
												 QSharedPointer<Component> component,
												 QWidget* widget,
												 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
view_(view),
editor_(component, view, libHandler, widget) {

	Q_ASSERT(view_);
}

ComponentEditorViewItem::~ComponentEditorViewItem() {
}

QString ComponentEditorViewItem::text() const {
	return view_->getName();
}

bool ComponentEditorViewItem::isValid() const {
	if (!view_->isValid(component_->getFileSetNames())) {
		return false;
	}
	return editor_.isValid();
}

ItemEditor* ComponentEditorViewItem::editor() {
	return &editor_;
}
