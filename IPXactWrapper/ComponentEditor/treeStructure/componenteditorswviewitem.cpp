/* 
 *  	Created on: 24.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorswviewitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorswviewitem.h"

#include <QApplication>
#include <QFont>

ComponentEditorSWViewItem::ComponentEditorSWViewItem(
	QSharedPointer<SWView> swView,
	ComponentEditorTreeModel* model,
	LibraryInterface* libHandler,
	QSharedPointer<Component> component,
	ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
swView_(swView),
editor_(component, swView, libHandler, NULL) {

	Q_ASSERT(swView_);
	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(helpUrlRequested(QString const&)),
		this, SIGNAL(helpUrlRequested(QString const&)));
}

ComponentEditorSWViewItem::~ComponentEditorSWViewItem() {
}

QString ComponentEditorSWViewItem::getTooltip() const {
	return tr("Contains the details of a software view");
}

QString ComponentEditorSWViewItem::text() const {
	return swView_->getName();
}

bool ComponentEditorSWViewItem::isValid() const {
	// if sw view is not valid
	if (!swView_->isValid()) {
		return false;
	}

	// check that the reference is found
	if (!libHandler_->contains(swView_->getHierarchyRef())) {
		return false;
	}

	// sw view was valid and reference was found
	return true;
}

ItemEditor* ComponentEditorSWViewItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorSWViewItem::editor() const {
	return &editor_;
}

QFont ComponentEditorSWViewItem::getFont() const {
	return QApplication::font();
}
