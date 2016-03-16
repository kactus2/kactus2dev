//-----------------------------------------------------------------------------
// File: bridgesmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 21.06.2012
//
// Description:
// The model to manage the bridges of a slave interface.
//-----------------------------------------------------------------------------

#include "bridgesmodel.h"
#include "BridgeColumns.h"

#include <QColor>

//-----------------------------------------------------------------------------
// Function: BridgesModel::BridgesModel()
//-----------------------------------------------------------------------------
BridgesModel::BridgesModel(QSharedPointer<SlaveInterface> slave, QObject *parent):
QAbstractTableModel(parent),
    bridges_(slave->getBridges())
{

}

//-----------------------------------------------------------------------------
// Function: BridgesModel::~BridgesModel()
//-----------------------------------------------------------------------------
BridgesModel::~BridgesModel()
{
}

//-----------------------------------------------------------------------------
// Function: BridgesModel::rowCount()
//-----------------------------------------------------------------------------
int BridgesModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return bridges_->size();
}

//-----------------------------------------------------------------------------
// Function: BridgesModel::columnCount()
//-----------------------------------------------------------------------------
int BridgesModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return BridgeColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: BridgesModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags BridgesModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: BridgesModel::headerData()
//-----------------------------------------------------------------------------
QVariant BridgesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (section == BridgeColumns::MASTER_COLUMN)
    {
        return tr("Master bus interface");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: BridgesModel::data()
//-----------------------------------------------------------------------------
QVariant BridgesModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= bridges_->size())
    {
		return QVariant();
	}
	

    if (role == Qt::DisplayRole) 
    {
        if (index.column() == BridgeColumns::MASTER_COLUMN)
        {
            return bridges_->at(index.row())->masterRef_;
        }
        else
        {
            return QVariant();
        }
    }
	
	else if (role == Qt::ForegroundRole)
    {
		if (bridges_->at(index.row())->masterRef_.isEmpty())
        {
			return QColor("red");
		}
		else
        {
			return QColor("black");
		}
	}
	else if (role == Qt::BackgroundRole)
    {
        if (index.column() == BridgeColumns::MASTER_COLUMN)
        {
            return QColor("LemonChiffon");
        }
        else
        {
            return QColor("white");
        }
    }

	else 
    {
		return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: BridgesModel::setData()
//-----------------------------------------------------------------------------
bool BridgesModel::setData(QModelIndex const& index, const QVariant& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= bridges_->size())
    {
		return false;
    }

    if (role == Qt::EditRole)
    {
        if (index.column() == BridgeColumns::MASTER_COLUMN)
        {
            (*bridges_)[index.row()]->masterRef_ = value.toString();

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
// Function: BridgesModel::onAddItem()
//-----------------------------------------------------------------------------
void BridgesModel::onAddItem(QModelIndex const& index)
{
	int row = bridges_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	bridges_->insert(row, QSharedPointer<SlaveInterface::Bridge>(new SlaveInterface::Bridge()));
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BridgesModel::onRemoveItem()
//-----------------------------------------------------------------------------
void BridgesModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= bridges_->size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	bridges_->removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BridgesModel::refresh()
//-----------------------------------------------------------------------------
void BridgesModel::refresh(QSharedPointer<SlaveInterface> slave)
{
	Q_ASSERT(slave);
	beginResetModel();
	bridges_ = slave->getBridges();
	endResetModel();
}

//-----------------------------------------------------------------------------
// Function: BridgesModel::isValid()
//-----------------------------------------------------------------------------
bool BridgesModel::isValid() const
{
	// on each bridge the master ref must be non-empty
	foreach (QSharedPointer<SlaveInterface::Bridge> bridge, *bridges_)
    {
		if (bridge->masterRef_.isEmpty())
        {
			return false;
		}
	}
	return true;
}
