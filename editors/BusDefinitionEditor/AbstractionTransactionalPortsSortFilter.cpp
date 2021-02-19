//-----------------------------------------------------------------------------
// File: AbstractionTransactionalPortsSortFilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.06.2018
//
// Description:
// Sort filter proxy model for abstraction transactional ports.
//-----------------------------------------------------------------------------

#include "AbstractionTransactionalPortsSortFilter.h"

#include <common/KactusColors.h>

#include <editors/BusDefinitionEditor/AbstractionTransactionalPortColumns.h>
#include <editors/BusDefinitionEditor/interfaces/PortAbstractionInterface.h>

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsSortFilter::AbstractionTransactionalPortsSortFilter()
//-----------------------------------------------------------------------------
AbstractionTransactionalPortsSortFilter::AbstractionTransactionalPortsSortFilter(
    AbstractionDefinitionPortsSortFilter::ColumnHandles columns, PortAbstractionInterface* portInterface,
    QObject *parent):
AbstractionDefinitionPortsSortFilter(columns, portInterface, parent)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsSortFilter::getBackgroundColorForIndex()
//-----------------------------------------------------------------------------
QColor AbstractionTransactionalPortsSortFilter::getBackgroundColorForIndex(QModelIndex const& index) const
{
    if (index.column() == AbstractionTransactionalPortColumns::PROTOCOLTYPE && indexedRowContainsPayload(index))
    {
        return KactusColors::MANDATORY_FIELD;
    }
    else if (index.column() == AbstractionTransactionalPortColumns::PAYLOADTYPE &&
        indexedRowContainsPayload(index))
    {
        return KactusColors::MANDATORY_FIELD;
    }
    
    return AbstractionDefinitionPortsSortFilter::getBackgroundColorForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsSortFilter::indexedRowContainsPayload()
//-----------------------------------------------------------------------------
bool AbstractionTransactionalPortsSortFilter::indexedRowContainsPayload(QModelIndex const& index) const
{
    QModelIndex payloadNameIndex = index.sibling(index.row(), AbstractionTransactionalPortColumns::PAYLOADNAME);
    QModelIndex payloadTypeIndex = index.sibling(index.row(), AbstractionTransactionalPortColumns::PAYLOADTYPE);
    QModelIndex payloadExtensionIndex =
        index.sibling(index.row(), AbstractionTransactionalPortColumns::PAYLOADEXTENSION);

    return !payloadNameIndex.data(Qt::DisplayRole).toString().isEmpty() ||
        !payloadTypeIndex.data(Qt::DisplayRole).toString().isEmpty() ||
        !payloadExtensionIndex.data(Qt::DisplayRole).toString().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTransactionalPortsSortFilter::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool AbstractionTransactionalPortsSortFilter::filterAcceptsRow(int source_row, const QModelIndex &source_parent)
const
{
    if (source_parent.isValid())
    {
        return false;
    }

    PortAbstractionInterface* portInterface = getPortInterface();
    std::string portName = portInterface->getIndexedItemName(source_row);
    return portInterface->portIsTransactional(portName);
}
