//-----------------------------------------------------------------------------
// File: SegmentItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 01.11.2012
//
// Description:
// Segment graphics item.
//-----------------------------------------------------------------------------

#include "SegmentItem.h"

#include <models/component.h>
#include <models/addressspace.h>

//-----------------------------------------------------------------------------
// Function: SegmentItem::SegmentItem()
//-----------------------------------------------------------------------------
SegmentItem::SegmentItem(QSharedPointer<Component> component,
                         QSharedPointer<AddressSpace> addressSpace,
                         QString const& name, unsigned int startAddress,
                         unsigned int range, QGraphicsItem* parent)
    : AddressSectionItem(name, startAddress, range, parent),
      component_(component),
      addressSpace_(addressSpace)
{
}

//-----------------------------------------------------------------------------
// Function: SegmentItem::~SegmentItem()
//-----------------------------------------------------------------------------
SegmentItem::~SegmentItem()
{
}
