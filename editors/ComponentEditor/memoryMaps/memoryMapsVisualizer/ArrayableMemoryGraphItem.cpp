//-----------------------------------------------------------------------------
// File: ArrayableMemoryGraphItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 24.06.2025
//
// Description:
// Represents a visualization of a memory item that can be expressed as an array of identical items.
// Replicas of the item are identified with their index. 
//-----------------------------------------------------------------------------

#include "ArrayableMemoryGraphItem.h"

#include <QObject>

//-----------------------------------------------------------------------------
// Function: ArrayableMemoryGraphItem::ArrayableMemoryGraphItem()
//-----------------------------------------------------------------------------
ArrayableMemoryGraphItem::ArrayableMemoryGraphItem(QSharedPointer<ExpressionParser> expressionParser, 
    QGraphicsItem* parent /*= nullptr*/) :
MemoryVisualizationItem(expressionParser, parent)
{

}

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

//-----------------------------------------------------------------------------
// Function: ArrayableMemoryGraphItem::getOffset()
//-----------------------------------------------------------------------------
quint64 ArrayableMemoryGraphItem::getOffset() const
{
    return offset_;
}

//-----------------------------------------------------------------------------
// Function: ArrayableMemoryGraphItem::setOffset()
//-----------------------------------------------------------------------------
void ArrayableMemoryGraphItem::setOffset(quint64 newOffset)
{
    offset_ = newOffset;
}
