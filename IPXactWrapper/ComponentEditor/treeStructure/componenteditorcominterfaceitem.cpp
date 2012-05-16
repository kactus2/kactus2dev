/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorcominterfaceitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorcominterfaceitem.h"

ComponentEditorComInterfaceItem::ComponentEditorComInterfaceItem(QSharedPointer<ComInterface> comInterface,
																 ComponentEditorTreeModel* model,
																 LibraryInterface* libHandler,
																 QSharedPointer<Component> component,
																 QWidget* widget,
																 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
interface_(comInterface),
editor_(libHandler, component, comInterface, widget) {

}

ComponentEditorComInterfaceItem::~ComponentEditorComInterfaceItem() {
}

QString ComponentEditorComInterfaceItem::text() const {
	return interface_->getName();
}

bool ComponentEditorComInterfaceItem::isValid() const {
	if (!interface_->isValid()) {
		return false;
	}
	return editor_.isValid();
}

ItemEditor* ComponentEditorComInterfaceItem::editor() {
	return &editor_;
}
