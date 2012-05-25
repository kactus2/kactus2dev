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
														 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
fileSets_(component->getFileSets()),
editor_(component) {

	foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
		QSharedPointer<ComponentEditorFileSetItem> fileSetItem(
			new ComponentEditorFileSetItem(fileSet, model, libHandler, component, this));
		childItems_.append(fileSetItem);
	}

	connect(&editor_, SIGNAL(contentChanged()), 
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
}

ComponentEditorFileSetsItem::~ComponentEditorFileSetsItem() {
}

QString ComponentEditorFileSetsItem::text() const { 
	return tr("File sets");
}

ItemEditor* ComponentEditorFileSetsItem::editor() {
	return &editor_;
}

QString ComponentEditorFileSetsItem::getTooltip() const {
	return tr("Contains the file sets of the component");
}
