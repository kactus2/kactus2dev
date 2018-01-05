//-----------------------------------------------------------------------------
// File: ListManagerModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 28.01.2012
//
// Description:
// Model class for managing a list of strings.
//-----------------------------------------------------------------------------

#include "listmanagermodel.h"

#include <QVariant>

#include <common/KactusColors.h>

//-----------------------------------------------------------------------------
// Function: listmanagermodel::ListManagerModel()
//-----------------------------------------------------------------------------
ListManagerModel::ListManagerModel(QObject *parent, const QStringList& items): 
QAbstractListModel(parent),
    items_(items)
{

}

//-----------------------------------------------------------------------------
// Function: listmanagermodel::~ListManagerModel()
//-----------------------------------------------------------------------------
ListManagerModel::~ListManagerModel()
{

}

//-----------------------------------------------------------------------------
// Function: listmanagermodel::rowCount()
//-----------------------------------------------------------------------------
int ListManagerModel::rowCount( const QModelIndex&) const
{
	return items_.size();
}

//-----------------------------------------------------------------------------
// Function: listmanagermodel::data()
//-----------------------------------------------------------------------------
QVariant ListManagerModel::data( const QModelIndex& index, int role) const
{
	// nothing for invalid indexes
	if (!index.isValid() || index.row() < 0 || index.row() >= items_.size())
    {
		return QVariant();
	}

	// return data for display role
	if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
		return items_.at(index.row());
	}
	// if unsupported role
	else
    {
		return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: listmanagermodel::headerData()
//-----------------------------------------------------------------------------
QVariant ListManagerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return tr("Items");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: listmanagermodel::setData()
//-----------------------------------------------------------------------------
bool ListManagerModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ )
{
	if (!index.isValid() || index.row() < 0 || index.row() >= items_.size() || index.column() != 0)
    {
		return false;
	}

	if (role == Qt::EditRole)
    {
		items_[index.row()] = value.toString();

		emit dataChanged(index, index);
		return true;
	}

	// unsupported role
	else
    {
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: listmanagermodel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ListManagerModel::flags( const QModelIndex& index ) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
    }

	return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//-----------------------------------------------------------------------------
// Function: listmanagermodel::appendItem()
//-----------------------------------------------------------------------------
void ListManagerModel::appendItem( const QString item )
{
	beginInsertRows(QModelIndex(), items_.size(), items_.size());
	items_.append(item);
	endInsertRows();
}

//-----------------------------------------------------------------------------
// Function: listmanagermodel::addItem()
//-----------------------------------------------------------------------------
void ListManagerModel::addItem( const QModelIndex& index )
{
	int row = items_.size();
	
	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	items_.insert(row, QString("new item"));
	endInsertRows();
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: listmanagermodel::items()
//-----------------------------------------------------------------------------
const QStringList& ListManagerModel::items() const
{
	return items_;
}

//-----------------------------------------------------------------------------
// Function: listmanagermodel::setItems()
//-----------------------------------------------------------------------------
void ListManagerModel::setItems( const QStringList& items )
{
	beginResetModel();
	items_ = items;
	endResetModel();
}

//-----------------------------------------------------------------------------
// Function: listmanagermodel::remove()
//-----------------------------------------------------------------------------
void ListManagerModel::remove( const QModelIndex& index )
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= items_.size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	items_.removeAt(index.row());
	endRemoveRows();
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: listmanagermodel::moveItem()
//-----------------------------------------------------------------------------
void ListManagerModel::moveItem( const QModelIndex& originalPos, const QModelIndex& newPos )
{
	if (!originalPos.isValid() ||
        originalPos == newPos || 
        originalPos.row() < 0 || originalPos.row() >= items_.size())
    {
		return;
	}

	// if the new position is invalid index then put the item last in the list
	if (!newPos.isValid() || newPos.row() < 0 || newPos.row() >= items_.size())
    {
		beginResetModel();
		QString value = items_.takeAt(originalPos.row());
		items_.append(value);
		endResetModel();
	}
	// if both indexes were valid
	else
    {
		beginResetModel();
		items_.swap(originalPos.row(), newPos.row());
		endResetModel();
	}
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: listmanagermodel::replace()
//-----------------------------------------------------------------------------
void ListManagerModel::replace(QModelIndex& index, const QString newText)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= items_.size())
    {
		return;
	}

	items_.replace(index.row(), newText);
	emit dataChanged(index, index);
}

//-----------------------------------------------------------------------------
// Function: listmanagermodel::appendItems()
//-----------------------------------------------------------------------------
void ListManagerModel::appendItems( const QStringList& items )
{
	foreach (const QString item, items)
    {
		appendItem(item);
	}
}
