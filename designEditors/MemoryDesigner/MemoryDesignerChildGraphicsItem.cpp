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

#include <designEditors/MemoryDesigner/MemoryItem.h>

#include <QBrush>
#include <QFont>

//-----------------------------------------------------------------------------
// Function: MemoryDesignerChildGraphicsItem::MemoryDesignerChildGraphicsItem()
//-----------------------------------------------------------------------------
MemoryDesignerChildGraphicsItem::MemoryDesignerChildGraphicsItem(QSharedPointer<MemoryItem> memoryItem,
    QString const& toolTipType, quint64 baseAddress, quint64 range, qreal blockWidth, QGraphicsItem* parent):
MemoryDesignerGraphicsItem(memoryItem->getName(), parent)
{
    if (range == 1)
    {
        getRangeStartLabel()->hide();
    }

    setGraphicsRectangle(blockWidth, range);

    quint64 rangeEnd = baseAddress + range - 1;
    setupToolTip(toolTipType, baseAddress, rangeEnd);

    qreal rangeStartLabelWidth = getRangeStartLabel()->boundingRect().width();
    qreal nameLabelWidth = getNameLabel()->boundingRect().width();
    qreal itemBoundingWidth = boundingRect().width();

    if (((getNameLabel()->collidesWithItem(getRangeStartLabel()) ||
        getNameLabel()->collidesWithItem(getRangeEndLabel()))
        && rangeStartLabelWidth + nameLabelWidth > itemBoundingWidth) || nameLabelWidth > itemBoundingWidth)
    {
        QString nameText = getNameLabel()->toPlainText();
        nameText = nameText.left(nameText.size() - 3);
        nameText.append("...");

        getNameLabel()->setPlainText(nameText);
        nameLabelWidth = getNameLabel()->boundingRect().width();

        while (rangeStartLabelWidth + nameLabelWidth > itemBoundingWidth && nameText != "...")
        {
            nameText = nameText.left(nameText.size() - 4);
            nameText.append("...");
            getNameLabel()->setPlainText(nameText);
            nameLabelWidth = getNameLabel()->boundingRect().width();
        }
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
        getRangeStartLabel()->setDefaultTextColor(emptyTextColor);
        getRangeEndLabel()->setDefaultTextColor(emptyTextColor);
    }
    setBrush(itemBrush);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerChildGraphicsItem::changeAddressRange()
//-----------------------------------------------------------------------------
void MemoryDesignerChildGraphicsItem::changeAddressRange(quint64 offset)
{
    quint64 newBaseAddress = getBaseAddress() + offset;
    getRangeStartLabel()->setPlainText(getValueFormattedToHexadecimal(newBaseAddress));

    quint64 newLastAddress = getLastAddress() + offset;
    getRangeEndLabel()->setPlainText(getValueFormattedToHexadecimal(newLastAddress));
}
