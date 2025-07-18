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

#include <QObject>

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

//-----------------------------------------------------------------------------
// Function: ArrayableMemoryGraphItem::getReplicaName()
//-----------------------------------------------------------------------------
QString ArrayableMemoryGraphItem::getReplicaName(QString const& originalName) const
{
    if (replicaIndex_ != -1)
    {
        return QObject::tr("%1 (%2)").arg(originalName).arg(QString::number(replicaIndex_));
    }
    else
    {
        return originalName;
    }
}
