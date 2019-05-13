//-----------------------------------------------------------------------------
// File: PortsFilter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 10.05.2019
//
// Description:
// Sorting proxy model for ports.
//-----------------------------------------------------------------------------

#include "PortsFilter.h"

#include <editors/ComponentEditor/ports/portsmodel.h>

#include <IPXACTmodels/Component/Port.h>

//-----------------------------------------------------------------------------
// Function: PortsFilter::PortsFilter()
//-----------------------------------------------------------------------------
PortsFilter::PortsFilter(QObject* parent):
QSortFilterProxyModel(parent)
{

}

//-----------------------------------------------------------------------------
// Function: PortsFilter::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool PortsFilter::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    QModelIndex portIndex = sourceModel()->index(source_row, 0, source_parent);
    QVariant portData = sourceModel()->data(portIndex, PortsModel::portRoles::getPortRole);
    
    QSharedPointer<Port> port = portData.value<QSharedPointer<Port> >();
    if (port && !portIsAccepted(port))
    {
        return false;
    }

    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

//-----------------------------------------------------------------------------
// Function: PortsFilter::onHandleExtensionsEditorItemChange()
//-----------------------------------------------------------------------------
void PortsFilter::onHandleExtensionsEditorItemChange(QModelIndex const& index)
{
    QModelIndex filteredIndex = mapToSource(index);
    emit changeExtensionsEditorItem(filteredIndex);
}
