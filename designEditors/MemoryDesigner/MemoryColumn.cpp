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

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/layouts/VStaticLayout.h>
#include <common/layouts/VStackedLayout.h>
#include <common/layouts/VCollisionLayout.h>
#include <common/layouts/IVGraphicsLayout.h>

#include <designEditors/MemoryDesigner/AddressSpaceGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryMapGraphicsItem.h>
#include <designEditors/MemoryDesigner/MainMemoryGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryConnectionItem.h>

#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: MemoryColumn()
//-----------------------------------------------------------------------------
MemoryColumn::MemoryColumn(QSharedPointer<ColumnDesc> desc, GraphicsColumnLayout* layout, int itemSpacing):
GraphicsColumn(desc, layout, false)
{
    setItemLayout(
        QSharedPointer<IVGraphicsLayout<QGraphicsItem> > (new VCollisionLayout<QGraphicsItem>(itemSpacing)));
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
    if (allowedItems & ColumnTypes::MEMORY &&
        (item->type() == AddressSpaceGraphicsItem::Type || item->type() == MemoryMapGraphicsItem::Type))
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumn::findItemAt()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryColumn::findItemAt(int y) const
{
    foreach (QGraphicsItem* item, getItems())
    {
        if (y >= item->sceneBoundingRect().top() && y < item->sceneBoundingRect().bottom())
        {
            return static_cast<MainMemoryGraphicsItem*>(item);
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumn::findItemAt()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryColumn::findGraphicsItemAt(int itemYPosition) const
{
    foreach (QGraphicsItem* item, getItems())
    {
        if (itemYPosition >= item->sceneBoundingRect().top() && itemYPosition < item->sceneBoundingRect().bottom())
        {
            return dynamic_cast<MainMemoryGraphicsItem*>(item);
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: MemoryColumn::findGraphicsItemByName()
//-----------------------------------------------------------------------------
MainMemoryGraphicsItem* MemoryColumn::findGraphicsItemByName(QString const& itemName,
    QString const& instanceName) const
{
    foreach (QGraphicsItem* item, getItems())
    {
        MainMemoryGraphicsItem* memoryItem = dynamic_cast<MainMemoryGraphicsItem*>(item);
        if (memoryItem && memoryItem->name().compare(itemName, Qt::CaseInsensitive) == 0 &&
            memoryItem->getContainingInstanceName().compare(instanceName, Qt::CaseInsensitive) == 0)
        {
            return memoryItem;
        }
    }

    return 0;
}
