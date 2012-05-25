/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfileitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorfileitem.h"

#include <QFont>
#include <QApplication>

ComponentEditorFileItem::ComponentEditorFileItem(QSharedPointer<File> file,
												 ComponentEditorTreeModel* model,
												 LibraryInterface* libHandler,
												 QSharedPointer<Component> component,
												 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
file_(file),
editor_(libHandler->getPath(*component->getVlnv()), component, file) {
	editor_.hide();
	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
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

QFont ComponentEditorFileItem::getFont() const {
	return QApplication::font();
}

QString ComponentEditorFileItem::getTooltip() const {
	return tr("Specifies a single file associated with the file set");
}
