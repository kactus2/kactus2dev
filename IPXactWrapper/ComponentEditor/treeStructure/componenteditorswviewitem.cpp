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
	if (!swView_->isValid()) {
		return false;
	}
	return editor_.isValid();
}

ItemEditor* ComponentEditorSWViewItem::editor() {
	return &editor_;
}

QFont ComponentEditorSWViewItem::getFont() const {
	return QApplication::font();
}
