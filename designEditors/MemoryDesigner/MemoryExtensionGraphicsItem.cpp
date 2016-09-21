//-----------------------------------------------------------------------------
// File: MemoryExtensionGraphicsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.09.2016
//
// Description:
// Extension item for memory designer graphics items.
//-----------------------------------------------------------------------------

#include "MemoryExtensionGraphicsItem.h"

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <designEditors/MemoryDesigner/AddressSpaceGraphicsItem.h>
#include <designEditors/MemoryDesigner/MemoryMapGraphicsItem.h>

#include <QPen>
#include <QBrush>

//-----------------------------------------------------------------------------
// Function: MemoryExtensionGraphicsItem::MemoryExtensionGraphicsItem()
//-----------------------------------------------------------------------------
MemoryExtensionGraphicsItem::MemoryExtensionGraphicsItem(qreal positionX, qreal positionY, qreal extensionWidth,
    qreal extensionHeight, QGraphicsItem* parentItem):
MemoryDesignerGraphicsItem("", parentItem)
{
    setRect(positionX, positionY, extensionWidth, extensionHeight);
    setPenAndBrush();
    setLabelPositions();
}

//-----------------------------------------------------------------------------
// Function: MemoryExtensionGraphicsItem::~MemoryExtensionGraphicsItem()
//-----------------------------------------------------------------------------
MemoryExtensionGraphicsItem::~MemoryExtensionGraphicsItem()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryExtensionGraphicsItem::type()
//-----------------------------------------------------------------------------
int MemoryExtensionGraphicsItem::type() const
{
    int extensionType = GFX_TYPE_MEMORY_ITEM;

    QGraphicsItem* parent = parentItem();
    AddressSpaceGraphicsItem* spaceParent = dynamic_cast<AddressSpaceGraphicsItem*>(parent);
    if (spaceParent)
    {
        extensionType = spaceParent->type();
    }
    else
    {
        MemoryMapGraphicsItem* mapParent = dynamic_cast<MemoryMapGraphicsItem*>(parent);
        if (mapParent)
        {
            extensionType = mapParent->type();
        }
    }

    return extensionType;
}

//-----------------------------------------------------------------------------
// Function: MemoryExtensionGraphicsItem::setPenAndBrush()
//-----------------------------------------------------------------------------
void MemoryExtensionGraphicsItem::setPenAndBrush()
{
    QPen extensionPen = pen();
    extensionPen.setStyle(Qt::DashLine);
    setPen(extensionPen);

    QBrush extensionBrush(QColor("whiteSmoke"));
    setBrush(extensionBrush);
}

//-----------------------------------------------------------------------------
// Function: MemoryExtensionGraphicsItem::setLabelPositions()
//-----------------------------------------------------------------------------
void MemoryExtensionGraphicsItem::setLabelPositions()
{
    hideMemoryRangeLabels();
}
