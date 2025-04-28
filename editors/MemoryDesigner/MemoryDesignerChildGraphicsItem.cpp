//-----------------------------------------------------------------------------
// File: MemoryDesignerChildGraphicsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 19.09.2016
//
// Description:
// Parent item for visualizing memory child items i.e. address blocks and segments.
//-----------------------------------------------------------------------------

#include "MemoryDesignerChildGraphicsItem.h"

#include <editors/MemoryDesigner/MemoryItem.h>

#include <QBrush>
#include <QFont>

//-----------------------------------------------------------------------------
// Function: MemoryDesignerChildGraphicsItem::MemoryDesignerChildGraphicsItem()
//-----------------------------------------------------------------------------
MemoryDesignerChildGraphicsItem::MemoryDesignerChildGraphicsItem(QSharedPointer<MemoryItem const> subMemoryItem,
    QString const& toolTipType, quint64 baseAddress, quint64 range, qreal blockWidth,
    QVector<QString> identifierChain, QSharedPointer<ConnectivityComponent const> containingInstance,
    QGraphicsItem* parent):
MemoryDesignerGraphicsItem(subMemoryItem->getName(), subMemoryItem->getDisplayName(), identifierChain,
    containingInstance, parent),
tooltipType_(toolTipType),
blockWidth_(blockWidth)
{
    quint64 rangeEnd = baseAddress + range - 1;

    setup(range, baseAddress, rangeEnd);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerChildGraphicsItem::MemoryDesignerChildGraphicsItem()
//-----------------------------------------------------------------------------
MemoryDesignerChildGraphicsItem::MemoryDesignerChildGraphicsItem(MemoryDesignerChildGraphicsItem const& other, QGraphicsItem* parentItem):
MemoryDesignerGraphicsItem(other, parentItem),
tooltipType_(other.tooltipType_),
blockWidth_(other.blockWidth_)
{
    quint64 lastAddress = other.getOriginalLastAddress();
    quint64 baseAddress = other.getOriginalBaseAddress();

    quint64 range = lastAddress - baseAddress + 1;
    setup(range, baseAddress, lastAddress);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerChildGraphicsItem::setup()
//-----------------------------------------------------------------------------
void MemoryDesignerChildGraphicsItem::setup(quint64 const& range, quint64 const& baseAddress, quint64 const& lastAddress)
{
    if (range == 1)
    {
        getRangeStartLabel()->hide();
    }

    setGraphicsRectangle(blockWidth_, range);

    if (tooltipType_.compare(QStringLiteral("Field")) != 0)
    {
        setupGraphicsItem(baseAddress, lastAddress, tooltipType_);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerChildGraphicsItem::~MemoryDesignerChildGraphicsItem()
//-----------------------------------------------------------------------------
MemoryDesignerChildGraphicsItem::~MemoryDesignerChildGraphicsItem()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerChildGraphicsItem::setColors()
//-----------------------------------------------------------------------------
void MemoryDesignerChildGraphicsItem::setColors(QColor itemColor, bool itemIsEmpty)
{
    QBrush itemBrush(itemColor);

    if (itemIsEmpty)
    {
        itemBrush.setColor(QColor("whiteSmoke"));

        QColor emptyTextColor (180, 180, 180);
        getNameLabel()->setDefaultTextColor(emptyTextColor);
        setEmptyItemRangeColors(emptyTextColor);
    }
    setBrush(itemBrush);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerChildGraphicsItem::setEmptyItemRangeColors()
//-----------------------------------------------------------------------------
void MemoryDesignerChildGraphicsItem::setEmptyItemRangeColors(QColor emptyItemRangeColour)
{
    getRangeStartLabel()->setDefaultTextColor(emptyItemRangeColour);
    getRangeEndLabel()->setDefaultTextColor(emptyItemRangeColour);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerChildGraphicsItem::fitNameLabel()
//-----------------------------------------------------------------------------
void MemoryDesignerChildGraphicsItem::fitNameLabel()
{
    fitLabel(getNameLabel());
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerChildGraphicsItem::getItemWidth()
//-----------------------------------------------------------------------------
qreal MemoryDesignerChildGraphicsItem::getItemWidth() const
{
    return boundingRect().width();
}
