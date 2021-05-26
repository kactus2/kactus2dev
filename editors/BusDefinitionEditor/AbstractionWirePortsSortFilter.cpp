//-----------------------------------------------------------------------------
// File: AbstractionWirePortsSortFilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 27.11.2019
//
// Description:
// Sort filter proxy model for abstraction definition wire ports.
//-----------------------------------------------------------------------------

#include "AbstractionWirePortsSortFilter.h"

#include <editors/BusDefinitionEditor/LogicalPortColumns.h>

#include <editors/BusDefinitionEditor/AbstractionPortsModel.h>
#include <editors/BusDefinitionEditor/interfaces/PortAbstractionInterface.h>

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsSortFilter::AbstractionWirePortsSortFilter()
//-----------------------------------------------------------------------------
AbstractionWirePortsSortFilter::AbstractionWirePortsSortFilter(PortAbstractionInterface* portInterface,
    QObject *parent):
    AbstractionDefinitionPortsSortFilter(portInterface, parent)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionWirePortsSortFilter::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool AbstractionWirePortsSortFilter::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (source_parent.isValid())
    {
        return false;
    }

    std::string portName = portInterface_->getIndexedItemName(source_row);
    return portInterface_->portIsWire(portName);
}
