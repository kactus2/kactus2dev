//-----------------------------------------------------------------------------
// File: ArrayableMemoryGraphItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 24.06.2025
//
// Description:
// Describes a memory item that can be expressed as an array of identical items.
// Replicas of the item are identified with their index. 
//-----------------------------------------------------------------------------

#include "ArrayableMemoryGraphItem.h"

//-----------------------------------------------------------------------------
// Function: ArrayableMemoryGraphItem::setReplicaIndex()
//-----------------------------------------------------------------------------
void ArrayableMemoryGraphItem::setReplicaIndex(qint64 newIndex)
{
    replicaIndex_ = newIndex;
}

//-----------------------------------------------------------------------------
// Function: ArrayableMemoryGraphItem::getReplicaIndex()
//-----------------------------------------------------------------------------
qint64 ArrayableMemoryGraphItem::getReplicaIndex() const
{
    return replicaIndex_;
}
