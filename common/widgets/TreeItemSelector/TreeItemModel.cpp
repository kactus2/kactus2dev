//-----------------------------------------------------------------------------
// File: TreeItemModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 30.8.2023
//
// Description:
// A model for generic tree items.
//-----------------------------------------------------------------------------

#include "TreeItemModel.h"

#include "TreeItem.h"

#include <common/KactusColors.h>

//-----------------------------------------------------------------------------
// Function: TreeItemModel::TreeItemModel()
//-----------------------------------------------------------------------------
TreeItemModel::TreeItemModel(TreeItem* rootItem, QObject* parent): 
    QAbstractItemModel(parent), 
    rootItem_(rootItem)
{

}

//-----------------------------------------------------------------------------
// Function: TreeItemModel::index()
//-----------------------------------------------------------------------------
QModelIndex TreeItemModel::index(int row, int column, QModelIndex const& parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return QModelIndex();
    }

    auto parentItem = getItem(parent);

    if (auto childItem = parentItem->children.at(row); childItem != nullptr)
    {
        return createIndex(row, column, childItem);
    }

    return QModelIndex();
}

//-----------------------------------------------------------------------------
// Function: TreeItemModel::parent()
//-----------------------------------------------------------------------------
QModelIndex TreeItemModel::parent(QModelIndex const& child) const
{
    if (!child.isValid())
    {
        return QModelIndex();
    }

    auto parentItem = getItem(child)->parent;

    if (parentItem == rootItem_)
    {
        return QModelIndex();
    }

    int parentRow = parentItem->parent->children.indexOf(parentItem);

    return createIndex(parentRow, 0, parentItem);
}

//-----------------------------------------------------------------------------
// Function: TreeItemModel::rowCount()
//-----------------------------------------------------------------------------
int TreeItemModel::rowCount(QModelIndex const& parent) const
{
    if (parent.column() > 0)
    {
        return 0;
    }

    return getItem(parent)->children.count();
}

//-----------------------------------------------------------------------------
// Function: TreeItemModel::columnCount()
//-----------------------------------------------------------------------------
int TreeItemModel::columnCount(QModelIndex const& /*parent*/) const
{
    return 2;
}

//-----------------------------------------------------------------------------
// Function: TreeItemModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags TreeItemModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }


    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

//-----------------------------------------------------------------------------
// Function: TreeItemModel::data()
//-----------------------------------------------------------------------------
QVariant TreeItemModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    auto item = getItem(index);

    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0)
        {
            return item->name;
        }
        else
        {
            return item->type;
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        if (index.column() == 0)
        {
            return KactusColors::REGULAR_TEXT;
        }
        else
        {
            return KactusColors::DISABLED_TEXT;
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: TreeItemModel::hasChildren()
//-----------------------------------------------------------------------------
bool TreeItemModel::hasChildren(QModelIndex const& parent) const
{ 
    return getItem(parent)->children.isEmpty() == false;
}


//-----------------------------------------------------------------------------
// Function: TreeItemModel::getItem()
//-----------------------------------------------------------------------------
TreeItem* TreeItemModel::getItem(QModelIndex const& index) const
{
    if (index.isValid())
    {
        return static_cast<TreeItem*>(index.internalPointer());
    }
        
    return rootItem_;
}
