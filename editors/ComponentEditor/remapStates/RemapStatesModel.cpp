//-----------------------------------------------------------------------------
// File: RemapStatesModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.04.2015
//
// Description:
// The model to manage the remap states summary.
//-----------------------------------------------------------------------------

#include "RemapStatesModel.h"

#include <QColor>

//-----------------------------------------------------------------------------
// Function: RemapStatesModel::RemapStatesModel()
//-----------------------------------------------------------------------------
RemapStatesModel::RemapStatesModel(QSharedPointer<QList<QSharedPointer<RemapState> > > remapStates,QObject* parent):
QAbstractTableModel(parent),
remapStates_(remapStates)
{

}

//-----------------------------------------------------------------------------
// Function: RemapStatesModel::~RemapStatesModel()
//-----------------------------------------------------------------------------
RemapStatesModel::~RemapStatesModel()
{

}

//-----------------------------------------------------------------------------
// Function: RemapStatesModel::rowCount()
//-----------------------------------------------------------------------------
int RemapStatesModel::rowCount(const QModelIndex& parent /* = QModelIndex() */) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return remapStates_->count();
}

//-----------------------------------------------------------------------------
// Function: RemapStatesModel::columnCount()
//-----------------------------------------------------------------------------
int RemapStatesModel::columnCount(const QModelIndex& parent /* = QModelIndex() */) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return RemapStatesModel::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: RemapStatesModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags RemapStatesModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }
    else
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }
}

//-----------------------------------------------------------------------------
// Function: RemapStatesModel::headerData()
//-----------------------------------------------------------------------------
QVariant RemapStatesModel::headerData(int section, Qt::Orientation orientation, int role /* = Qt::DisplayRole */)
    const
{
    if (orientation != Qt::Horizontal)
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        if (section == RemapStatesModel::NAME_COLUMN)
        {
            return tr("Name");
        }
        else if (section == RemapStatesModel::DISPLAY_NAME_COLUMN)
        {
            return tr("Display name");
        }
        else if (section == RemapStatesModel::DESCRIPTION_COLUMN)
        {
            return tr("Description");
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: RemapStatesModel::data()
//-----------------------------------------------------------------------------
QVariant RemapStatesModel::data(const QModelIndex& index, int role /* = Qt::DisplayRole */) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= remapStates_->size())
    {
        return QVariant();
    }

    if (Qt::DisplayRole == role)
    {
        if (index.column() == RemapStatesModel::NAME_COLUMN)
        {
            return remapStates_->at(index.row())->getName();
        }
        else if (index.column() == RemapStatesModel::DISPLAY_NAME_COLUMN)
        {
            return remapStates_->at(index.row())->getDisplayName();
        }
        else if (index.column() == RemapStatesModel::DESCRIPTION_COLUMN)
        {
            return remapStates_->at(index.row())->getDescription();
        }
    }

    else if (Qt::BackgroundRole == role)
    {
        if (index.column() == RemapStatesModel::NAME_COLUMN)
        {
            return QColor("lemonChiffon");
        }
        else
        {
            return QColor("white");
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: RemapStatesModel::setData()
//-----------------------------------------------------------------------------
bool RemapStatesModel::setData(const QModelIndex& index, const QVariant& value, int role /* = Qt::EditRole */)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= remapStates_->size())
    {
        return false;
    }

    if (Qt::EditRole == role)
    {
        if (index.column() == RemapStatesModel::NAME_COLUMN)
        {
            remapStates_->at(index.row())->setName(value.toString());
        }
        else if (index.column() == RemapStatesModel::DISPLAY_NAME_COLUMN)
        {
            remapStates_->at(index.row())->setDisplayName(value.toString());
        }
        else if (index.column() == RemapStatesModel::DESCRIPTION_COLUMN)
        {
            remapStates_->at(index.row())->setDescription(value.toString());
        }
        else
        {
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
// Function: RemapStatesModel::isValid()
//-----------------------------------------------------------------------------
bool RemapStatesModel::isValid() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: RemapStatesModel::onAddItem()
//-----------------------------------------------------------------------------
void RemapStatesModel::onAddItem(const QModelIndex& index)
{
    int row = remapStates_->size();

    if (index.isValid())
    {
        row = index.row();
    }

    beginInsertRows(QModelIndex(), row, row);
    
    remapStates_->insert(row, QSharedPointer<RemapState>(new RemapState()));
    
    endInsertRows();

    emit remapStateAdded(row);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: RemapStatesModel::onRemoveItem()
//-----------------------------------------------------------------------------
void RemapStatesModel::onRemoveItem(const QModelIndex& index)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= remapStates_->size())
    {
        return;
    }

    beginRemoveRows(QModelIndex(), index.row(), index.row());
    remapStates_->removeAt(index.row());
    endRemoveRows();

    emit remapStateRemoved(index.row());
    emit contentChanged();
}
