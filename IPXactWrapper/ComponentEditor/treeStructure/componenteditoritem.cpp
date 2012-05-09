/* 
 *  	Created on: 7.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditoritem.cpp
 *		Project: Kactus 2
 */

#include "componenteditoritem.h"
#include "componenteditortreemodel.h"

#include <QApplication>

ComponentEditorItem::ComponentEditorItem(ComponentEditorTreeModel* model,
										 LibraryInterface* libHandler,
										 QSharedPointer<Component> component,
										 ComponentEditorItem* parent ):
QObject(parent),
libHandler_(libHandler),
component_(component), 
childItems_(),
parent_(parent) {

	connect(this, SIGNAL(contentChanged(ComponentEditorItem*)),
		model, SLOT(onContentChanged(ComponentEditorItem*)), Qt::UniqueConnection);
}

ComponentEditorItem::ComponentEditorItem( LibraryInterface* libHandler, 
										 QSharedPointer<Component> component, 
										 ComponentEditorTreeModel* parent ):
QObject(parent),
libHandler_(libHandler),
component_(component),
childItems_(),
parent_(NULL) {

	connect(this, SIGNAL(contentChanged(ComponentEditorItem*)),
		parent, SLOT(onContentChanged(ComponentEditorItem*)), Qt::UniqueConnection);
}

ComponentEditorItem::~ComponentEditorItem() {
}

int ComponentEditorItem::row() const {
	if (parent_) {
		return parent_->getIndexOf(this);
	}
	return -1;
}

int ComponentEditorItem::getIndexOf(const ComponentEditorItem* child ) const {
	return childItems_.indexOf(const_cast<ComponentEditorItem* const>(child));
}

int ComponentEditorItem::rowCount() const {
	return childItems_.count();
}

bool ComponentEditorItem::hasChildren() const {
	return !childItems_.isEmpty();
}

ComponentEditorItem* ComponentEditorItem::child( const int index ) {
	if (index < 0 || childItems_.count() < index) {
		return NULL;
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

void ComponentEditorItem::addChild( ComponentEditorItem* child ) {
	Q_ASSERT(child);
	childItems_.append(child);
}

void ComponentEditorItem::removeChild( ComponentEditorItem* child ) {
	Q_ASSERT(child);
	childItems_.removeAll(child);
}

void ComponentEditorItem::moveChild( const int sourceIndex, int targetIndex ) {

	// if the source index can not be used to identify an item
	if (sourceIndex < 0 || sourceIndex > childItems_.count()) {
		return;
	}

	// Take the item from the list
	ComponentEditorItem* itemToMove = childItems_.takeAt(sourceIndex);

	// if item is moved down then the target must be decremented because on item is
	// removed before adding to the list
	if (targetIndex > sourceIndex) {
		--targetIndex;
	}

	childItems_.insert(targetIndex, itemToMove);
}

QString ComponentEditorItem::text() const {
	return tr("root");
}

bool ComponentEditorItem::isValid() const {
	return true;
}

ItemEditor* ComponentEditorItem::editor() {
	return NULL;
}

void ComponentEditorItem::onContentChanged() {
	emit contentChanged(this);
}
