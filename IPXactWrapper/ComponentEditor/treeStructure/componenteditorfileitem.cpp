/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfileitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorfileitem.h"

ComponentEditorFileItem::ComponentEditorFileItem(QSharedPointer<File> file,
												 ComponentEditorTreeModel* model,
												 LibraryInterface* libHandler,
												 QSharedPointer<Component> component,
												 QWidget* widget,
												 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
file_(file),
editor_(libHandler->getPath(*component->getVlnv()), component, file, widget) {

}

ComponentEditorFileItem::~ComponentEditorFileItem() {
}

QString ComponentEditorFileItem::text() const {
	return file_->getName();
}

bool ComponentEditorFileItem::isValid() const {
	if (!file_->isValid(true)) {
		return false;
	}
	return editor_.isValid();
}

ItemEditor* ComponentEditorFileItem::editor() {
	return &editor_;
}
