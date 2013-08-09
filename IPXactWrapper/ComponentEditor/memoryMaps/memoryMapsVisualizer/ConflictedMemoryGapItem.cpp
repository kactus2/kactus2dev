//-----------------------------------------------------------------------------
// File: ConflictedItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 08.08.2013
//
// Description:
// Graphical item representing an overlapping memory section.
//-----------------------------------------------------------------------------

#include "ConflictedItem.h"
#include <IPXactWrapper/ComponentEditor/visualization/memorygapitem.h>

//-----------------------------------------------------------------------------
// Function: ConflictedItem()
//-----------------------------------------------------------------------------
ConflictedItem::ConflictedItem( QGraphicsItem* parent ):
MemoryGapItem(parent) {

    setConflicted();
    setName("conflicted");
    setBrush(QBrush(QColor(Qt::red)));
    
}

//-----------------------------------------------------------------------------
// Function: ~ConflictedItem()
//-----------------------------------------------------------------------------
ConflictedItem::~ConflictedItem() {
}
