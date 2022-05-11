//-----------------------------------------------------------------------------
// File: MemoryBlockFilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.02.2022
//
// Description:
// Sorting proxy model for memory blocks.
//-----------------------------------------------------------------------------

#include "MemoryBlockFilter.h"

#include <KactusAPI/include/MemoryBlockInterface.h>

//-----------------------------------------------------------------------------
// Function: PortsFilter::PortsFilter()
//-----------------------------------------------------------------------------
MemoryBlockFilter::MemoryBlockFilter(QSharedPointer<ExpressionParser> parser, MemoryBlockInterface* blockInterface,
    QObject* parent):
ExpressionProxyModel(parser, parent),
blockInterface_(blockInterface)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryBlockFilter::filterAcceptsRow()
//-----------------------------------------------------------------------------
bool MemoryBlockFilter::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    std::string blockName = blockInterface_->getIndexedItemName(source_row);
    if (!blockInterface_->acceptBlock(blockName))
    {
        return false;
    }

    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}
