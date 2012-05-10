/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfilesetsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorfilesetsitem.h"
#include "componenteditorfilesetitem.h"

ComponentEditorFileSetsItem::ComponentEditorFileSetsItem( ComponentEditorTreeModel* model,
														 LibraryInterface* libHandler,
														 QSharedPointer<Component> component, 
														 QWidget* widget, 
														 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
fileSets_(component->getFileSets()) {

	foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
		ComponentEditorFileSetItem* fileSetItem = 
			new ComponentEditorFileSetItem(fileSet, model, libHandler, component, widget, this);
		childItems_.append(fileSetItem);
	}
}

ComponentEditorFileSetsItem::~ComponentEditorFileSetsItem() {
}

QString ComponentEditorFileSetsItem::text() const { 
	return tr("File sets");
}

bool ComponentEditorFileSetsItem::isValid() const {
	foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
		if (!fileSet->isValid(true)) {
			return false;
		}
	}
	// if all file sets were valid
	return true;
}

ItemEditor* ComponentEditorFileSetsItem::editor() {
	return NULL;
}
