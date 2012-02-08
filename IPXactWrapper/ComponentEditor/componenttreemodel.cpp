/* 
 *
 *  Created on: 21.1.2011
 *      Author: Antti Kamppi
 */

#include "componenttreemodel.h"
#include <models/component.h>
#include "componenttreeitem.h"

#include <QColor>

#include <QDebug>

ComponentTreeModel::ComponentTreeModel(QSharedPointer<Component> component, 
									   LibraryInterface* handler,
									   QObject *parent):
QAbstractItemModel(parent), 
rootItem_(0), 
component_(component),
handler_(handler) {

    rootItem_ = new ComponentTreeItem(ComponentTreeItem::COMPONENT, component.data(),
    		component, handler_, 0);
}

ComponentTreeModel::~ComponentTreeModel() {

	delete rootItem_;
	rootItem_ = 0;
}

QModelIndex ComponentTreeModel::index(int row, int column,
		const QModelIndex& parent) const {

	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	ComponentTreeItem* parentItem = 0;

	// if parent is invalid the asked index is for the root item
	if (!parent.isValid()) {
		parentItem = rootItem_;
	}
	else {
		parentItem = static_cast<ComponentTreeItem*>(
				parent.internalPointer());
	}

	Q_ASSERT(parentItem);

	// get pointer to specified child of the parent
	ComponentTreeItem* child = parentItem->child(row);

	// if the item was found
	if (child) {
		return createIndex(row, column, child);
	}
	// if child was not found
	else {
		return QModelIndex();
	}
}

QModelIndex ComponentTreeModel::generalEditorIndex() const {
	// the general editor is the first child of the root item
	return index(0, 0, QModelIndex());
}

QModelIndex ComponentTreeModel::parent(const QModelIndex& index) const {

	// if the child parameter is invalid then invalid model index is
	// returned
	if (!index.isValid()) {
		return QModelIndex();
	}

	// pointer to the child item
	ComponentTreeItem* childItem =
                        static_cast<ComponentTreeItem*>(index.internalPointer());
	ComponentTreeItem* parent = childItem->parent();

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

int ComponentTreeModel::rowCount(const QModelIndex& parent) const {

	// only one column exists
	if (parent.column() > 0) {
		return 0;
	}

	ComponentTreeItem* parentItem = 0;

	// if the given item is invalid, it is interpreted as root item
	if (!parent.isValid()) {
		parentItem = rootItem_;
	}
	else {
		parentItem = static_cast<ComponentTreeItem*>(
				parent.internalPointer());
	}

	// return how many children the parent has
	return parentItem->rowCount();
}

int ComponentTreeModel::columnCount(const QModelIndex&) const {

	// the model only has one column
	return 1;
}

QVariant ComponentTreeModel::data(const QModelIndex& index, int role) const {

	if (!index.isValid()) {
		return QVariant();
	}

	ComponentTreeItem* item = static_cast<ComponentTreeItem*>(
		index.internalPointer());
	Q_ASSERT_X(item, "ComponentTreeModel::data in Qt::DisplayRole",
		"static_cast failed to give valid ComponentTreeItem-pointer");

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
	// not supported role
	else {
		return QVariant();
	}
}


bool ComponentTreeModel::hasChildren(const QModelIndex& parent) const {
	
	// if parent is invalid then it is root and it always has children
	if (!parent.isValid()) {
		return true;
	}

	ComponentTreeItem* item = static_cast<ComponentTreeItem*>(
		parent.internalPointer());
	Q_ASSERT_X(item, "ComponentTreeModel::hasChildren",
		"static_cast failed to give valid ComponentTreeItem-pointer");

	return item->hasChildren();
}


Qt::ItemFlags ComponentTreeModel::flags(const QModelIndex& index) const {

	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ComponentTreeModel::headerData(int section,
		Qt::Orientation orientation, int role) const {

	if (section != 0 || orientation != Qt::Horizontal ||
			role != Qt::DisplayRole) {
                return QVariant();
	}

        return tr("Component element");
}

void ComponentTreeModel::setItemName( const QString newName, 
									 const QModelIndex& index )
{
	// do nothing for invalid indexes.
	if (!index.isValid()) {
		return;
	}

	// get pointer to the correct item
	ComponentTreeItem* item = static_cast<ComponentTreeItem*>(
		index.internalPointer());
	Q_ASSERT_X(item, "ComponentTreeModel::setItemName",
		"static_cast failed to give valid ComponentTreeItem-pointer");

	// change the text
	item->setText(newName);

	// inform the view of the changes.
	emit dataChanged(index, index);

}

void ComponentTreeModel::updateItem( const QModelIndex& index ) {
	emit dataChanged(index, index);
}

void ComponentTreeModel::removeItem( const QModelIndex& index ) {
	
	// if item's parent is invalid then the item is one of the item's in first
	// hierarchy level in tree and the item can't be removed
	QModelIndex parent = ComponentTreeModel::parent(index);
	if (!parent.isValid()) 	{
		return;
	}

	ComponentTreeItem* childItem = static_cast<ComponentTreeItem*>(
		index.internalPointer());
	Q_ASSERT_X(childItem, "ComponentTreeModel::removeItem",
		"static_cast failed to give valid ComponentTreeItem-pointer for child");

	// if the type of the item is one of the types that doesn't have an editor
	// then it can't be removed directly, also GENERAL editor is never removed.
	if (!childItem->getDataPointer() || 
		childItem->type() == ComponentTreeItem::GENERAL) {
		return;
	}

	ComponentTreeItem* parentItem = static_cast<ComponentTreeItem*>(
		parent.internalPointer());
	Q_ASSERT_X(parentItem, "ComponentTreeModel::removeItem",
		"static_cast failed to give valid ComponentTreeItem-pointer for parent");

	int childNumber = parentItem->getIndexOf(childItem);
	Q_ASSERT_X(childNumber >= 0, "ComponentTreeModel::removeItem",
		"The child did not belong to the parent");

	beginRemoveRows(parent, childNumber, childNumber);
	parentItem->removeChild(childItem);
	endRemoveRows();


}

QModelIndex ComponentTreeModel::createChild( const QModelIndex& parent ) {

	if (!parent.isValid()) {
		return QModelIndex();
	}

	ComponentTreeItem* parentItem = static_cast<ComponentTreeItem*>(
		parent.internalPointer());
	Q_ASSERT_X(parentItem, "ComponentTreeModel::createChild",
		"static_cast failed to give valid ComponentTreeItem-pointer");

	// if parentItem can't have children then don't create anything and
	// return an invalid model index
	if (!parentItem->canHaveChildren()) {
		return QModelIndex();
	}

	int addedIndex = parentItem->rowCount();

	beginInsertRows(parent, addedIndex, addedIndex);
	parentItem->createChild();
	endInsertRows();

	// return the model index of created child
	return index(addedIndex, 0, parent);
}

void ComponentTreeModel::moveItem( const QModelIndex& source, const QModelIndex& target) {

	// invalid indexes can't be moved
	if (!source.isValid() || !target.isValid())
		return;

	// source is set on top of the target in this case the location would not change.
// 	else if (source.row() + 1 == target.row())
// 		return;

	QModelIndex parentIndex = parent(source);
	if (!parentIndex.isValid())
		return;

	int targetIndex = target.row();
	if (target.row() > source.row())
		++targetIndex;

	ComponentTreeItem* parent = static_cast<ComponentTreeItem*>(parentIndex.internalPointer());

	beginMoveRows(parentIndex, source.row(), source.row(), parentIndex, targetIndex);
	parent->moveChild(source.row(), targetIndex);
	endMoveRows();

	emit itemMoved();
}

