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
#include <QIcon>

ComponentEditorItem::ComponentEditorItem(ComponentEditorTreeModel* model,
										 LibraryInterface* libHandler,
										 QSharedPointer<Component> component,
										 ComponentEditorItem* parent ):
QObject(parent),
libHandler_(libHandler),
component_(component), 
model_(model),
childItems_(),
editor_(NULL),
locked_(true),
parent_(parent) {

	connect(this, SIGNAL(contentChanged(ComponentEditorItem*)),
		model, SLOT(onContentChanged(ComponentEditorItem*)), Qt::UniqueConnection);
    connect(this, SIGNAL(contentChanged()),
        model, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(this, SIGNAL(createChild(ComponentEditorItem*, int)),
		model, SLOT(addItem(ComponentEditorItem*, int)), Qt::UniqueConnection);
	connect(this, SIGNAL(removeChild(ComponentEditorItem*, int)),
		model, SLOT(removeItem(ComponentEditorItem*, int)), Qt::UniqueConnection);
	connect(this, SIGNAL(moveChild(ComponentEditorItem*, int, int)),
		model, SLOT(moveItem(ComponentEditorItem*, int, int)), Qt::UniqueConnection);

	connect(this, SIGNAL(selectItem(ComponentEditorItem*)),
		model, SLOT(onSelectItem(ComponentEditorItem*)), Qt::UniqueConnection);

    connect(this, SIGNAL(helpUrlRequested(QString const&)),
            model, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);
	connect(this, SIGNAL(errorMessage(const QString&)),
		model, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);

	connect(this, SIGNAL(openDesign(const VLNV&, const QString&)),
		model, SIGNAL(openDesign(const VLNV&, const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(openSWDesign(const VLNV&, const QString&)),
		model, SIGNAL(openSWDesign(const VLNV&, const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(openSystemDesign(const VLNV&, const QString&)),
		model, SIGNAL(openSystemDesign(const VLNV&, const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(openBus(const VLNV&, const VLNV&)),
		model, SIGNAL(openBus(const VLNV&, const VLNV&)), Qt::UniqueConnection);
	connect(this, SIGNAL(openComDefinition(const VLNV&)),
		model, SIGNAL(openComDefinition(const VLNV&)), Qt::UniqueConnection);
}

ComponentEditorItem::ComponentEditorItem( LibraryInterface* libHandler, 
										 QSharedPointer<Component> component, 
										 ComponentEditorTreeModel* parent ):
QObject(parent),
libHandler_(libHandler),
component_(component),
model_(parent),
childItems_(), 
editor_(NULL),
locked_(true),
parent_(NULL) {

	connect(this, SIGNAL(contentChanged(ComponentEditorItem*)),
		parent, SLOT(onContentChanged(ComponentEditorItem*)), Qt::UniqueConnection);
}

ComponentEditorItem::~ComponentEditorItem() {
	childItems_.clear();
	if (editor_) {
		delete editor_;
		editor_ = NULL;
	}
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
	//font.setPointSize(font.pointSize() + 2);
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

	// all children were valid
	return true;
}

ItemVisualizer* ComponentEditorItem::visualizer() {
	return NULL;
}

void ComponentEditorItem::onEditorChanged() {
	
	// if there is a valid parent then update it also
	if (parent_) {
		emit contentChanged(parent_);
	}
	
	// update this item
	emit contentChanged(this);

	// also inform of child changes
	foreach (QSharedPointer<ComponentEditorItem> childItem, childItems_) {
		// tell the model that data has changed for the child
		emit contentChanged(childItem.data());

		// tell the child to update it's editor contents
		childItem->refreshEditor();

		// tell the child to update it's visualization graphics item
		childItem->updateGraphics();
	}
}

void ComponentEditorItem::setLocked( bool locked ) {
	// if this item contains an editor
	if (editor_) {
		editor_->setDisabled(locked);
	}
	// if this item contains a visualizer
	if (visualizer()) {
		visualizer()->setDisabled(locked);
	}

	// also tell child items
	foreach (QSharedPointer<ComponentEditorItem> childItem, childItems_) {
		childItem->setLocked(locked);
	}

	locked_ = locked;
}

void ComponentEditorItem::refreshEditor() {
	if (editor_) {
		editor_->refresh();
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

void ComponentEditorItem::createChild( int /*index*/ ) {
	// This must be implemented in sub classes to create the correct type of child.
}

void ComponentEditorItem::removeChild( int index ) {
	Q_ASSERT(index >= 0);
	Q_ASSERT(index < childItems_.size());

	// first tell the child to remove the graphics item
	childItems_[index]->removeGraphicsItem();

	// then the child can be removed
	childItems_.removeAt(index);
}

bool ComponentEditorItem::canBeOpened() const {
	// Normally items can't be opened
	return false;
}

void ComponentEditorItem::openItem(bool /*builtinEditor*/) {

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

QGraphicsItem* ComponentEditorItem::getGraphicsItem() {
	return NULL;
}

void ComponentEditorItem::updateGraphics() {
	return;
}

void ComponentEditorItem::removeGraphicsItem() {
	return;
}

bool ComponentEditorItem::hasIcon() const
{
    // Normally items have no icon.
    return false;
}

QIcon ComponentEditorItem::getIcon() const
{
    // Normally items have no default icon.
    return QIcon();
}

void ComponentEditorItem::onSelectRequest() {
	emit selectItem(this);
}
