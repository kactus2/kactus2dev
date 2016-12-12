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
#include <QFontMetrics>
#include <QGraphicsSceneMouseEvent>

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::MemoryDesignerGraphicsItem()
//-----------------------------------------------------------------------------
MemoryDesignerGraphicsItem::MemoryDesignerGraphicsItem(QString const& itemName, QString const& instanceName,
    QGraphicsItem* parent):
QGraphicsRectItem(parent),
nameLabel_(new QGraphicsTextItem(itemName, this)),
startRangeLabel_(new QGraphicsTextItem(this)),
endRangeLabel_(new QGraphicsTextItem(this)),
baseAddress_(0),
lastAddress_(0),
itemName_(itemName),
amountOfLabelNumbers_(0),
memoryConnections_(),
instanceName_(instanceName)
{
    QFont labelFont = nameLabel_->font();
    labelFont.setWeight(QFont::Bold);

    nameLabel_->setFont(labelFont);
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
    hideStartRangeLabel();
    hideEndRangeLabel();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::hideStartRangeLabel()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::hideStartRangeLabel()
{
    startRangeLabel_->hide();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::hideEndRangeLabel()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::hideEndRangeLabel()
{
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
// Function: MemoryDesignerGraphicsItem::setupLabels()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::setupLabels(quint64 memoryStart, quint64 memoryEnd)
{
    amountOfLabelNumbers_ = getAmountOfLabelNumbers(memoryStart, memoryEnd);
    setBaseAddress(memoryStart);
    setLastAddress(memoryEnd);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::setupToolTip()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::setupToolTip(QString const& identifier)
{
    QString toolTipText =
        QStringLiteral("<b>") + identifier + QStringLiteral(":</b> ") + name() + QStringLiteral("<br>") +
        QStringLiteral("<b>Component Instance:</b> ") + instanceName_ + QStringLiteral("<br><br>") +
        QStringLiteral("<b>Generic Base Address:</b> ") + getRangeStartLabel()->toPlainText() +
        QStringLiteral("<br>") +
        QStringLiteral("<b>Generic Last Address:</b> ") + getRangeEndLabel()->toPlainText();
    setToolTip(toolTipText);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::getAmountOfLabelNumbers()
//-----------------------------------------------------------------------------
int MemoryDesignerGraphicsItem::getAmountOfLabelNumbers(quint64 memoryStart, quint64 memoryEnd) const
{
    QString formattedStart = QString::number(memoryStart, 16).toUpper();
    QString formattedEnd = QString::number(memoryEnd, 16).toUpper();

    int startSize = formattedStart.size();
    int endSize = formattedEnd.size();

    int amountOfNumbers = endSize;
    if (startSize > endSize)
    {
        amountOfNumbers = startSize;
    }

    while (amountOfNumbers % 4)
    {
        amountOfNumbers++;
    }

    return amountOfNumbers;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::getFormattedRangeValue()
//-----------------------------------------------------------------------------
QString MemoryDesignerGraphicsItem::getValueFormattedToHexadecimal(quint64 range) const
{
    QString formattedRange = QString::number(range, 16).toUpper();

    int rangeSize = formattedRange.size();
    int amountOfZeros = amountOfLabelNumbers_ - rangeSize;
    while (amountOfZeros > 0)
    {
        formattedRange.prepend('0');
        amountOfZeros--;
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
    return baseAddress_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::getLastAddress()
//-----------------------------------------------------------------------------
quint64 MemoryDesignerGraphicsItem::getLastAddress() const
{
    return lastAddress_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::setBaseAddress()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::setBaseAddress(quint64 newBaseAddress)
{
    baseAddress_ = newBaseAddress;
    startRangeLabel_->setPlainText(getValueFormattedToHexadecimal(newBaseAddress));
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::setLastAddress()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::setLastAddress(quint64 newLastAddress)
{
    lastAddress_ = newLastAddress;
    endRangeLabel_->setPlainText(getValueFormattedToHexadecimal(newLastAddress));
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

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::addMemoryConnection()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::addMemoryConnection(MemoryConnectionItem* connectionItem)
{
    quint64 connectionBaseAddress = connectionItem->getRangeStartValue();
    memoryConnections_.insertMulti(connectionBaseAddress, connectionItem);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::getMemoryConnections()
//-----------------------------------------------------------------------------
QMap<quint64, MemoryConnectionItem*> MemoryDesignerGraphicsItem::getMemoryConnections() const
{
    return memoryConnections_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::getConnectionsInVector()
//-----------------------------------------------------------------------------
QVector<MemoryConnectionItem*> MemoryDesignerGraphicsItem::getConnectionsInVector() const
{
    QVector<MemoryConnectionItem*> vectoredConnections;

    QMapIterator<quint64, MemoryConnectionItem*> connectionIterator(memoryConnections_);
    while (connectionIterator.hasNext())
    {
        connectionIterator.next();

        vectoredConnections.append(connectionIterator.value());
    }

    return vectoredConnections;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::fitNameLabel()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::fitLabel(QGraphicsTextItem* label)
{
    qreal originalLabelWidth = label->boundingRect().width();
    qreal availableArea = getItemWidth() - 6;

    bool collidesWithRange = labelCollidesWithRangeLabels(label);

    if (collidesWithRange || originalLabelWidth > availableArea)
    {
        if (collidesWithRange)
        {
            availableArea = availableArea - getRangeStartLabel()->boundingRect().width();
        }

        QFontMetrics labelMetrics (label->font());
        QString shortened = labelMetrics.elidedText(label->toPlainText(), Qt::ElideRight, availableArea);

        label->setPlainText(shortened);

        if (label->pos().x() < 0)
        {
            int shortenedWidth = label->boundingRect().width();
            qreal widthDifference = originalLabelWidth - shortenedWidth;
            label->setX(label->pos().x() + widthDifference);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::getItemWidth()
//-----------------------------------------------------------------------------
qreal MemoryDesignerGraphicsItem::getItemWidth() const
{
    return boundingRect().width();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::labelCollidesWithRangeLabels()
//-----------------------------------------------------------------------------
bool MemoryDesignerGraphicsItem::labelCollidesWithRangeLabels(QGraphicsTextItem* label) const
{
    return label->collidesWithItem(getRangeStartLabel()) || label->collidesWithItem(getRangeEndLabel());
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::getContainingInstance()
//-----------------------------------------------------------------------------
QString MemoryDesignerGraphicsItem::getContainingInstance() const
{
    return instanceName_;
}
