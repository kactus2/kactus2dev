/* 
 *  	Created on: 10.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfilesetitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorfilesetitem.h"
#include "componenteditordeffilebuildersitem.h"
#include "componenteditorfilesitem.h"

ComponentEditorFileSetItem::ComponentEditorFileSetItem(QSharedPointer<FileSet> fileSet,
													   ComponentEditorTreeModel* model,
													   LibraryInterface* libHandler,
													   QSharedPointer<Component> component,
													   QWidget* widget,
													   ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
fileSet_(fileSet),
editor_(libHandler->getPath(*component->getVlnv()), component, fileSet.data(), widget) {

	Q_ASSERT(fileSet);

	ComponentEditorDefFileBuildersItem* fileBuildersItem = new ComponentEditorDefFileBuildersItem(
		fileSet_->getDefaultFileBuilders(), model, libHandler, component, widget, this);
	childItems_.append(fileBuildersItem);

	ComponentEditorFilesItem* filesItem = new ComponentEditorFilesItem(
		fileSet_->getFiles(), model, libHandler, component, widget, this);
	childItems_.append(filesItem);
}

ComponentEditorFileSetItem::~ComponentEditorFileSetItem() {
}

QString ComponentEditorFileSetItem::text() const {
	return fileSet_->getName();
}

bool ComponentEditorFileSetItem::isValid() const {
	if (!fileSet_->isValid(true)) {
		return false;
	}
	return editor_.isValid();
}

ItemEditor* ComponentEditorFileSetItem::editor() {
	return &editor_;
}
