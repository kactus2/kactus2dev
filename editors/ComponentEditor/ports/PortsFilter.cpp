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
#include <KactusAPI/include/PortsInterface.h>

#include <IPXACTmodels/Component/Port.h>

//-----------------------------------------------------------------------------
// Function: PortsFilter::PortsFilter()
//-----------------------------------------------------------------------------
PortsFilter::PortsFilter(QSharedPointer<PortsInterface> portInterface, QObject* parent):
QSortFilterProxyModel(parent),
portInterface_(portInterface)
{

}

//-----------------------------------------------------------------------------
// Function: PortsFilter::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool PortsFilter::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    QModelIndex portNameIndex = sourceModel()->index(source_row, nameColumn(), source_parent);
    if (!portIsAccepted(portNameIndex.data(Qt::DisplayRole).toString()))
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

//-----------------------------------------------------------------------------
// Function: PortsFilter::getInterface()
//-----------------------------------------------------------------------------
QSharedPointer<PortsInterface> PortsFilter::getInterface() const
{
    return portInterface_;
}
