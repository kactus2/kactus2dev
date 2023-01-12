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
ListManagerModel::ListManagerModel(QStringList const& items, QObject *parent): 
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
int ListManagerModel::rowCount(QModelIndex const& /*parent*/) const
{
    return items_.size();
}

//-----------------------------------------------------------------------------
// Function: listmanagermodel::data()
//-----------------------------------------------------------------------------
QVariant ListManagerModel::data(QModelIndex const& index, int role) const
{
    // nothing for invalid indexes
    if (index.isValid() == false || index.row() < 0 || index.row() >= items_.size())
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
bool ListManagerModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (index.isValid() == false || index.row() < 0 || index.row() >= items_.size() || index.column() != 0)
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
Qt::ItemFlags ListManagerModel::flags( QModelIndex const& index ) const
{
    if (index.isValid() == false)
    {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//-----------------------------------------------------------------------------
// Function: listmanagermodel::appendItem()
//-----------------------------------------------------------------------------
void ListManagerModel::appendItem(QString const& item)
{
    beginInsertRows(QModelIndex(), items_.size(), items_.size());
    items_.append(item);
    endInsertRows();
}

//-----------------------------------------------------------------------------
// Function: listmanagermodel::addItem()
//-----------------------------------------------------------------------------
void ListManagerModel::addItem(QModelIndex const& index)
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
void ListManagerModel::setItems(QStringList const& items)
{
    beginResetModel();
    items_ = items;
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: listmanagermodel::remove()
//-----------------------------------------------------------------------------
void ListManagerModel::remove(QModelIndex const& index)
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
void ListManagerModel::moveItem(QModelIndex const& originalPos, QModelIndex const& newPos)
{
    if (originalPos.isValid() == false || originalPos == newPos || 
        originalPos.row() < 0 || originalPos.row() >= items_.size())
    {
        return;
    }

    // if the new position is invalid index then put the item last in the list
    if (newPos.isValid() == false || newPos.row() < 0 || newPos.row() >= items_.size())
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
        items_.swapItemsAt(originalPos.row(), newPos.row());
        endResetModel();
    }
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: listmanagermodel::replace()
//-----------------------------------------------------------------------------
void ListManagerModel::replace(QModelIndex& index, const QString newText)
{
    if (index.isValid() == false || index.row() < 0 || index.row() >= items_.size())
    {
        return;
    }

    items_.replace(index.row(), newText);
    emit dataChanged(index, index);
}
