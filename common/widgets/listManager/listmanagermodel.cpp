/* 
 *
 *  Created on: 28.1.2011
 *      Author: Antti Kamppi
 * 		filename: listmanagermodel.cpp
 */

#include "listmanagermodel.h"

#include <QVariant>

#include <QDebug>

ListManagerModel::ListManagerModel(QObject *parent, 
								   const QStringList& items)
	: QAbstractListModel(parent), items_(items) {

}

ListManagerModel::~ListManagerModel() {

}

int ListManagerModel::rowCount( const QModelIndex&) const {
	return items_.size();
}

QVariant ListManagerModel::data( const QModelIndex& index, int role) const {
	
	// nothing for invalid indexes
	if (!index.isValid()) {
		return QVariant();
	}

	// if index.row() is invalid
	else if (index.row() < 0 || index.row() >= items_.size()) {
		return QVariant();
	}

	// return data for display role
	if (role == Qt::DisplayRole) {
		return items_.at(index.row());
	}
	// if unsupported role
	else {
		return QVariant();
	}
}

QVariant ListManagerModel::headerData(int section, Qt::Orientation orientation, int role) const {

	// only one column to display
	if (section != 0) {
		return QVariant();
	}
	// only horizontal headers
	else if (orientation != Qt::Horizontal) {
		return QVariant();
	}

	// data for displayRole
	if (role == Qt::DisplayRole) {
		return tr("Items");
	}
	// unsupported role
	else {
		return QVariant();
	}

}

bool ListManagerModel::setData( const QModelIndex& index,
							   const QVariant& value,
							   int role /*= Qt::EditRole*/ ) {
								   
	if (!index.isValid())
		return false;

	// if row is invalid
	else if (index.row() < 0 || index.row() >= items_.size())
		return false;

	// list model has only one column
	else if (index.column() != 0)
		return false;

	if (role == Qt::EditRole) {
		
		items_[index.row()] = value.toString();

		emit dataChanged(index, index);
		return true;
	}

	// unsupported role
	else {
		return false;
	}
}

Qt::ItemFlags ListManagerModel::flags( const QModelIndex& index ) const {
	if (!index.isValid())
		return Qt::NoItemFlags;

	return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void ListManagerModel::appendItem( const QString item ) {
	beginInsertRows(QModelIndex(), items_.size(), items_.size());
	items_.append(item);
	endInsertRows();
}

void ListManagerModel::addItem( const QModelIndex& index ) {
	int row = items_.size();
	
	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	items_.insert(row, QString("new item"));
	endInsertRows();
	emit contentChanged();
}

const QStringList& ListManagerModel::items() const {
	return items_;
}

void ListManagerModel::setItems( const QStringList& items ) {
	beginResetModel();
	items_ = items;
	endResetModel();
}

void ListManagerModel::remove( const QModelIndex& index ) {

	// dont remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= items_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	items_.removeAt(index.row());
	endRemoveRows();
	emit contentChanged();
}

void ListManagerModel::moveItem( const QModelIndex& originalPos, 
								const QModelIndex& newPos ) {

	// if there was no item in the starting point
	if (!originalPos.isValid()) {
		return;
	}
	// if the indexes are the same
	else if (originalPos == newPos) {
		return;
	}
	else if (originalPos.row() < 0 || originalPos.row() >= items_.size()) {
		return;
	}

	// if the new position is invalid index then put the item last in the list
	if (!newPos.isValid() || newPos.row() < 0 || newPos.row() >= items_.size()) {

		beginResetModel();
		QString value = items_.takeAt(originalPos.row());
		items_.append(value);
		endResetModel();
	}
	// if both indexes were valid
	else {
		beginResetModel();
		items_.swap(originalPos.row(), newPos.row());
		endResetModel();
	}
	emit contentChanged();
}

void ListManagerModel::replace(QModelIndex& index, const QString newText) {

	if (!index.isValid()) {
		return;
	}
	// make sure the row is correct
	else if (index.row() < 0 || index.row() >= items_.size()) {
		return;
	}

	items_.replace(index.row(), newText);
	emit dataChanged(index, index);
}

void ListManagerModel::appendItems( const QStringList& items ) {
	foreach (const QString item, items) {
		appendItem(item);
	}
}
