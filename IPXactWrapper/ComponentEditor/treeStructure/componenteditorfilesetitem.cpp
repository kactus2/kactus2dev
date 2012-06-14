/* 
 *  	Created on: 10.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfilesetitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorfilesetitem.h"
#include "componenteditorfileitem.h"

#include <QFont>
#include <QApplication>

ComponentEditorFileSetItem::ComponentEditorFileSetItem(QSharedPointer<FileSet> fileSet,
													   ComponentEditorTreeModel* model,
													   LibraryInterface* libHandler,
													   QSharedPointer<Component> component,
													   ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
fileSet_(fileSet),
files_(fileSet->getFiles()),
editor_(libHandler, component, fileSet, NULL) {

	Q_ASSERT(fileSet);

	foreach (QSharedPointer<File> file, files_) {
		QSharedPointer<ComponentEditorFileItem> fileItem(new ComponentEditorFileItem(
			file, model, libHandler, component, this));
		childItems_.append(fileItem);
	}

	editor_.hide();

	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childAdded(int)),
		this, SLOT(onAddChild(int)), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childRemoved(int)),
		this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childMoved(int, int)),
		this, SLOT(onMoveChild(int, int)), Qt::UniqueConnection);
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

const ItemEditor* ComponentEditorFileSetItem::editor() const {
	return &editor_;
}

QFont ComponentEditorFileSetItem::getFont() const {
	return QApplication::font();
}

QString ComponentEditorFileSetItem::getTooltip() const {
	return tr("Contains a list of files and their build commands");
}

void ComponentEditorFileSetItem::createChild( int index ) {

	QSharedPointer<ComponentEditorFileItem> fileItem(new ComponentEditorFileItem(
		files_.at(index), model_, libHandler_, component_, this));
	childItems_.insert(index, fileItem);
}

void ComponentEditorFileSetItem::onEditorChanged() {
	// call the base class implementation
	ComponentEditorItem::onEditorChanged();

	// also inform of child changes
	foreach (QSharedPointer<ComponentEditorItem> childItem, childItems_) {
		// tell the model that data has changed for the child
		emit contentChanged(childItem.data());

		// tell the child to update it's editor contents
		childItem->refreshEditor();
	}
}
