//-----------------------------------------------------------------------------
// File: MemoryBaseItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.10.2012
//
// Description:
// Memory base item class.
//-----------------------------------------------------------------------------

#include "MemoryBaseItem.h"

//-----------------------------------------------------------------------------
// Function: MemoryBaseItem::MemoryBaseItem()
//-----------------------------------------------------------------------------
MemoryBaseItem::MemoryBaseItem(QGraphicsItem* parent) : QGraphicsRectItem(parent)
{
}

//-----------------------------------------------------------------------------
// Function: MemoryBaseItem::~MemoryBaseItem()
//-----------------------------------------------------------------------------
MemoryBaseItem::~MemoryBaseItem()
{
}

//-----------------------------------------------------------------------------
// Function: MemoryBaseItem::drawGuides()
//-----------------------------------------------------------------------------
void MemoryBaseItem::drawGuides(QPainter*, QRectF const&) const
{
}
