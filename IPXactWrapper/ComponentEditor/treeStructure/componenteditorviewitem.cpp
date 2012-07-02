/* 
 *  	Created on: 14.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorviewitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorviewitem.h"

#include <QFont>
#include <QApplication>

ComponentEditorViewItem::ComponentEditorViewItem(QSharedPointer<View> view,
												 ComponentEditorTreeModel* model,
												 LibraryInterface* libHandler,
												 QSharedPointer<Component> component,
												 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
view_(view),
editor_(component, view, libHandler) {
	
	Q_ASSERT(view_);

	setObjectName(tr("ComponentEditorViewItem: %1").arg(view->getName()));
	
	editor_.hide();

	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
}

ComponentEditorViewItem::~ComponentEditorViewItem() {
}

QString ComponentEditorViewItem::text() const {
	return view_->getName();
}

bool ComponentEditorViewItem::isValid() const {
	// check that view is valid
	if (!view_->isValid(component_->getFileSetNames())) {
		return false;
	}
	// check that the reference can be found
	if (view_->isHierarchical() && !libHandler_->contains(view_->getHierarchyRef())) {
		return false;
	}

	// view was valid and the reference was found
	return true;
}

ItemEditor* ComponentEditorViewItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorViewItem::editor() const {
	return &editor_;
}

QFont ComponentEditorViewItem::getFont() const {
	return QApplication::font();
}

QString ComponentEditorViewItem::getTooltip() const {
	return tr("Specifies a representation level of the component");
}
