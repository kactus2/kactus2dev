//-----------------------------------------------------------------------------
// File: PortMapTreeSortProxyModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 30.05.2016
//
// Description:
// Sorting proxy model for bus interface port maps.
//-----------------------------------------------------------------------------

#include "PortMapTreeSortProxyModel.h"

#include <editors/ComponentEditor/busInterfaces/portmaps/PortMapsColumns.h>

#include <IPXACTmodels/common/PresenceTypes.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <QModelIndex>

//-----------------------------------------------------------------------------
// Function: PortMapTreeSortProxyModel::PortMapTreeSortProxyModel()
//-----------------------------------------------------------------------------
PortMapTreeSortProxyModel::PortMapTreeSortProxyModel(QSharedPointer<BusInterface> busInterface, QObject *parent):
QSortFilterProxyModel(parent),
containingBus_(busInterface)
{

}

//-----------------------------------------------------------------------------
// Function: PortMapTreeSortProxyModel::~PortMapTreeSortProxyModel()
//-----------------------------------------------------------------------------
PortMapTreeSortProxyModel::~PortMapTreeSortProxyModel()
{

}

//-----------------------------------------------------------------------------
// Function: PortMapTreeSortProxyModel::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool PortMapTreeSortProxyModel::filterAcceptsRow(int source_row, const QModelIndex & source_parent) const
{
    if (!source_parent.isValid())
    {
        QModelIndex index = sourceModel()->index(source_row, PortMapsColumns::LOGICAL_PORT);

        bool definitionHasPort = sourceModel()->data(index, Qt::UserRole + 1).toBool();
        if (!definitionHasPort)
        {
            if (!sourceModel()->hasChildren(index))
            {
                return false;
            }
        }
    }

    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeSortProxyModel::lessThan()
//-----------------------------------------------------------------------------
bool PortMapTreeSortProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (indexIsValid(left) && indexIsValid(right) && !left.parent().isValid() && !right.parent().isValid())
    {
        QModelIndex leftPresenceIndex = left.sibling(left.row(), PortMapsColumns::LOGICAL_PRESENCE);
        QModelIndex rightPresenceIndex = right.sibling(right.row(), PortMapsColumns::LOGICAL_PRESENCE);

        PresenceTypes::Presence leftPresence = PresenceTypes::str2Presence(
            leftPresenceIndex.data(Qt::DisplayRole).toString(), PresenceTypes::UNKNOWN);
        PresenceTypes::Presence rightPresence = PresenceTypes::str2Presence(
            rightPresenceIndex.data(Qt::DisplayRole).toString(), PresenceTypes::UNKNOWN);

        if (leftPresence == PresenceTypes::ILLEGAL && rightPresence != PresenceTypes::ILLEGAL)
        {
            return sortOrder() == Qt::DescendingOrder;
        }
        else if (rightPresence == PresenceTypes::ILLEGAL && leftPresence != PresenceTypes::ILLEGAL)
        {
            return sortOrder() == Qt::AscendingOrder;
        }
    }

    return QSortFilterProxyModel::lessThan(left, right);
}

//-----------------------------------------------------------------------------
// Function: PortMapTreeSortProxyModel::indexIsValid()
//-----------------------------------------------------------------------------
bool PortMapTreeSortProxyModel::indexIsValid(QModelIndex const& index) const
{
    if (index.isValid() && index.column() < PortMapsColumns::COLUMN_COUNT && 0 <= index.row() &&
        index.row() < sourceModel()->rowCount())
    {
        return true;
    }
    else
    {
        return false;
    }
}
