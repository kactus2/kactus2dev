//-----------------------------------------------------------------------------
// File: PortMappingTableModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.05.2016
//
// Description:
// The base class for logical table model and physical table model.
//-----------------------------------------------------------------------------

#include "PortMappingTableModel.h"
#include "PortMappingColumns.h"

//-----------------------------------------------------------------------------
// Function: PortMappingTableModel::PortMappingTableModel()
//-----------------------------------------------------------------------------
PortMappingTableModel::PortMappingTableModel(QObject *parent): 
QAbstractTableModel(parent)
{

}

//-----------------------------------------------------------------------------
// Function: PortMappingTableModel::~PortMappingTableModel()
//-----------------------------------------------------------------------------
PortMappingTableModel::~PortMappingTableModel()
{

}

//-----------------------------------------------------------------------------
// Function: PortMappingTableModel::columnCount()
//-----------------------------------------------------------------------------
int PortMappingTableModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    else
    {
        return PortMappingColumns::COLUMN_COUNT;
    }
}

//-----------------------------------------------------------------------------
// Function: PortMappingTableModel::headerData()
//-----------------------------------------------------------------------------
QVariant PortMappingTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal)
    {
        if (role == Qt::DisplayRole)
        {
            if (section == PortMappingColumns::NAME)
            {
                return QString(tr("Name"));
            }
            else if (section == PortMappingColumns::DIRECTION)
            {
                return QString(tr("Direction"));
            }
            else if (section == PortMappingColumns::LEFT_BOUND)
            {
                return QString(tr("Left bound"));
            }
            else if (section == PortMappingColumns::RIGHT_BOUND)
            {
                return QString(tr("Right bound"));
            }
            else if (section == PortMappingColumns::SIZE)
            {
                return QString(tr("Size"));
            }
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortMappingTableModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags PortMappingTableModel::flags(QModelIndex const& index) const
{
    if (!index.isValid() || index.column() >= PortMappingColumns::COLUMN_COUNT)
    {
        return Qt::NoItemFlags;
    }

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    return flags;
}
