/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfilesitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorfilesitem.h"
#include "componenteditorfileitem.h"

#include <QFont>
#include <QApplication>

ComponentEditorFilesItem::ComponentEditorFilesItem(QList<QSharedPointer<File> >& files,
												   ComponentEditorTreeModel* model,
												   LibraryInterface* libHandler, 
												   QSharedPointer<Component> component,
												   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
files_(files) {

	foreach (QSharedPointer<File> file, files_) {
		QSharedPointer<ComponentEditorFileItem> fileItem(new ComponentEditorFileItem(
			file, model, libHandler, component, this));
		childItems_.append(fileItem);
	}
}

ComponentEditorFilesItem::~ComponentEditorFilesItem() {
}

QString ComponentEditorFilesItem::text() const {
	return tr("Files");
}

ItemEditor* ComponentEditorFilesItem::editor() {
	return NULL;
}

QFont ComponentEditorFilesItem::getFont() const {
	return QApplication::font();
}

QString ComponentEditorFilesItem::getTooltip() const {
	return tr("Contains the files contained in the file set");
}
