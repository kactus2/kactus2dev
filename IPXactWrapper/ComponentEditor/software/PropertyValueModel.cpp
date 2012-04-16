//-----------------------------------------------------------------------------
// File: PropertyValueModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 16.4.2012
//
// Description:
// Model for property values.
//-----------------------------------------------------------------------------

#include "PropertyValueModel.h"

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::PropertyValueModel()
//-----------------------------------------------------------------------------
PropertyValueModel::PropertyValueModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::~PropertyValueModel()
//-----------------------------------------------------------------------------
PropertyValueModel::~PropertyValueModel()
{
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::setData()
//-----------------------------------------------------------------------------
void PropertyValueModel::setData(QMap<QString, QString> const& propertyValues)
{
    beginResetModel();

    table_.clear();

    QMapIterator<QString, QString> iter(propertyValues);

    while (iter.hasNext())
    {
        iter.next();
        table_.append(NameValuePair(iter.key(), iter.value()));
    }

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::getData()
//-----------------------------------------------------------------------------
QMap<QString, QString> PropertyValueModel::getData() const
{
    QMap<QString, QString> values;

    foreach (NameValuePair const& pair, table_)
    {
        values[pair.first] = pair.second;
    }

    return values;
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::rowCount()
//-----------------------------------------------------------------------------
int PropertyValueModel::rowCount(QModelIndex const& parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return table_.size();
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::columnCount()
//-----------------------------------------------------------------------------
int PropertyValueModel::columnCount(QModelIndex const& parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return 2;
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::data()
//-----------------------------------------------------------------------------
QVariant PropertyValueModel::data(QModelIndex const& index, int role /*= Qt::DisplayRole*/) const
{
    if (!index.isValid())
    {
        return QVariant();
    }
    else if (index.row() < 0 || index.row() >= table_.size())
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case 0:
            return table_.at(index.row()).first;
        
        case 1:
            return table_.at(index.row()).second;
        
        default:
            return QVariant();
        }
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::headerData()
//-----------------------------------------------------------------------------
QVariant PropertyValueModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        switch (section)
        {
        case 0:
            return tr("Name");

        case 1:
            return tr("Value");

        default:
            return QVariant();
        }
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags PropertyValueModel::flags(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::setData()
//-----------------------------------------------------------------------------
bool PropertyValueModel::setData(QModelIndex const& index, QVariant const& value, int role /*= Qt::EditRole*/)
{
    if (!index.isValid())
    {
        return false;
    }
    else if (index.row() < 0 || index.row() >= table_.size())
    {
        return false;
    }

    if (role == Qt::EditRole)
    {
        switch (index.column())
        {
        case 0:
            {
                table_[index.row()].first = value.toString();
                break;
            }

        case 1:
            {
                table_[index.row()].second = value.toString();
                break;
            }

        default:
            return false;
        }

        emit dataChanged(index, index);
        emit contentChanged();
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::isValid()
//-----------------------------------------------------------------------------
bool PropertyValueModel::isValid() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::apply()
//-----------------------------------------------------------------------------
void PropertyValueModel::apply()
{

}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::restore()
//-----------------------------------------------------------------------------
void PropertyValueModel::restore()
{

}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::onAdd()
//-----------------------------------------------------------------------------
void PropertyValueModel::onAdd()
{
    NameValuePair element(QString(""), QString(""));

    if (!table_.contains(element))
    {
        beginInsertRows(QModelIndex(), table_.size(), table_.size());
        table_.append(element);
        endInsertRows();
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::onAddItem()
//-----------------------------------------------------------------------------
void PropertyValueModel::onAddItem(QModelIndex const& index)
{
    int row = table_.size();

    // if the index is valid then add the item to the correct position
    if (index.isValid())
    {
        row = index.row();
    }

    NameValuePair element(QString(""), QString(""));

    if (!table_.contains(element))
    {
        beginInsertRows(QModelIndex(), row, row);
        table_.insert(row, element);
        endInsertRows();

        // tell also parent widget that contents have been changed
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::onRemove()
//-----------------------------------------------------------------------------
void PropertyValueModel::onRemove(QModelIndex const& index )
{
    if (!index.isValid())
    {
        return;
    }

    // remove the indexed configurable element
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    table_.removeAt(index.row());
    endRemoveRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PropertyValueModel::onRemoveItem()
//-----------------------------------------------------------------------------
void PropertyValueModel::onRemoveItem(QModelIndex const& index )
{
    // don't remove anything if index is invalid
    if (!index.isValid())
    {
        return;
    }
    // make sure the row number if valid
    else if (index.row() < 0 || index.row() >= table_.size())
    {
        return;
    }

    // remove the indexed configurable element
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    table_.removeAt(index.row());
    endRemoveRows();

    emit contentChanged();
}