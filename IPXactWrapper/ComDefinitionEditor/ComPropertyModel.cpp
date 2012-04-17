//-----------------------------------------------------------------------------
// File: ComPropertyModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.4.2012
//
// Description:
// Model for COM properties.
//-----------------------------------------------------------------------------

#include "ComPropertyModel.h"

#include "ComPropertyDelegate.h"

#include <models/ComProperty.h>

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::ComPropertyModel()
//-----------------------------------------------------------------------------
ComPropertyModel::ComPropertyModel(QObject *parent)
    : QAbstractTableModel(parent),
      table_()
{
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::~ComPropertyModel()
//-----------------------------------------------------------------------------
ComPropertyModel::~ComPropertyModel()
{
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::setProperties()
//-----------------------------------------------------------------------------
void ComPropertyModel::setProperties(QList< QSharedPointer<ComProperty> > const& properties)
{
    beginResetModel();

    table_.clear();

    foreach (QSharedPointer<ComProperty> prop, properties)
    {
        table_.append(QSharedPointer<ComProperty>(new ComProperty(*prop.data())));
    }

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::getProperties()
//-----------------------------------------------------------------------------
QList< QSharedPointer<ComProperty> > const& ComPropertyModel::getProperties() const
{
    return table_;
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::rowCount()
//-----------------------------------------------------------------------------
int ComPropertyModel::rowCount(QModelIndex const& parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return table_.size();
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::columnCount()
//-----------------------------------------------------------------------------
int ComPropertyModel::columnCount(QModelIndex const& parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return PROPERTY_COL_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::data()
//-----------------------------------------------------------------------------
QVariant ComPropertyModel::data(QModelIndex const& index, int role /*= Qt::DisplayRole*/) const
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
        case PROPERTY_COL_NAME:
            return table_.at(index.row())->getName();

        case PROPERTY_COL_REQUIRED:
            return table_.at(index.row())->isRequired();

        case PROPERTY_COL_TYPE:
            return table_.at(index.row())->getType();

        case PROPERTY_COL_DEFAULT:
            return table_.at(index.row())->getDefaultValue();

        case PROPERTY_COL_DESC:
            return table_.at(index.row())->getDescription();
        
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
// Function: ComPropertyModel::headerData()
//-----------------------------------------------------------------------------
QVariant ComPropertyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        switch (section)
        {
        case PROPERTY_COL_NAME:
            return tr("Name");

        case PROPERTY_COL_REQUIRED:
            return tr("Required");

        case PROPERTY_COL_TYPE:
            return tr("Type");

        case PROPERTY_COL_DEFAULT:
            return tr("Default Value");

        case PROPERTY_COL_DESC:
            return tr("Description");

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
// Function: ComPropertyModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ComPropertyModel::flags(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::setData()
//-----------------------------------------------------------------------------
bool ComPropertyModel::setData(QModelIndex const& index, QVariant const& value, int role /*= Qt::EditRole*/)
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
        case PROPERTY_COL_NAME:
            {
                table_[index.row()]->setName(value.toString());
                break;
            }
            

        case PROPERTY_COL_REQUIRED:
            {
                table_[index.row()]->setRequired(value.toBool());
                break;
            }

        case PROPERTY_COL_TYPE:
            {
                table_[index.row()]->setType(value.toString());
                break;
            }

        case PROPERTY_COL_DEFAULT:
            {
                table_[index.row()]->setDefaultValue(value.toString());
                break;
            }

        case PROPERTY_COL_DESC:
            {
                table_[index.row()]->setDescription(value.toString());
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
// Function: ComPropertyModel::isValid()
//-----------------------------------------------------------------------------
bool ComPropertyModel::isValid() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::onAdd()
//-----------------------------------------------------------------------------
void ComPropertyModel::onAdd()
{
    beginInsertRows(QModelIndex(), table_.size(), table_.size());
    table_.append(QSharedPointer<ComProperty>(new ComProperty()));
    endInsertRows();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::onAddItem()
//-----------------------------------------------------------------------------
void ComPropertyModel::onAddItem(QModelIndex const& index)
{
    int row = table_.size();

    // if the index is valid then add the item to the correct position
    if (index.isValid())
    {
        row = index.row();
    }

    beginInsertRows(QModelIndex(), row, row);
    table_.insert(row, QSharedPointer<ComProperty>(new ComProperty()));
    endInsertRows();

    // tell also parent widget that contents have been changed
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComPropertyModel::onRemove()
//-----------------------------------------------------------------------------
void ComPropertyModel::onRemove(QModelIndex const& index )
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
// Function: ComPropertyModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ComPropertyModel::onRemoveItem(QModelIndex const& index )
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
