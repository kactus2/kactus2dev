//-----------------------------------------------------------------------------
// File: MemoryDesignerGraphicsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 10.08.2016
//
// Description:
// Parent class for memory designer graphics items.
//-----------------------------------------------------------------------------

#include "MemoryDesignerGraphicsItem.h"

#include <designEditors/common/diagramgrid.h>

#include <designEditors/MemoryDesigner/MemoryColumn.h>
#include <designEditors/MemoryDesigner/MemoryConnectionItem.h>
#include <designEditors/MemoryDesigner/MemoryCollisionItem.h>
#include <designEditors/MemoryDesigner/MemoryDesignerConstants.h>

#include <QFont>
#include <QGraphicsSceneMouseEvent>

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::MemoryDesignerGraphicsItem()
//-----------------------------------------------------------------------------
MemoryDesignerGraphicsItem::MemoryDesignerGraphicsItem(QString const& itemName, QGraphicsItem* parent):
QGraphicsRectItem(parent),
nameLabel_(new QGraphicsTextItem(itemName, this)),
startRangeLabel_(new QGraphicsTextItem("", this)),
endRangeLabel_(new QGraphicsTextItem("", this)),
itemName_(itemName)
{
    QFont labelFont = nameLabel_->font();
    labelFont.setWeight(QFont::Bold);

    nameLabel_->setFont(labelFont);
    startRangeLabel_->setDefaultTextColor(QColor(80, 80, 80));
    endRangeLabel_->setDefaultTextColor(QColor(80, 80, 80));
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::~MemoryDesignerGraphicsItem()
//-----------------------------------------------------------------------------
MemoryDesignerGraphicsItem::~MemoryDesignerGraphicsItem()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::hideMemoryRangeLabels()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::hideMemoryRangeLabels()
{
    startRangeLabel_->hide();
    endRangeLabel_->hide();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::setGraphicsRectangle()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::setGraphicsRectangle(qreal rectangleWidth, qreal rectangleHeight)
{
    setRect(
        QRectF(-rectangleWidth / 2, 0, rectangleWidth, rectangleHeight * MemoryDesignerConstants::RANGEINTERVAL));
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::getNameLabel()
//-----------------------------------------------------------------------------
QGraphicsTextItem* MemoryDesignerGraphicsItem::getNameLabel() const
{
    return nameLabel_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::getRangeStartLabel()
//-----------------------------------------------------------------------------
QGraphicsTextItem* MemoryDesignerGraphicsItem::getRangeStartLabel() const
{
    return startRangeLabel_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::getRangeEndLabel()
//-----------------------------------------------------------------------------
QGraphicsTextItem* MemoryDesignerGraphicsItem::getRangeEndLabel() const
{
    return endRangeLabel_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::setupToolTip()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::setupToolTip(QString const& identifier, quint64 memoryStart, quint64 memoryEnd)
{
    QString memoryStartInHexa = getValueFormattedToHexadecimal(memoryStart);
    QString memoryEndInHexa = getValueFormattedToHexadecimal(memoryEnd);

    QString toolTip =
        "Generic ranges of " + identifier + " " + itemName_ + ":\n" + memoryStartInHexa + "\n" + memoryEndInHexa;
    setToolTip(toolTip);

    startRangeLabel_->setPlainText(memoryStartInHexa);
    endRangeLabel_->setPlainText(memoryEndInHexa);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::getFormattedRangeValue()
//-----------------------------------------------------------------------------
QString MemoryDesignerGraphicsItem::getValueFormattedToHexadecimal(quint64 range) const
{
    QString formattedRange = QString::number(range, 16).toUpper();

    int rangeSize = formattedRange.size();
    while (rangeSize % 4)
    {
        formattedRange.prepend('0');
        rangeSize++;
    }

    return formattedRange;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::name()
//-----------------------------------------------------------------------------
QString MemoryDesignerGraphicsItem::name() const
{
    return itemName_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::getBaseAddress()
//-----------------------------------------------------------------------------
quint64 MemoryDesignerGraphicsItem::getBaseAddress() const
{
    bool temporary = true;
    return startRangeLabel_->toPlainText().toULongLong(&temporary, 16);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::getLastAddress()
//-----------------------------------------------------------------------------
quint64 MemoryDesignerGraphicsItem::getLastAddress() const
{
    bool temporary = true;
    return endRangeLabel_->toPlainText().toULongLong(&temporary, 16);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::condense()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::condense(qreal newItemHeight)
{
    QRectF itemRectangle = boundingRect();
    int subItemPenWidth = pen().width();

    qint16 itemXPosition = -itemRectangle.width() / 2;
    quint64 itemWidth = itemRectangle.width() - subItemPenWidth;

    setRect(itemXPosition, 0, itemWidth, newItemHeight);

    setLabelPositions();
}
