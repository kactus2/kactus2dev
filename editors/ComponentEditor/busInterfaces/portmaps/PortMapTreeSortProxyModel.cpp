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
