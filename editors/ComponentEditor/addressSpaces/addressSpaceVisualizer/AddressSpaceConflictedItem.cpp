//-----------------------------------------------------------------------------
// File: AddressSpaceConflictedItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 21.08.2013
//
// Description:
// Overlapping memory item for segments and local memory blocks.
//-----------------------------------------------------------------------------

#include "AddressSpaceConflictedItem.h"

//-----------------------------------------------------------------------------
// Function: AddressSpaceConflictedItem()
//-----------------------------------------------------------------------------
AddressSpaceConflictedItem::AddressSpaceConflictedItem(VisualizerItem::LabelLayout addrPos,
                                                       QString const& addressSpaceWidth,
                                                       QSharedPointer<ExpressionParser> expressionParser,
                                                       QGraphicsItem* parent):
AddressSpaceGapItem(addrPos, addressSpaceWidth, expressionParser, parent)
{
    setConflicted(true);
    setName("Conflicted");
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceConflictedItem::refresh()
//-----------------------------------------------------------------------------
void AddressSpaceConflictedItem::refresh()
{
    AddressSpaceGapItem::refresh();

    setToolTip("<b>Two or more memory blocks are mapped to these addresses.<br>First address: </b>" + 
        addr2Str(getOffset()) + "<br>" +
        "<b>Last address: </b>" + addr2Str(getLastAddress()));
}
