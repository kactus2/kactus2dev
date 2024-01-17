//-----------------------------------------------------------------------------
// File: registergraphitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
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

#include <KactusAPI/include/ExpressionParser.h>

#include <common/KactusColors.h>
#include <common/graphicsItems/visualizeritem.h>

#include <QBrush>
#include <QStringBuilder>
#include <QMapIterator>

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::RegisterGraphItem()
//-----------------------------------------------------------------------------
RegisterGraphItem::RegisterGraphItem(QSharedPointer<Register> reg,
    QSharedPointer<ExpressionParser> expressionParser, QGraphicsItem* parent):
MemoryVisualizationItem(expressionParser, parent),
register_(reg)
{
	Q_ASSERT(register_);

	setDefaultBrush(QBrush(KactusColors::REGISTER_COLOR));

    updateDisplay();
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::updateDisplay()
//-----------------------------------------------------------------------------
void RegisterGraphItem::updateDisplay()
{
    QString name = register_->name();

    int dimension = parseExpression(register_->getDimension());
    if (dimension > 0)
    {
        name.append("[" % QString::number(dimension - 1) % ":0]");
    }

    quint64 offset = getOffset();
    quint64 lastAddress = getLastAddress();

    setName(name);
    setDisplayOffset(offset);
    setDisplayLastAddress(lastAddress);

    // Set tooltip to show addresses in hexadecimals.
    const int BIT_WIDTH = getBitWidth();
    setToolTip("<b>Name: </b>" % register_->name() % "<br>" %
        "<b>First address: </b>" % toHexString(offset, BIT_WIDTH) % "<br>" %
        "<b>Last address: </b>" % toHexString(lastAddress, BIT_WIDTH) % "<br>" %
        "<b>Size [bits]: </b>" % QString::number(getBitWidth()));
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::addChild()
//-----------------------------------------------------------------------------
void RegisterGraphItem::addChild(MemoryVisualizationItem* childItem)
{
    childItems_.insert(childItem->getOffset(), childItem);

    childItem->setParentItem(this);    
    childItem->setVisible(isExpanded());
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::removeChild()
//-----------------------------------------------------------------------------
void RegisterGraphItem::removeChild( MemoryVisualizationItem* childItem )
{
    quint64 offset = childItems_.key(childItem);

    Q_ASSERT(childItems_.contains(offset));
    childItems_.remove(offset, childItem);
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::getOffset()
//-----------------------------------------------------------------------------
quint64 RegisterGraphItem::getOffset() const
{	
	MemoryVisualizationItem const* blockItem = static_cast<MemoryVisualizationItem const*>(parentItem());
	Q_ASSERT(blockItem);
	quint64 blockOffset = blockItem->getOffset();

    quint64 regOffset = parseExpression(register_->getAddressOffset());

	// the total offset is the address block's offset added with register's offset
	return blockOffset + regOffset;
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
    return parseExpression(register_->getSize());
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::getAddressUnitSize()
//-----------------------------------------------------------------------------
unsigned int RegisterGraphItem::getAddressUnitSize() const 
{
	MemoryVisualizationItem const* addrBlock = static_cast<MemoryVisualizationItem const*>(parentItem());
	Q_ASSERT(addrBlock);
	return addrBlock->getAddressUnitSize();
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::setWidth()
//-----------------------------------------------------------------------------
void RegisterGraphItem::setWidth(qreal width)
{
    VisualizerItem::setWidth(width);

    // Child width is adjusted according to width and number of bits.
    repositionChildren();
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::isPresent()
//-----------------------------------------------------------------------------
bool RegisterGraphItem::isPresent() const
{
    return register_->getIsPresent().isEmpty() || parseExpression(register_->getIsPresent()) == 1;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::repositionChildren()
//-----------------------------------------------------------------------------
void RegisterGraphItem::repositionChildren()
{
    unsigned int highestBitInUse = findHighestReservedBit();
    const unsigned int BIT_WIDTH = highestBitInUse + 1;
    const qreal PIXELS_PER_BIT = rect().width() / BIT_WIDTH;

    for (auto current : childItems_)
    {
        bool present = current->isPresent();
        current->setVisible(present && isExpanded());

        if (present)
        {
            quint64 availableBits = BIT_WIDTH - current->getOffset();
            quint64 childBitWidth = qMin(quint64(current->getBitWidth()), availableBits);

            qreal width = qMax(1.0, PIXELS_PER_BIT * childBitWidth);
            current->setWidth(width);

            quint64 highestChildBit = current->getLastAddress();
            qreal xPos = (highestBitInUse - highestChildBit) * PIXELS_PER_BIT;

            current->setPos(QPointF(xPos, rect().bottom()));
        }
    }

    resizeToContent();
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
    unsigned int bitSize = parseExpression(register_->getSize());
    unsigned int size = bitSize / addrUnit;
    if (size*addrUnit < bitSize) 
    {
        ++size; //Round truncated number upwards
    }

    quint64 dimension = qMax(quint64(1), parseExpression(register_->getDimension()));

    return dimension * size;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::getRegisterMSB()
//-----------------------------------------------------------------------------
unsigned int RegisterGraphItem::getRegisterMSB() const
{
    unsigned int registerMSB = 0;
    
    if (unsigned int registerSize = getBitWidth(); 
        registerSize != 0)
    {
        registerMSB = registerSize -1;
    }

    return registerMSB;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::findHighestReservedBit()
//-----------------------------------------------------------------------------
unsigned int RegisterGraphItem::findHighestReservedBit() const
{
    unsigned int highestBit = getRegisterMSB();

    if (!childItems_.isEmpty())
    {
        for (MemoryVisualizationItem const* current: childItems_)
        {
            if (current->getLastAddress() > highestBit && current->isPresent())
            {
                highestBit = current->getLastAddress();
            }
        }
    }

    return highestBit;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::removeGapsAndSortChildren()
//-----------------------------------------------------------------------------
void RegisterGraphItem::removeGapsAndSortChildren()
{
    QMultiMap<quint64, MemoryVisualizationItem*> sortedMap;
    for (auto item = childItems_.cbegin(); item != childItems_.cend(); ++item)
    {
        MemoryGapItem* gap = dynamic_cast<MemoryGapItem*>(*item);
        if (gap)
        {
            delete gap;
        }
        else
        {
            sortedMap.insert((*item)->getOffset(), *item);
        }
    }

    childItems_ = sortedMap;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::fillGapsBetweenChildren()
//-----------------------------------------------------------------------------
void RegisterGraphItem::fillGapsBetweenChildren()
{
    if (childItems_.isEmpty())
    {
        return;
    }

    auto parentMSB = getRegisterMSB();
    quint64 highestBitHandled = 0;

    // QMap sorts children by ascending offsets.
    for (auto i = childItems_.begin(); i != childItems_.end(); ++i)
    {
        auto currentChild = i.value();
        auto currentOffset = currentChild->getOffset();

        if (currentChild->isPresent())
        {
            // Insert gap between fields only if they are not overlapping with a third field.
            if (highestBitHandled < currentOffset)
            {        
                i = addMemoryGap(highestBitHandled, currentOffset - 1);
                highestBitHandled = currentOffset;
            }
            else
            {
                highestBitHandled = qMax(highestBitHandled, currentChild->getLastAddress() + 1);
            }
        }
    }

    // Insert gap between the MSB of the register and the left-most item if needed.
    if (highestBitHandled <= parentMSB)
    {
        addMemoryGap(highestBitHandled, parentMSB);
    }
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::emptySpaceBeforeChild()
//-----------------------------------------------------------------------------
bool RegisterGraphItem::emptySpaceBeforeChild(MemoryVisualizationItem const* current,
    quint64 lastBitIndexInUse) const
{
    return current->getLastAddress() + 1 < lastBitIndexInUse;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::addMemoryGap()
//-----------------------------------------------------------------------------
QMultiMap<quint64, MemoryVisualizationItem*>::iterator RegisterGraphItem::addMemoryGap(quint64 startAddress,
    quint64 endAddress)
{
    FieldGapItem* gap = new FieldGapItem(tr("Reserved"), getExpressionParser(), this);
    gap->setDisplayOffset(startAddress);
    gap->setDisplayLastAddress(endAddress);
    gap->updateDisplay();

    return childItems_.insert(gap->getLastAddress(), gap);
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::markConflictingChildren()
//-----------------------------------------------------------------------------
void RegisterGraphItem::markConflictingChildren()
{
    if (childItems_.isEmpty())
    {
        return;
    }

    auto registerMSB = getRegisterMSB();
    quint64 highestBitHandled = 0;

    // QMap sorts children by ascending offsets.
    for (auto i = childItems_.begin(); i != childItems_.end(); ++i)
    {
        auto currentChild = i.value();
        auto currentOffset = currentChild->getOffset();

        if (!currentChild->isPresent())
        {
            continue;
        }

        bool overlaps = currentOffset < highestBitHandled;
        bool isOutsideRegister = currentOffset > registerMSB ||
            currentChild->getLastAddress() > registerMSB;

        currentChild->setConflicted(overlaps || isOutsideRegister);

        if (overlaps && i != childItems_.begin())
        {
            // Walk in the opposite direction and mark any overlapping items conflicted.
            for (auto previous = std::prev(i); previous != childItems_.begin(); --previous)
            {
                if ((*previous)->getLastAddress() >= currentOffset)
                {
                    (*previous)->setConflicted(true);
                }
            }

            // Check first item separately to avoid undefined iterator behavior.
            if (childItems_.first()->getLastAddress() >= currentOffset)
            {
                childItems_.first()->setConflicted(true);
            }
        }

        highestBitHandled = qMax(highestBitHandled, currentChild->getLastAddress() + 1);
    }
}
