/* 
 *  	Created on: 10.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfilesetitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorfilesetitem.h"
#include "componenteditordeffilebuildersitem.h"
#include "componenteditorfilesitem.h"

#include <QFont>
#include <QApplication>

ComponentEditorFileSetItem::ComponentEditorFileSetItem(QSharedPointer<FileSet> fileSet,
													   ComponentEditorTreeModel* model,
													   LibraryInterface* libHandler,
													   QSharedPointer<Component> component,
													   ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
fileSet_(fileSet),
editor_(libHandler->getPath(*component->getVlnv()), component, fileSet.data()) {

	Q_ASSERT(fileSet);

	QSharedPointer<ComponentEditorDefFileBuildersItem> fileBuildersItem(new ComponentEditorDefFileBuildersItem(
		fileSet_->getDefaultFileBuilders(), model, libHandler, component, this));
	childItems_.append(fileBuildersItem);

	QSharedPointer<ComponentEditorFilesItem> filesItem(new ComponentEditorFilesItem(
		fileSet_->getFiles(), model, libHandler, component, this));
	childItems_.append(filesItem);

	editor_.hide();

	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
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

QFont ComponentEditorFileSetItem::getFont() const {
	return QApplication::font();
}

QString ComponentEditorFileSetItem::getTooltip() const {
	return tr("Contains a list of files and their build commands");
}
