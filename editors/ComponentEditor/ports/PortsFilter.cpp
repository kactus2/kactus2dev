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
// Function: PortsFilter::data()
//-----------------------------------------------------------------------------
QVariant PortsFilter::data(const QModelIndex& index, int role) const
{
    if (index.column() == 0)
    {
        if (role == Qt::DisplayRole)
        {
            return index.row();
        }
        return QVariant();
    }

    return QSortFilterProxyModel::data(index, role);
}

//-----------------------------------------------------------------------------
// Function: PortsFilter::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool PortsFilter::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    if (QModelIndex portNameIndex = sourceModel()->index(source_row, nameColumn(), source_parent);
        portIsAccepted(portNameIndex.data(Qt::DisplayRole).toString()) == false)
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
