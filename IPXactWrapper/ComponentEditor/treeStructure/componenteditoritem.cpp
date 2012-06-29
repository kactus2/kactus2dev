/* 
 *  	Created on: 7.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditoritem.cpp
 *		Project: Kactus 2
 */

#include "componenteditoritem.h"
#include "componenteditortreemodel.h"
#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include <IPXactWrapper/ComponentEditor/itemvisualizer.h>

#include <QApplication>

ComponentEditorItem::ComponentEditorItem(ComponentEditorTreeModel* model,
										 LibraryInterface* libHandler,
										 QSharedPointer<Component> component,
										 ComponentEditorItem* parent ):
QObject(parent),
libHandler_(libHandler),
component_(component), 
childItems_(),
model_(model),
parent_(parent) {

	connect(this, SIGNAL(contentChanged(ComponentEditorItem*)),
		model, SLOT(onContentChanged(ComponentEditorItem*)), Qt::UniqueConnection);
	connect(this, SIGNAL(createChild(ComponentEditorItem*, int)),
		model, SLOT(addItem(ComponentEditorItem*, int)), Qt::UniqueConnection);
	connect(this, SIGNAL(removeChild(ComponentEditorItem*, int)),
		model, SLOT(removeItem(ComponentEditorItem*, int)), Qt::UniqueConnection);
	connect(this, SIGNAL(moveChild(ComponentEditorItem*, int, int)),
		model, SLOT(moveItem(ComponentEditorItem*, int, int)), Qt::UniqueConnection);
}

ComponentEditorItem::ComponentEditorItem( LibraryInterface* libHandler, 
										 QSharedPointer<Component> component, 
										 ComponentEditorTreeModel* parent ):
QObject(parent),
libHandler_(libHandler),
component_(component),
childItems_(),
model_(parent), 
parent_(NULL) {

	connect(this, SIGNAL(contentChanged(ComponentEditorItem*)),
		parent, SLOT(onContentChanged(ComponentEditorItem*)), Qt::UniqueConnection);
}

ComponentEditorItem::~ComponentEditorItem() {
	childItems_.clear();
}

int ComponentEditorItem::row() const {
	if (parent_) {
		return parent_->getIndexOf(this);
	}
	return -1;
}

int ComponentEditorItem::getIndexOf( const ComponentEditorItem* child ) const {

	for (int i = 0; i < childItems_.size(); ++i) {
		if (childItems_.at(i).data() == child) {
			return i;
		}
	}
	return -1;
}

int ComponentEditorItem::rowCount() const {
	return childItems_.count();
}

bool ComponentEditorItem::hasChildren() const {
	return !childItems_.isEmpty();
}

QSharedPointer<ComponentEditorItem> ComponentEditorItem::child( const int index ) {
	if (index < 0 || childItems_.count() < index) {
		return QSharedPointer<ComponentEditorItem>();
	}

	return childItems_.at(index);
}

ComponentEditorItem* ComponentEditorItem::parent() {
	return parent_;
}

QFont ComponentEditorItem::getFont() const {
	QFont font(QApplication::font());
	font.setPointSize(font.pointSize() + 2);
	font.setBold(true);
	return font;
}

bool ComponentEditorItem::isValid() const {
	
	// if at least one child is not valid then this is not valid
	foreach (QSharedPointer<ComponentEditorItem> childItem, childItems_) {
		if (!childItem->isValid()) {
			return false;
		}
	}

	// if all children were valid then check the editor
	if (editor()) {
		return editor()->isValid();
	}

	// all children were valid
	return true;
}

ItemVisualizer* ComponentEditorItem::visualizer() {
	return NULL;
}

void ComponentEditorItem::onEditorChanged() {
	emit contentChanged(this);
}

void ComponentEditorItem::setLocked( bool locked ) {
	// if this item contains an editor
	if (editor()) {
		editor()->setDisabled(locked);
	}
	// if this item contains a visualizer
	if (visualizer()) {
		visualizer()->setDisabled(locked);
	}

	// also tell child items
	foreach (QSharedPointer<ComponentEditorItem> childItem, childItems_) {
		childItem->setLocked(locked);
	}
}

void ComponentEditorItem::refreshEditor() {
	if (editor()) {
		editor()->refresh();
	}
}

void ComponentEditorItem::onAddChild( int index ) {
	emit createChild(this, index);
}

void ComponentEditorItem::onRemoveChild( int index ) {
	emit removeChild(this, index);
}

void ComponentEditorItem::onMoveChild( int source, int target ) {
	emit moveChild(this, source, target);
}

void ComponentEditorItem::moveChild( const int sourceIndex, int targetIndex ) {

	// if the source index can not be used to identify an item
	if (sourceIndex < 0 || sourceIndex >= childItems_.count()) {
		return;
	}

	// Take the item from the list
	QSharedPointer<ComponentEditorItem> itemToMove = childItems_.takeAt(sourceIndex);

	childItems_.insert(targetIndex, itemToMove);
}

void ComponentEditorItem::createChild( int index ) {
	// This must be implemented in sub classes to create the correct type of child.
}

void ComponentEditorItem::removeChild( int index ) {
	Q_ASSERT(index >= 0);
	Q_ASSERT(index < childItems_.size());
	childItems_.removeAt(index);
}

bool ComponentEditorItem::canBeOpened() const {
	// Normally items can't be opened
	return false;
}

void ComponentEditorItem::openItem(bool builtinEditor) {

	// Normally items can't be opened
	return;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorItem::hasBuiltinEditor()
//-----------------------------------------------------------------------------
bool ComponentEditorItem::hasBuiltinEditor() const
{
    return false;
}
