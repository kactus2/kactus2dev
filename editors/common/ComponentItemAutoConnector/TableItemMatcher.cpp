//-----------------------------------------------------------------------------
// File: TableItemMatcher.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 25.02.2019
//
// Description:
// Automatically defines possible matches between two items.
//-----------------------------------------------------------------------------

#include "TableItemMatcher.h"

//-----------------------------------------------------------------------------
// Function: TableItemMatcher::canDrop()
//-----------------------------------------------------------------------------
bool TableItemMatcher::canDrop(QModelIndex const& sourceIndex, QModelIndex const& targetIndex,
    QSharedPointer<Component> sourceComponent, QSharedPointer<Component> targetComponent) const
{
    QString sourceItem = sourceIndex.data(Qt::DisplayRole).toString();
    QString targetItem = targetIndex.data(Qt::DisplayRole).toString();

    return targetItem.isEmpty() || itemsCanBeConnected(sourceItem, sourceComponent, targetItem, targetComponent);
}
