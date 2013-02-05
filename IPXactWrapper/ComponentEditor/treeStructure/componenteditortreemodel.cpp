/* 
 *  	Created on: 7.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditortreemodel.cpp
 *		Project: Kactus 2
 */

#include "componenteditortreemodel.h"

#include <QColor>

#include <QDebug>

ComponentEditorTreeModel::ComponentEditorTreeModel( LibraryInterface* libHandler,
												   QObject* parent,
                                                   QWidget* parentWnd)
    : QAbstractItemModel(parent),
      libHandler_(libHandler),
      rootItem_(),
      parentWnd_(parentWnd)
{

	setObjectName(tr("ComponentEditorTreeModel"));
}

ComponentEditorTreeModel::~ComponentEditorTreeModel() {
	rootItem_.clear();
}

void ComponentEditorTreeModel::setComponent( QSharedPointer<Component> component ) {

	beginResetModel();
	rootItem_.clear();
	rootItem_ = QSharedPointer<ComponentEditorRootItem>(
		new ComponentEditorRootItem(libHandler_, component, this, parentWnd_));

	endResetModel();
}

int ComponentEditorTreeModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	// only one column exists
	if (parent.column() > 0) {
		return 0;
	}

	ComponentEditorItem* parentItem = 0;

	// if the given item is invalid, it is interpreted as root item
	if (!parent.isValid()) {
		parentItem = rootItem_.data();
	}
	else {
		parentItem = static_cast<ComponentEditorItem*>(
			parent.internalPointer());
	}

	// return how many children the parent has
	return parentItem->rowCount();
}

int ComponentEditorTreeModel::columnCount( const QModelIndex&) const {
	return 1;
}

QVariant ComponentEditorTreeModel::data( const QModelIndex& index, 
										int role /*= Qt::DisplayRole*/ ) const {

	if (!index.isValid()) {
		return QVariant();
	}

	ComponentEditorItem* item = static_cast<ComponentEditorItem*>(
		index.internalPointer());
	Q_ASSERT(item);

	if (role == Qt::DisplayRole) {

		QString text = item->text();

		// if the object has not a name
		if (text.isEmpty()) {
			text = tr("unnamed");
		}

		return text;
	}
	else if (role == Qt::FontRole) {
		return item->getFont();
	}
	else if (role == Qt::ForegroundRole) {

		if (item->isValid())
			return QColor("black");
		else
			return QColor("red");
	}
	else if (role == Qt::ToolTipRole) {
		return item->getTooltip();
	}
	// not supported role
	else {
		return QVariant();
	}
}

Qt::ItemFlags ComponentEditorTreeModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool ComponentEditorTreeModel::hasChildren( const QModelIndex& parent /*= QModelIndex()*/ ) const {

	ComponentEditorItem* parentItem = 0;

	// if the given item is invalid, it is interpreted as root item
	if (!parent.isValid()) {
		parentItem = rootItem_.data();
	}
	else {
		parentItem = static_cast<ComponentEditorItem*>(
			parent.internalPointer());
	}

	// return how many children the parent has
	return parentItem->hasChildren();
}

void ComponentEditorTreeModel::onContentChanged(ComponentEditorItem* item ) {
	QModelIndex index = ComponentEditorTreeModel::index(item);
	emit dataChanged(index, index);
}

QModelIndex ComponentEditorTreeModel::index(int row,
											int column, 
											const QModelIndex& parent /*= QModelIndex()*/ ) const {

	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	ComponentEditorItem* parentItem = 0;

	// if parent is invalid the asked index is for the root item
	if (!parent.isValid()) {
		parentItem = rootItem_.data();
	}
	else {
		parentItem = static_cast<ComponentEditorItem*>(
			parent.internalPointer());
	}

	Q_ASSERT(parentItem);

	// get pointer to specified child of the parent
	QSharedPointer<ComponentEditorItem> child = parentItem->child(row);

	// if the item was found
	if (child) {
		return createIndex(row, column, child.data());
	}
	// if child was not found
	else {
		return QModelIndex();
	}
}

QModelIndex ComponentEditorTreeModel::index(ComponentEditorItem* item ) const {
	Q_ASSERT(item);

	// if the item is the root item then it's model index is invalid
	if (item == rootItem_.data()) {
		return QModelIndex();
	}
	else {
		return createIndex(item->row(), 0, item);
	}
}

QModelIndex ComponentEditorTreeModel::parent( const QModelIndex& index ) const {
	// if the child parameter is invalid then there is no parent
	if (!index.isValid()) {
		return QModelIndex();
	}

	// pointer to the child item
	ComponentEditorItem* childItem =
		static_cast<ComponentEditorItem*>(index.internalPointer());
	ComponentEditorItem* parent = childItem->parent();

	// if the parent does not exist then this item is root item
	if (!parent) {
		return QModelIndex();
	}

	// if row is invalid then the grandparent does not exist and parent is
	// a root item so we return an invalid QModelIndex
	int row = parent->row();
	if (row == -1) {
		return QModelIndex();
	}

	// create new index and return it
	return createIndex(row, 0, parent);
}

void ComponentEditorTreeModel::setLocked( bool locked ) {
	rootItem_->setLocked(locked);
}

void ComponentEditorTreeModel::addItem( ComponentEditorItem* parentItem, int childIndex ) {
	Q_ASSERT(parentItem);
	Q_ASSERT(childIndex >= 0);

	// find the index of the parent item
	QModelIndex parentIndex = index(parentItem);

	beginInsertRows(parentIndex, childIndex, childIndex);
	parentItem->createChild(childIndex);
	endInsertRows();

	emit expandItem(parentIndex);
}

void ComponentEditorTreeModel::removeItem( ComponentEditorItem* parentItem, int childIndex ) {
	Q_ASSERT(parentItem);
	Q_ASSERT(childIndex >= 0);

	// find the index of the parent item
	QModelIndex parentIndex = index(parentItem);

	beginRemoveRows(parentIndex, childIndex, childIndex);
	parentItem->removeChild(childIndex);
	endRemoveRows();
}

void ComponentEditorTreeModel::moveItem( ComponentEditorItem* parentItem, int childSource, int childtarget ) {
	Q_ASSERT(parentItem);
	Q_ASSERT(childSource >= 0);
	Q_ASSERT(childtarget >= 0);

	// if the location of the item does not change
	if (childSource == childtarget) {
		return;
	}

	// the index identifying the parent object
	QModelIndex parentIndex = index(parentItem);

	// target index is told to the parent item when moving
	int targetIndex = childtarget;

	// if item is moved down then the target must be incremented for abstract item's
	// way to inform view of row moves
	if (childtarget > childSource) {
		++childtarget;
	}

	// make the moving of the item
	beginMoveRows(parentIndex, childSource, childSource, parentIndex, childtarget);
	parentItem->moveChild(childSource, targetIndex);
	endMoveRows();
}

void ComponentEditorTreeModel::onSelectBusInterface( const QString& interfaceName ) {
	const QSharedPointer<ComponentEditorItem> busIfItem = rootItem_->getBusInterfaceItem(interfaceName);
	if (!busIfItem) {
		return;
	}

	// inform component editor to select the item
	QModelIndex busIfIndex = index(busIfItem.data());
	emit selectItem(busIfIndex);
}

void ComponentEditorTreeModel::onSelectItem( ComponentEditorItem* item ) {
	QModelIndex itemIndex = index(item);
	emit selectItem(itemIndex);
}
