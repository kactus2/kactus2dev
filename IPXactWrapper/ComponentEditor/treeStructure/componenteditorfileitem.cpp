/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfileitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorfileitem.h"

#include <models/generaldeclarations.h>

#include <QFont>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>

ComponentEditorFileItem::ComponentEditorFileItem(QSharedPointer<File> file,
												 ComponentEditorTreeModel* model,
												 LibraryInterface* libHandler,
												 QSharedPointer<Component> component,
												 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
file_(file),
editor_(libHandler, component, file) {
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

const ItemEditor* ComponentEditorFileItem::editor() const {
	return &editor_;
}

QFont ComponentEditorFileItem::getFont() const {
	return QApplication::font();
}

QString ComponentEditorFileItem::getTooltip() const {
	return tr("Specifies a single file associated with the file set");
}

bool ComponentEditorFileItem::canBeOpened() const {
	return true;
}

void ComponentEditorFileItem::openItem() {

	// create the necessary paths
	const QString relPath = file_->getName();
	const QString xmlPath = libHandler_->getPath(*component_->getVlnv());
	const QString absolutePath = General::getAbsolutePath(xmlPath, relPath);

	// open the file
	QDesktopServices::openUrl(QUrl::fromLocalFile(absolutePath));
}
