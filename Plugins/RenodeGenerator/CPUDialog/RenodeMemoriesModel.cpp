//-----------------------------------------------------------------------------
// File: RenodeMemoriesModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 20.03.2023
//
// Description:
// The model to manage the Renode memory details.
//-----------------------------------------------------------------------------

#include "RenodeMemoriesModel.h"

#include <common/KactusColors.h>

#include <Plugins/RenodeGenerator/CPUDialog/RenodeColumns.h>

#include <QColor>

//-----------------------------------------------------------------------------
// Function: RenodeMemoriesModel::RenodeMemoriesModel()
//-----------------------------------------------------------------------------
RenodeMemoriesModel::RenodeMemoriesModel(QObject *parent):
QAbstractTableModel(parent),
memories_()
{
}

//-----------------------------------------------------------------------------
// Function: RenodeMemoriesModel::setupMemories()
//-----------------------------------------------------------------------------
void RenodeMemoriesModel::setupMemories(QVector<QSharedPointer<RenodeStructs::cpuMemories>> newMemories)
{
    beginResetModel();

    memories_.clear();
    memories_ = newMemories;

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: RenodeMemoriesModel::rowCount()
//-----------------------------------------------------------------------------
int RenodeMemoriesModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

    return memories_.count();
}

//-----------------------------------------------------------------------------
// Function: RenodeMemoriesModel::columnCount()
//-----------------------------------------------------------------------------
int RenodeMemoriesModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

    return MemoryColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: RenodeMemoriesModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags RenodeMemoriesModel::flags(QModelIndex const& index) const
{
	if (!index.isValid() || index.column() == MemoryColumns::NAME || index.column() == MemoryColumns::BASEADDRESS || index.column() == MemoryColumns::SIZE)
    {
		return Qt::NoItemFlags;
	}
    else
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
}

//-----------------------------------------------------------------------------
// Function: RenodeMemoriesModel::headerData()
//-----------------------------------------------------------------------------
QVariant RenodeMemoriesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal)
    {
		return QVariant();
	}

	if (Qt::DisplayRole == role)
    {
        if (section == MemoryColumns::NAME)
        {
            return tr("Memory");
        }
        else if (section == MemoryColumns::CLASS)
        {
            return tr("Class");
        }
        else if (section == MemoryColumns::BASEADDRESS)
        {
            return tr("Base address");
        }
        else if (section == MemoryColumns::SIZE)
        {
            return tr("Size");
        }
	}
    
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: RenodeMemoriesModel::data()
//-----------------------------------------------------------------------------
QVariant RenodeMemoriesModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= memories_.size())
    {
		return QVariant();
	}

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        return valueForIndex(index);
    }
    else if (role == Qt::ToolTipRole)
    {
        return tooltipForIndex(index);
    }
	else if (role == Qt::ForegroundRole)
    {
        if (index.column() == MemoryColumns::NAME || index.column() == MemoryColumns::BASEADDRESS || index.column() == MemoryColumns::SIZE)
        {
            return KactusColors::DISABLED_TEXT;
        }
        else
        {
            return KactusColors::REGULAR_TEXT;
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: RenodeMemoriesModel::setData()
//-----------------------------------------------------------------------------
bool RenodeMemoriesModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= memories_.size())
    {
		return false;
	}

    QSharedPointer<RenodeStructs::cpuMemories> indexedMemory = memories_.at(index.row());
    if (Qt::EditRole == role)
    {
        if (index.column() == MemoryColumns::CLASS)
        {
            indexedMemory->className_ = value.toString();
        }

        emit dataChanged(index, index);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: RenodeMemoriesModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant RenodeMemoriesModel::valueForIndex(QModelIndex const& index) const
{
    QSharedPointer<RenodeStructs::cpuMemories> indexedMemory = memories_.at(index.row());

    if (index.column() == MemoryColumns::NAME)
    {
        return indexedMemory->memoryName_;
    }
    else if (index.column() == MemoryColumns::CLASS)
    {
        return indexedMemory->className_;
    }
    else if (index.column() == MemoryColumns::BASEADDRESS)
    {
        return indexedMemory->baseAddress_;
    }
    else if (index.column() == MemoryColumns::SIZE)
    {
        return indexedMemory->size_;
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: RenodeMemoriesModel::tooltipForIndex()
//-----------------------------------------------------------------------------
QVariant RenodeMemoriesModel::tooltipForIndex(QModelIndex const& index) const
{
    if (index.column() == MemoryColumns::NAME)
    {
        return QString("Name of the memory");
    }
    else if (index.column() == PeripheralColumns::CLASS)
    {
        return QString("The type name.");
    }
    else if (index.column() == MemoryColumns::BASEADDRESS)
    {
        return QString("Base address of the memory as a hexadecimal value.");
    }
    else if (index.column() == MemoryColumns::SIZE)
    {
        return QString("Size of the memory as a hexadecimal value.");
    }

    return QVariant();
}
