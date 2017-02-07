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
    QString baseAddressInHexa = QString::number(memoryStart, 16).toUpper();
    QString lastAddressInHexa = QString::number(memoryEnd, 16).toUpper();

    int amountOfNumbers = MemoryDesignerConstants::getAmountOfNumbersInRange(baseAddressInHexa, lastAddressInHexa);
    baseAddressInHexa = MemoryDesignerConstants::getValueWithZerosAdded(baseAddressInHexa, amountOfNumbers);
    lastAddressInHexa = MemoryDesignerConstants::getValueWithZerosAdded(lastAddressInHexa, amountOfNumbers);

    baseAddress_ = memoryStart;
    lastAddress_ = memoryEnd;
    startRangeLabel_->setPlainText(baseAddressInHexa);
    endRangeLabel_->setPlainText(lastAddressInHexa);
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
// Function: MemoryDesignerGraphicsItem::condense()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::condense(qreal newItemHeight)
{
    QRectF itemRectangle = boundingRect();
    int itemPenWidth = pen().width();

    qint16 itemXPosition = -itemRectangle.width() / 2;
    qreal itemWidth = itemRectangle.width() - itemPenWidth;

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
// Function: MemoryDesignerGraphicsItem::hasConnection()
//-----------------------------------------------------------------------------
bool MemoryDesignerGraphicsItem::hasConnection(MemoryConnectionItem* comparisonConnectionItem) const
{
    return getMemoryConnections().values().contains(comparisonConnectionItem);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::fitNameLabel()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::fitLabel(QGraphicsTextItem* label)
{
    QString labelText = label->toPlainText();

    QFontMetrics fontMetrics(label->font());
    qreal originalLabelWidth = fontMetrics.width(labelText);
    qreal availableArea = getItemWidth() - 6;

    bool collidesWithRange = labelCollidesWithRangeLabels(label, fontMetrics.height());

    if (collidesWithRange || originalLabelWidth > availableArea)
    {
        if (collidesWithRange)
        {
            availableArea = availableArea - getRangeStartLabel()->boundingRect().width();
        }

        QFontMetrics labelMetrics (label->font());
        QString shortened = labelMetrics.elidedText(labelText, Qt::ElideRight, availableArea);

        label->setPlainText(shortened);

        if (label->pos().x() < 0)
        {
            qreal shortenedWidth = fontMetrics.width(shortened);
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
bool MemoryDesignerGraphicsItem::labelCollidesWithRangeLabels(QGraphicsTextItem* label, qreal fontHeight)
    const
{
    if (label->collidesWithItem(getRangeStartLabel()) || label->collidesWithItem(getRangeEndLabel()))
    {
        QRectF labelRectangle = label->sceneBoundingRect();
        QRectF startRangeRectangle = getRangeStartLabel()->sceneBoundingRect();
        QRectF endRangeRectangle = getRangeEndLabel()->sceneBoundingRect();

        labelRectangle.setHeight(fontHeight);
        startRangeRectangle.setHeight(fontHeight);
        endRangeRectangle.setHeight(fontHeight);

        return MemoryDesignerConstants::itemOverlapsAnotherItem(labelRectangle, 0, startRangeRectangle, 0) ||
            MemoryDesignerConstants::itemOverlapsAnotherItem(labelRectangle, 0, endRangeRectangle, 0);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::getContainingInstance()
//-----------------------------------------------------------------------------
QString MemoryDesignerGraphicsItem::getContainingInstance() const
{
    return instanceName_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::changeAddressRange()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::changeAddressRange(quint64 offset)
{
    quint64 newBaseAddress = getBaseAddress() + offset;
    quint64 newLastAddress = getLastAddress() + offset;

    setupLabels(newBaseAddress, newLastAddress);

    baseAddress_ = newBaseAddress;
    lastAddress_ = newLastAddress;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::compressToUnCutAddresses()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::compressToUnCutAddresses(QVector<quint64> unCutAddresses, const int CUTMODIFIER)
{
    quint64 cutArea = 0;
    
    quint64 baseAddress = getBaseAddress();
    quint64 lastAddress = getLastAddress();

    quint64 beginArea = unCutAddresses.first();
    foreach (quint64 endArea, unCutAddresses)
    {
        if (beginArea >= baseAddress && endArea <= lastAddress)
        {
            qint64 addressDifference = endArea - beginArea - CUTMODIFIER;
            if (addressDifference > 0)
            {
                cutArea += addressDifference;
            }
        }
        else if (endArea > lastAddress)
        {
            break;
        }

        beginArea = endArea;
    }

    if (cutArea > 0)
    {
        qreal cutAreaHeight = cutArea * MemoryDesignerConstants::RANGEINTERVAL;
        qreal condensedHeight = boundingRect().height() - cutAreaHeight;
        condense(condensedHeight);
    }
}
