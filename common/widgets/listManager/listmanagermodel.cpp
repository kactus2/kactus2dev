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

void ListManagerModel::appendItem( const QString item ) {
	beginInsertRows(QModelIndex(), items_.size(), items_.size());
	items_.append(item);
	endInsertRows();
}

const QStringList& ListManagerModel::items() const {
	return items_;
}

void ListManagerModel::setItems( const QStringList& items ) {
	beginResetModel();
	items_ = items;
	endResetModel();
}

void ListManagerModel::remove( QModelIndex& index ) {

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
}

void ListManagerModel::moveUp( QModelIndex& index ) {
	// dont move anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid and if this is already top item
	// then it cant be moved up
	else if (index.row() <= 0 || index.row() >= items_.size()) {
		return;
	}

	beginMoveRows(QModelIndex(), index.row(), index.row(), QModelIndex(),
		index.row() - 1);
	items_.swap(index.row(), index.row() - 1);
	endMoveRows();

}

void ListManagerModel::moveDown( QModelIndex& index ) {
	// dont move anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid and if this is already bottom item
	// then it cant be moved down
	else if (index.row() < 0 || index.row() == items_.size()-1) {
		return;
	}
	
	// inform view that rows are moved
	beginMoveRows(QModelIndex(), index.row(), index.row(), QModelIndex(),
		index.row() + 2);
	// move the rows
	items_.swap(index.row(), index.row() + 1);

	// inform that move has been finished
	endMoveRows();

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
