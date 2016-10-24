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

#include <QBrush>
#include <QFont>

//-----------------------------------------------------------------------------
// Function: MemoryDesignerChildGraphicsItem::MemoryDesignerChildGraphicsItem()
//-----------------------------------------------------------------------------
MemoryDesignerChildGraphicsItem::MemoryDesignerChildGraphicsItem(QString const& itemName,
    QString const& toolTipType, quint64 baseAddress, quint64 range, qreal blockWidth, QGraphicsItem* parent):
MemoryDesignerGraphicsItem(itemName, parent)
{
    if (range == 1)
    {
        getRangeStartLabel()->hide();
    }

    setGraphicsRectangle(blockWidth, range);

    quint64 rangeEnd = baseAddress + range - 1;
    setupToolTip(toolTipType, baseAddress, rangeEnd);
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

//-----------------------------------------------------------------------------
// Function: MemoryDesignerChildGraphicsItem::fitNameLabel()
//-----------------------------------------------------------------------------
void MemoryDesignerChildGraphicsItem::fitNameLabel()
{
    qreal nameLabelWidth = getNameLabel()->boundingRect().width();
    qreal itemBoundingWidth = getItemWidth();

    bool nameCollidesWithRange = getNameLabel()->collidesWithItem(getRangeStartLabel()) ||
        getNameLabel()->collidesWithItem(getRangeEndLabel());

    if (nameCollidesWithRange || nameLabelWidth > itemBoundingWidth)
    {
        qreal rangeStartLabelWidth = 0;
        if (nameCollidesWithRange)
        {
            rangeStartLabelWidth = getRangeStartLabel()->boundingRect().width();
        }

        qreal originalNameWidth = nameLabelWidth;

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

        qreal widthDifference = originalNameWidth - nameLabelWidth;
        getNameLabel()->setPos(getNameLabel()->pos().x() + widthDifference, getNameLabel()->pos().y());
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerChildGraphicsItem::getItemWidth()
//-----------------------------------------------------------------------------
qreal MemoryDesignerChildGraphicsItem::getItemWidth() const
{
    return boundingRect().width();
}
