//-----------------------------------------------------------------------------
// File: bridgesmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 21.06.2012
//
// Description:
// The model to manage the bridges of a slave interface.
//-----------------------------------------------------------------------------

#include "bridgesmodel.h"
#include "BridgeColumns.h"

#include <common/KactusColors.h>

#include <KactusAPI/include/TransparentBridgeInterface.h>

//-----------------------------------------------------------------------------
// Function: BridgesModel::BridgesModel()
//-----------------------------------------------------------------------------
BridgesModel::BridgesModel(TransparentBridgeInterface* bridgeInterface, QObject *parent):
QAbstractTableModel(parent),
bridgeInterface_(bridgeInterface)
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

    return bridgeInterface_->itemCount();
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
    if (!index.isValid() || index.row() < 0 || index.row() >= bridgeInterface_->itemCount())
    {
		return QVariant();
	}
	
    QString masterReference = QString::fromStdString(bridgeInterface_->getIndexedMasterReference(index.row()));

    if (role == Qt::DisplayRole) 
    {
        if (index.column() == BridgeColumns::MASTER_COLUMN)
        {
            return masterReference;
        }
        else
        {
            return QVariant();
        }
    }
	
	else if (role == Qt::ForegroundRole)
    {
        if (masterReference.isEmpty())
        {
			return KactusColors::ERROR;
		}
		else
        {
			return KactusColors::REGULAR_TEXT;
		}
	}
	else if (role == Qt::BackgroundRole)
    {
        if (index.column() == BridgeColumns::MASTER_COLUMN)
        {
            return KactusColors::MANDATORY_FIELD;
        }
        else
        {
            return KactusColors::REGULAR_FIELD;
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
    if (!index.isValid() || index.row() < 0 || index.row() >= bridgeInterface_->itemCount())
    {
		return false;
    }

    if (role == Qt::EditRole)
    {
        if (index.column() == BridgeColumns::MASTER_COLUMN)
        {
            std::string currentMaster = bridgeInterface_->getIndexedMasterReference(index.row());
            bridgeInterface_->setMasterReference(currentMaster, value.toString().toStdString());
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
    int row = bridgeInterface_->itemCount();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);

    bridgeInterface_->addBridge(row);

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
    if (!index.isValid() || index.row() < 0 || index.row() >= bridgeInterface_->itemCount())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());

    std::string masterReference = bridgeInterface_->getIndexedMasterReference(index.row());
    bridgeInterface_->removeTransparentBridge(masterReference);

	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BridgesModel::refresh()
//-----------------------------------------------------------------------------
void BridgesModel::refresh()
{
	beginResetModel();
	endResetModel();
}
