//-----------------------------------------------------------------------------
// File: AddressSpaceConflictedItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 21.08.2013
//
// Description:
// Overlapping memory item for segments and local memory blocks.
//-----------------------------------------------------------------------------

#include "AddressSpaceConflictedItem.h"
#include <common/KactusColors.h>

#include <QBrush>

//-----------------------------------------------------------------------------
// Function: AddressSpaceConflictedItem()
//-----------------------------------------------------------------------------
AddressSpaceConflictedItem::AddressSpaceConflictedItem(
                                         AddressSpaceConflictedItem::AddressPosition addrPos,
                                         QString const& addressSpaceWidth, 
                                         QSharedPointer<ExpressionParser> expressionParser,
										 QGraphicsItem* parent):
AddressSpaceGapItem(addrPos, addressSpaceWidth, expressionParser, parent)
{
    //QBrush brush(KactusColors::DRAFT_COMPONENT);
    //setDefaultBrush(QBrush());
    setConflicted(true);
    setName("Conflicted");
}

//-----------------------------------------------------------------------------
// Function: ~AddressSpaceConflictedItem()
//-----------------------------------------------------------------------------
AddressSpaceConflictedItem::~AddressSpaceConflictedItem() {
}

//-----------------------------------------------------------------------------
// Function: ~refresh()
//-----------------------------------------------------------------------------
void AddressSpaceConflictedItem::refresh()
{
    AddressSpaceGapItem::refresh();

    // Set tooltip to show addresses in hexadecimals.
    setToolTip("<b>Two or more memory blocks are mapped to these addresses.<br>First address: </b>" + 
        addr2Str(getOffset(), getBitWidth()) + "<br>" +
        "<b>Last address: </b>" + addr2Str(getLastAddress(),getBitWidth()));
}