/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfilesitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorfilesitem.h"
#include "componenteditorfileitem.h"

ComponentEditorFilesItem::ComponentEditorFilesItem(QList<QSharedPointer<File> >& files,
												   ComponentEditorTreeModel* model,
												   LibraryInterface* libHandler, 
												   QSharedPointer<Component> component,
												   QWidget* widget,
												   ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
files_(files) {

	foreach (QSharedPointer<File> file, files_) {
		ComponentEditorFileItem* fileItem = new ComponentEditorFileItem(
			file, model, libHandler, component, widget, this);
		childItems_.append(fileItem);
	}
}

ComponentEditorFilesItem::~ComponentEditorFilesItem() {
}

QString ComponentEditorFilesItem::text() const {
	return tr("Files");
}

bool ComponentEditorFilesItem::isValid() const {
	foreach (QSharedPointer<File> file, files_) {
		if (!file->isValid(true)) {
			return false;
		}
	}
	return true;
}

ItemEditor* ComponentEditorFilesItem::editor() {
	return NULL;
}
