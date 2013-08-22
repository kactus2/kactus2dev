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
	setNamePosition(VisualizerItem::NAME_CENTERED, VisualizerItem::NAME_MIDDLE);
	setName("Conflicted");
}

//-----------------------------------------------------------------------------
// Function: ~AddressSpaceConflictedItem()
//-----------------------------------------------------------------------------
AddressSpaceConflictedItem::~AddressSpaceConflictedItem() {
}