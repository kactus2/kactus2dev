//-----------------------------------------------------------------------------
// File: ComponentEditorSystemViewItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.7.2012
//
// Description:
// The item for a single software view in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "ComponentEditorSystemViewItem.h"

#include <QApplication>
#include <QFont>

ComponentEditorSystemViewItem::ComponentEditorSystemViewItem(
	QSharedPointer<SystemView> systemView,
	ComponentEditorTreeModel* model,
	LibraryInterface* libHandler,
	QSharedPointer<Component> component,
	ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
systemView_(systemView),
editor_(component, systemView, libHandler, NULL) {

	Q_ASSERT(systemView_);
	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
}

ComponentEditorSystemViewItem::~ComponentEditorSystemViewItem() {
}

QString ComponentEditorSystemViewItem::getTooltip() const {
	return tr("Contains the details of a software view");
}

QString ComponentEditorSystemViewItem::text() const {
	return systemView_->getName();
}

bool ComponentEditorSystemViewItem::isValid() const {
	// if system view is not valid
	if (!systemView_->isValid()) {
		return false;
	}

	// check that the reference is found
	if (!libHandler_->contains(systemView_->getHierarchyRef())) {
		return false;
	}

	// system view was valid and reference was found
	return true;
}

ItemEditor* ComponentEditorSystemViewItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorSystemViewItem::editor() const {
	return &editor_;
}

QFont ComponentEditorSystemViewItem::getFont() const {
	return QApplication::font();
}
