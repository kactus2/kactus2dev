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
#include <designEditors/MemoryDesigner/ConnectivityComponent.h>
#include <designEditors/MemoryDesigner/MemoryItem.h>

#include <QFont>
#include <QFontMetrics>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::MemoryDesignerGraphicsItem()
//-----------------------------------------------------------------------------
MemoryDesignerGraphicsItem::MemoryDesignerGraphicsItem(QString const& itemName, QString const& displayName,
    QVector<QString> identifierChain, QSharedPointer<ConnectivityComponent> componentInstance,
    QGraphicsItem* parent):
QObject(),
QGraphicsRectItem(parent),
nameLabel_(new QGraphicsTextItem(itemName, this)),
startRangeLabel_(new QGraphicsTextItem(this)),
endRangeLabel_(new QGraphicsTextItem(this)),
baseAddress_(0),
lastAddress_(0),
originalBaseAddress_(0),
originalLastAddress_(0),
itemName_(itemName),
displayName_(displayName),
memoryConnections_(),
containingInstance_(componentInstance),
componentVLNV_(getVLNVFromContainingInstance()),
openComponentAction_(new QAction(this)),
openItemEditorAction_(new QAction(this)),
identifierChain_(identifierChain)
{
    if (!itemName.isEmpty())
    {
        identifierChain_.append(itemName);
    }

    QString openComponentText = QStringLiteral("Open containing component");
    openComponentAction_->setText(openComponentText);

    QString openItemEditorText = QStringLiteral("Open editor for ") + itemName;
    openItemEditorAction_->setText(openItemEditorText);

    connect(openComponentAction_, SIGNAL(triggered()),
        this, SLOT(openContainingComponent()), Qt::UniqueConnection);
    connect(openItemEditorAction_, SIGNAL(triggered()), this, SLOT(openItemEditor()), Qt::UniqueConnection);

    if (!displayName_.isEmpty())
    {
        nameLabel_->setPlainText(displayName);
    }

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
// Function: MemoryDesignerGraphicsItem::hideCollidingRangeLabels()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::hideCollidingRangeLabels(quint64 connectionBaseAddress,
    quint64 connectionLastAddress)
{
    quint64 itemBaseAddress = getBaseAddress();
    quint64 itemLastAddress = getLastAddress();

    quint64 minItemBaseConnectionLast = qMin(itemBaseAddress, connectionLastAddress);
    quint64 maxItemBaseConnectionLast = qMax(itemBaseAddress, connectionLastAddress);
    if (connectionBaseAddress - itemBaseAddress < 2 || maxItemBaseConnectionLast - minItemBaseConnectionLast < 2)
    {
        hideStartRangeLabel();
    }

    quint64 minItemLastConnectionBase = qMin(itemLastAddress, connectionBaseAddress);
    quint64 maxItemLastconnectionBase = qMax(itemLastAddress, connectionBaseAddress);
    quint64 minLastAddress = qMin(itemLastAddress, connectionLastAddress);
    quint64 maxLastAddress = qMax(itemLastAddress, connectionLastAddress);
    if (maxItemLastconnectionBase - minItemLastConnectionBase < 2 || maxLastAddress - minLastAddress < 2)
    {
        hideEndRangeLabel();
    }
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
// Function: MemoryDesignerGraphicsItem::setupGraphicsItem()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::setupGraphicsItem(quint64 memoryStart, quint64 memoryEnd,
    QString const& identifier)
{
    setupLabels(memoryStart, memoryEnd);
    setupOriginalAddresses(memoryStart, memoryEnd);
    setupToolTip(identifier);
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
// Function: MemoryDesignerGraphicsItem::setOriginalAddresses()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::setupOriginalAddresses(quint64 memoryStart, quint64 memoryEnd)
{
    originalBaseAddress_ = memoryStart;
    originalLastAddress_ = memoryEnd;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::setupToolTip()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::setupToolTip(QString const& identifier)
{
    QString lineEnd = QStringLiteral("<br>");

    QString identifierText = QStringLiteral("<b>") + identifier + QStringLiteral(":</b> ") + name() + lineEnd;

    QString displayNameText("");
    if (!displayName_.isEmpty())
    {
        displayNameText = QStringLiteral("<b>Display Name:</b> ") + displayName() + lineEnd;
    }

    QString instanceText = QStringLiteral("<b>Component Instance:</b> ") + containingInstance_->getName() +
        lineEnd + lineEnd;

    QString baseAddressText =
        QStringLiteral("<b>Generic Base Address:</b> ") + getRangeStartLabel()->toPlainText() + lineEnd;
    QString lastAddressText = QStringLiteral("<b>Generic Last Address:</b> ") + getRangeEndLabel()->toPlainText();

    QString toolTipText = identifierText + displayNameText + instanceText + baseAddressText + lastAddressText;
    setToolTip(toolTipText);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::addToToolTip()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::addToToolTip(QString const& toolTipAddition)
{
    QString toolTipText = toolTip();
    toolTipText += toolTipAddition;
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
// Function: MemoryDesignerGraphicsItem::displayName()
//-----------------------------------------------------------------------------
QString MemoryDesignerGraphicsItem::displayName() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::getBaseAddress()
//-----------------------------------------------------------------------------
quint64 MemoryDesignerGraphicsItem::getBaseAddress() const
{
    return baseAddress_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::getOriginalBaseAddress()
//-----------------------------------------------------------------------------
quint64 MemoryDesignerGraphicsItem::getOriginalBaseAddress() const
{
    return originalBaseAddress_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::getLastAddress()
//-----------------------------------------------------------------------------
quint64 MemoryDesignerGraphicsItem::getLastAddress() const
{
    return lastAddress_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::getOriginalLastAddress()
//-----------------------------------------------------------------------------
quint64 MemoryDesignerGraphicsItem::getOriginalLastAddress() const
{
    return originalLastAddress_;
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
QSharedPointer<ConnectivityComponent> MemoryDesignerGraphicsItem::getContainingInstance() const
{
    return containingInstance_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::changeAddressRange()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::changeAddressRange(quint64 offset)
{
    quint64 newBaseAddress = originalBaseAddress_ + offset;
    quint64 newLastAddress = originalLastAddress_ + offset;

    setupLabels(newBaseAddress, newLastAddress);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::compressToUnCutAddresses()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::compressToUnCutAddresses(QVector<quint64> unCutAddresses, const int CUTMODIFIER,
    bool memoryItemsAreCompressed)
{
    quint64 cutArea = 0;
    
    quint64 baseAddress = getBaseAddress();
    quint64 lastAddress = getLastAddress();

    quint64 beginArea = unCutAddresses.first();
    foreach (quint64 endArea, unCutAddresses)
    {
        if (beginArea >= baseAddress && endArea <= lastAddress)
        {
            quint64 addressDifference = endArea - beginArea;
            qint64 singleCut = 0;

            if (memoryItemsAreCompressed)
            {
                singleCut = addressDifference - CUTMODIFIER;
            }
            else
            {
                int areaRequiredRows = MemoryDesignerConstants::getRequiredRowsForRange(addressDifference + 1);
                singleCut = addressDifference + 1 - areaRequiredRows;
            }

            if (singleCut > 0)
            {
                cutArea += singleCut;
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

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::compressToUnCutCoordinates()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::compressToUnCutCoordinates(QVector<qreal> unCutCoordinates,
    const qreal CUTMODIFIER, bool memoryItemsAreCompressed)
{
    qreal cutArea = 0;

    qreal itemTop = sceneBoundingRect().top();
    qreal itemLow = sceneBoundingRect().bottom();

    qreal beginArea = unCutCoordinates.first();
    foreach (qreal endArea, unCutCoordinates)
    {
        if (beginArea >= itemTop && endArea <= itemLow)
        {
            qreal areaDifference = endArea - beginArea;
            if (areaDifference > 0)
            {
                if (memoryItemsAreCompressed)
                {
                    areaDifference = areaDifference - CUTMODIFIER;
                }
                else
                {
                    qreal requiredArea = MemoryDesignerConstants::getRequiredAreaForUsedArea(areaDifference);
                    areaDifference = areaDifference - requiredArea;
                }

                if (areaDifference > 0)
                {
                    cutArea += areaDifference;
                }
            }
        }
        else if (endArea > itemLow)
        {
            break;
        }

        beginArea = endArea;
    }

    if (cutArea > 0)
    {
        qreal condensedHeight = boundingRect().height() - cutArea;
        condense(condensedHeight);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::contextMenuEvent()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu contextMenu;
    contextMenu.addAction(openComponentAction_);
    if (!identifierChain_.isEmpty())
    {
        contextMenu.addAction(openItemEditorAction_);
    }

    contextMenu.exec(event->screenPos());
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::openContainingComponent()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::openContainingComponent()
{
    emit openComponentDocument(componentVLNV_, QVector<QString>());
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::openItemEditor()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::openItemEditor()
{
    if (!identifierChain_.isEmpty())
    {
        emit openComponentDocument(componentVLNV_, identifierChain_);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::getVLNVFromContainingInstance()
//-----------------------------------------------------------------------------
VLNV MemoryDesignerGraphicsItem::getVLNVFromContainingInstance()
{
    if (containingInstance_)
    {
        VLNV::IPXactType vlnvType = VLNV::COMPONENT;
        QString vlnvString = containingInstance_->getVlnv();

        VLNV componentVLNV(vlnvType, vlnvString);
        return componentVLNV;
    }

    return VLNV();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::getIdentifierChain()
//-----------------------------------------------------------------------------
QVector<QString> MemoryDesignerGraphicsItem::getIdentifierChain() const
{
    return identifierChain_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignerGraphicsItem::setNewIdentifierChain()
//-----------------------------------------------------------------------------
void MemoryDesignerGraphicsItem::setNewIdentifierChain(QVector<QString> newIdentifiers)
{
    identifierChain_ = newIdentifiers;
}
