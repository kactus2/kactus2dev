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
AddressSpaceConflictedItem::AddressSpaceConflictedItem(QSharedPointer<AddressSpace> addrSpace,
										 AddressSpaceConflictedItem::AddressPosition addrPos,
										 QGraphicsItem* parent):
AddressSpaceGapItem(addrSpace, addrPos, parent)
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