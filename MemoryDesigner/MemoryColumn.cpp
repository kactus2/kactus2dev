//-----------------------------------------------------------------------------
// File: MemoryColumn.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.10.2012
//
// Description:
// Memory column.
//-----------------------------------------------------------------------------

#include "MemoryColumn.h"

#include "MemoryItem.h"
#include "AddressSpaceItem.h"

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/layouts/VStaticLayout.h>
#include <common/layouts/VStackedLayout.h>

#include <models/component.h>

//-----------------------------------------------------------------------------
// Function: MemoryColumn()
//-----------------------------------------------------------------------------
MemoryColumn::MemoryColumn(ColumnDesc const& desc, GraphicsColumnLayout* layout)
    : GraphicsColumn(desc, layout)
{
    if (desc.getContentType() == COLUMN_CONTENT_BUSES)
    {
        setItemLayout(QSharedPointer< IVGraphicsLayout<QGraphicsItem> >(new VStackedLayout<QGraphicsItem>(10.0)));
    }
    else
    {
        setItemLayout(QSharedPointer< IVGraphicsLayout<QGraphicsItem> >(new VStaticLayout<QGraphicsItem>(10.0)));
    }
}

//-----------------------------------------------------------------------------
// Function: ~MemoryColumn()
//-----------------------------------------------------------------------------
MemoryColumn::~MemoryColumn()
{
}

//-----------------------------------------------------------------------------
// Function: MemoryColumn::isItemAllowed()
//-----------------------------------------------------------------------------
bool MemoryColumn::isItemAllowed(QGraphicsItem* item, unsigned int allowedItems) const
{
    switch (item->type())
    {
    case MemoryItem::Type:
        {
            return (allowedItems & CIT_BRIDGE);
        }

    case AddressSpaceItem::Type:
        {
            // If the item is in the allowed items, make sure that it does not intersect any other items.
            if (allowedItems & CIT_COMPONENT)
            {
                foreach (QGraphicsItem* otherItem, getItems())
                {
                    if (otherItem != item && otherItem->type() == AddressSpaceItem::Type)
                    {
                        QRectF rect1 = otherItem->sceneBoundingRect().adjusted(0.0, -2.0, 0.0, 2.0);
                        QRectF rect2 = item->sceneBoundingRect().adjusted(0.0, -2.0, 0.0, 2.0);

                        if (!(rect1.bottom() < rect2.top() || rect1.top() > rect2.bottom()))
                        {
                            return false;
                        }
                    }
                }

                return true;
            }

            return false;
        }

    default:
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryColumn::findItemAt()
//-----------------------------------------------------------------------------
MemoryBaseItem* MemoryColumn::findItemAt(int y) const
{
    foreach (QGraphicsItem* item, getItems())
    {
        if (y >= item->sceneBoundingRect().top() && y < item->sceneBoundingRect().bottom())
        {
            return static_cast<MemoryBaseItem*>(item);
        }
    }

    return 0;
}
