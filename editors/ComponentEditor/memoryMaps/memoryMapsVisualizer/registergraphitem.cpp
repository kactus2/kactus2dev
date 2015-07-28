//-----------------------------------------------------------------------------
// File: registergraphitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 24.04.2012
//
// Description:
// The graphical item that represents one register.
//-----------------------------------------------------------------------------

#include "registergraphitem.h"
#include "fieldgraphitem.h"

#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>
#include <editors/ComponentEditor/visualization/fieldgapitem.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <common/KactusColors.h>
#include <common/utils.h>
#include <common/graphicsItems/visualizeritem.h>

#include <IPXACTmodels/field.h>

#include <QBrush>
#include <QColor>

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::RegisterGraphItem()
//-----------------------------------------------------------------------------
RegisterGraphItem::RegisterGraphItem(QSharedPointer<Register> reg,
    QSharedPointer<ExpressionParser> expressionParser, QGraphicsItem* parent):
MemoryVisualizationItem(parent),
register_(reg),
dimensionIndex_(0),
expressionParser_(expressionParser)
{
	Q_ASSERT(register_);

    QBrush brush(KactusColors::REGISTER_COLOR);
	setDefaultBrush(brush);

    updateDisplay();
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::~RegisterGraphItem()
//-----------------------------------------------------------------------------
RegisterGraphItem::~RegisterGraphItem()
{
}

//-----------------------------------------------------------------------------
// Function: registergraphitem::refresh()
//-----------------------------------------------------------------------------
void RegisterGraphItem::refresh() 
{
    updateDisplay();
    reorganizeChildren();
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::updateDisplay()
//-----------------------------------------------------------------------------
void RegisterGraphItem::updateDisplay()
{
    QString name = register_->getName();

    if (expressionParser_->parseExpression(register_->getDimensionExpression()).toUInt() > 0)
    {
        name.append("[" + QString::number(dimensionIndex_) + "]");
    }

    quint64 offset = getOffset();
    quint64 lastAddress = getLastAddress();

    setName(name);
    setDisplayOffset(offset);
    setDisplayLastAddress(lastAddress);

    // Set tooltip to show addresses in hexadecimals.
    setToolTip("<b>Name: </b>" + register_->getName() + "<br>" +
        "<b>First address: </b>" + toHexString(offset) + "<br>" +
        "<b>Last address: </b>" + toHexString(lastAddress) + "<br>" +
        "<b>Size [bits]: </b>" + expressionParser_->parseExpression(register_->getSizeExpression()));
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::removeChild()
//-----------------------------------------------------------------------------
void RegisterGraphItem::removeChild( MemoryVisualizationItem* childItem )
{
    quint64 offset = childItem->getLastAddress();

    Q_ASSERT(childItems_.contains(offset));
    childItems_.remove(offset, childItem);
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::getOffset()
//-----------------------------------------------------------------------------
quint64 RegisterGraphItem::getOffset() const
{
	// the register offset from the address block
	quint64 regOffset = expressionParser_->parseExpression(register_->getAddressOffset()).toUInt();

	// the address block's offset
	MemoryVisualizationItem* blockItem = static_cast<MemoryVisualizationItem*>(parentItem());
	Q_ASSERT(blockItem);
	quint64 blockOffset = blockItem->getOffset();

	// the total offset is the address block's offset added with register's offset
	return blockOffset + regOffset + (dimensionIndex_ * getSizeInAUB());
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::getLastAddress()
//-----------------------------------------------------------------------------
quint64 RegisterGraphItem::getLastAddress() const
{
    quint64 size = getSizeInAUB();

    if (size == 0)
    {
        return 0;
    }

    // the last address contained in the register
    return getOffset() + size - 1;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::getBitWidth()
//-----------------------------------------------------------------------------
int RegisterGraphItem::getBitWidth() const
{
	return expressionParser_->parseExpression(register_->getSizeExpression()).toUInt();
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::getAddressUnitSize()
//-----------------------------------------------------------------------------
unsigned int RegisterGraphItem::getAddressUnitSize() const 
{
	MemoryVisualizationItem* addrBlock = static_cast<MemoryVisualizationItem*>(parentItem());
	Q_ASSERT(addrBlock);
	return addrBlock->getAddressUnitSize();
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::setWidth()
//-----------------------------------------------------------------------------
void RegisterGraphItem::setWidth(qreal width)
{
    if (childWidth_ != width)
    {
        childWidth_ = width;
        VisualizerItem::setWidth(width);
        reorganizeChildren();
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::setDimensionIndex()
//-----------------------------------------------------------------------------
void RegisterGraphItem::setDimensionIndex(unsigned int index)
{
    dimensionIndex_ = index;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::isPresent()
//-----------------------------------------------------------------------------
bool RegisterGraphItem::isPresent() const
{
     return expressionParser_->parseExpression(register_->getIsPresentExpression()) == "1";
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::updateChildMap()
//-----------------------------------------------------------------------------
void RegisterGraphItem::updateChildMap()
{
    QMap<quint64, MemoryVisualizationItem*> newMap;

    foreach (MemoryVisualizationItem* item, childItems_)
    {
        MemoryGapItem* gap = dynamic_cast<MemoryGapItem*>(item);
        if (gap)
        {
            childItems_.remove(gap->getLastAddress(), gap);
            delete gap;
            gap = 0;
        }
    }

    unsigned int registerMSB = getRegisterMSB(getBitWidth());

    foreach (MemoryVisualizationItem* item, childItems_)
    {
        bool childIsOutsideRegister = item->getLastAddress() > registerMSB;
        item->setConflicted(childIsOutsideRegister);

        item->setDisplayOffset(item->getLastAddress());
        item->setDisplayLastAddress(item->getOffset());

        item->setVisible(isExpanded() && item->isPresent());

        newMap.insertMulti(item->getLastAddress(), item);
    }

    // Sort childs with same last bit for stable order.
    foreach(quint64 msb, newMap.uniqueKeys())
    {
        if(newMap.count(msb) != 1)
        {
            QList<MemoryVisualizationItem*> childs = newMap.values(msb);
            newMap.remove(msb);

            qSort(childs.begin(), childs.end(), compareItems);
            foreach(MemoryVisualizationItem* child, childs)
            {
                newMap.insertMulti(msb,child);
            }
        }
    }

    childItems_ = newMap;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::repositionChildren()
//-----------------------------------------------------------------------------
void RegisterGraphItem::repositionChildren()
{
    unsigned int registerSize = getBitWidth();
    unsigned int registerMSB = getRegisterMSB(registerSize);

    quint64 lastBitIndexInUse = registerSize;
    MemoryVisualizationItem* previous = 0;
    // QMap sorts children by ascending keys. This must iterate children from largest to smallest key (MSB). 
    // A local copy of the child items is created to ensure that inserting gaps does not interfere the iteration.
    QMap<quint64, MemoryVisualizationItem*> childCopies = childItems_;
    for (QMap<quint64, MemoryVisualizationItem*>::iterator i = childCopies.end() - 1; i != childCopies.begin() - 1; 
        i--)
    { 
        MemoryVisualizationItem* current = i.value();

        if (current->isPresent())
        {
            // if there is a gap between the MSB of the register and the last item.
            if (emptySpaceBeforeLeftmostChild(current, registerMSB))
            {
                createMemoryGap(current->getOffset() + current->getBitWidth(), registerMSB);
            }
            else if (emptySpaceBeforeChild(current, lastBitIndexInUse))
            {
                createMemoryGap(current->getLastAddress() + 1, lastBitIndexInUse - 1);
            }

            if (childrenOverlap(current, previous))
            {
                current->setConflicted(true);
                previous->setConflicted(true);
            }
            else if (current->getLastAddress() >= lastBitIndexInUse)
            {
                current->setConflicted(true);
            }

            current->setPos(findPositionFor(current));
            current->setWidth(findWidthFor(current));

            lastBitIndexInUse = qMin(current->getOffset(), lastBitIndexInUse);
            previous = current;
        }
    }

    if (previous && lastBitIndexInUse > 0)
    {
        createMemoryGap(0, lastBitIndexInUse - 1);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::getSizeInAUB()
//-----------------------------------------------------------------------------
quint64 RegisterGraphItem::getSizeInAUB() const
{
    // how many bits one address unit takes
    unsigned int addrUnit = getAddressUnitSize();

    // prevent division by zero
    if (addrUnit == 0)
    {
        addrUnit = 1;
    }

    // how many address unit are contained in the register
    unsigned int bitSize = expressionParser_->parseExpression(register_->getSizeExpression()).toUInt();
    unsigned int size = bitSize / addrUnit;
    if (size*addrUnit < bitSize) 
    {
        size++; //Round truncated number upwards
    }

    return size;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::findWidthFor()
//-----------------------------------------------------------------------------
qreal RegisterGraphItem::findWidthFor(MemoryVisualizationItem* child) const
{
    unsigned int registerSize = getBitWidth();

    quint64 availableBits = registerSize - child->getOffset();
    quint64 childBitWidth = qMin(quint64(child->getBitWidth()), availableBits);

    qreal pixelsPerBit = childWidth_/registerSize;

    return pixelsPerBit * childBitWidth;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::findPositionFor()
//-----------------------------------------------------------------------------
QPointF RegisterGraphItem::findPositionFor(MemoryVisualizationItem* child) const
{
    qreal y = rect().bottom();

    unsigned int registerSize = getBitWidth();
    unsigned int registerMSB = getRegisterMSB(registerSize);

    unsigned int hiBit = child->getLastAddress();

    if (hiBit >= registerMSB)
    {
        return QPointF(0, y);
    }

    quint64 bits =  qMin(hiBit + 1, registerSize);    
    qreal pixelsPerBit = childWidth_/registerSize;

    qreal x = childWidth_ - pixelsPerBit * bits;

    return QPointF(x, y);
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::getRegisterMSB()
//-----------------------------------------------------------------------------
unsigned int RegisterGraphItem::getRegisterMSB(unsigned int registerSize) const
{
    unsigned int registerMSB = 0;
    if (registerSize != 0)
    {
        registerMSB = registerSize -1;
    }

    return registerMSB;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::emptySpaceBeforeLeftmostChild()
//-----------------------------------------------------------------------------
bool RegisterGraphItem::emptySpaceBeforeLeftmostChild(MemoryVisualizationItem* current, unsigned int registerMSB) const
{
    return current == childItems_.last() && current->getLastAddress() < registerMSB;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::emptySpaceBeforeChild()
//-----------------------------------------------------------------------------
bool RegisterGraphItem::emptySpaceBeforeChild(MemoryVisualizationItem* current, quint64 lastBitIndexInUse) const
{
    return current->getLastAddress() + 1 < lastBitIndexInUse;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::createMemoryGap()
//-----------------------------------------------------------------------------
void RegisterGraphItem::createMemoryGap(quint64 startAddress, quint64 endAddress)
{
    // create the gap item
    FieldGapItem* gap = new FieldGapItem(tr("Reserved"), this);
    gap->setStartAddress(startAddress);
    gap->setEndAddress(qMin(endAddress, quint64(getRegisterMSB(getBitWidth()))));
    gap->setPos(findPositionFor(gap));
    gap->setWidth(findWidthFor(gap));

    childItems_.insert(gap->getLastAddress(), gap);
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::childrenOverlap()
//-----------------------------------------------------------------------------
bool RegisterGraphItem::childrenOverlap(MemoryVisualizationItem* current, MemoryVisualizationItem* previous)
{
    return previous && current->getLastAddress() >= previous->getOffset();
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::compareItems()
//-----------------------------------------------------------------------------
bool RegisterGraphItem::compareItems(MemoryVisualizationItem const* s1, MemoryVisualizationItem const* s2)
{
    // Field with higher offset (lsb) precedes lower offset.
    return s1->getOffset() > s2->getOffset();
}
