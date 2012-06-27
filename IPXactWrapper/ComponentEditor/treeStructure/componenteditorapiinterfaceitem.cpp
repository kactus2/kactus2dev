/* 
 *  	Created on: 24.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorapiinterfaceitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorapiinterfaceitem.h"

#include <QApplication>

ComponentEditorAPIInterfaceItem::ComponentEditorAPIInterfaceItem(
	QSharedPointer<ApiInterface> APIInterface,
	ComponentEditorTreeModel* model,
	LibraryInterface* libHandler,
	QSharedPointer<Component> component,
	ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
apiIf_(APIInterface),
editor_(libHandler, component, APIInterface, NULL) {

	Q_ASSERT(apiIf_);

	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
}

ComponentEditorAPIInterfaceItem::~ComponentEditorAPIInterfaceItem() {
}

QString ComponentEditorAPIInterfaceItem::getTooltip() const {
	return tr("Contains the details of an API interface in the component");
}

QString ComponentEditorAPIInterfaceItem::text() const {
	return apiIf_->getName();
}

bool ComponentEditorAPIInterfaceItem::isValid() const {
	if (!apiIf_->isValid()) {
		return false;
	}
	return editor_.isValid();
}

ItemEditor* ComponentEditorAPIInterfaceItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorAPIInterfaceItem::editor() const {
	return &editor_;
}

QFont ComponentEditorAPIInterfaceItem::getFont() const {
	return QApplication::font();
}
