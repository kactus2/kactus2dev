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

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <common/KactusColors.h>
#include <common/graphicsItems/visualizeritem.h>

#include <QBrush>
#include <QStringBuilder>

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
    setToolTip("<b>Name: </b>" % register_->name() % "<br>" %
        "<b>First address: </b>" % toHexString(offset) % "<br>" %
        "<b>Last address: </b>" % toHexString(lastAddress) % "<br>" %
        "<b>Size [bits]: </b>" % QString::number(getBitWidth()));
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::addChild()
//-----------------------------------------------------------------------------
void RegisterGraphItem::addChild(MemoryVisualizationItem* childItem)
{
    childItems_.insertMulti(childItem->getOffset(), childItem);

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

    int dimension = qMax(1, parseExpression(register_->getDimension()));

    return dimension * size;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::getRegisterMSB()
//-----------------------------------------------------------------------------
unsigned int RegisterGraphItem::getRegisterMSB() const
{
    unsigned int registerMSB = 0;
    unsigned int registerSize = getBitWidth();
    if (registerSize != 0)
    {
        registerMSB = registerSize -1;
    }

    return registerMSB;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::findHighestReservedBit()
//-----------------------------------------------------------------------------
unsigned int RegisterGraphItem::findHighestReservedBit()
{
    unsigned int highestBit = getRegisterMSB();

    // Check, if fields span outside register boundary i.e. MSB.
    for (auto i = childItems_.cend() - 1; i != childItems_.cbegin() - 1; --i)
    {
        MemoryVisualizationItem* current = i.value();
        if (current->getLastAddress() > highestBit && current->isPresent())
        {
            highestBit = current->getLastAddress();
        }
    }

    return highestBit;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::removeGapsAndSortChildren()
//-----------------------------------------------------------------------------
void RegisterGraphItem::removeGapsAndSortChildren()
{
    QMap<quint64, MemoryVisualizationItem*> sortedMap;
    for (auto item = childItems_.cbegin(); item != childItems_.cend(); ++item)
    {
        MemoryGapItem* gap = dynamic_cast<MemoryGapItem*>(*item);
        if (gap)
        {
            delete gap;
        }
        else
        {
            sortedMap.insertMulti((*item)->getLastAddress(), *item);
        }
    }

    childItems_ = sortedMap;
}

//-----------------------------------------------------------------------------
// Function: RegisterGraphItem::fillGapsBetweenChildren()
//-----------------------------------------------------------------------------
void RegisterGraphItem::fillGapsBetweenChildren()
{
    quint64 lowestBitHandled = findHighestReservedBit() + 1;

    // QMap sorts children by ascending keys. This must iterate children from largest to smallest key (MSB). 
    for (auto i = childItems_.end() - 1; i != childItems_.begin() - 1; --i)
    {
        MemoryVisualizationItem const* current = i.value();
        if (current->isPresent())
        {
            if (emptySpaceBeforeChild(current, lowestBitHandled))
            {
                i = addMemoryGap(current->getLastAddress() + 1, lowestBitHandled - 1);
            }

            lowestBitHandled = qMin(current->getOffset(), lowestBitHandled);
        }
    }

    // If there is a gap between the LSB of the register and the right-most item.
    if (lowestBitHandled > 0)
    {
        addMemoryGap(0, lowestBitHandled - 1);
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
QMap<quint64, MemoryVisualizationItem*>::iterator RegisterGraphItem::addMemoryGap(quint64 startAddress,
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
    unsigned int registerMSB = getRegisterMSB();
    quint64 lowestBitHandled = registerMSB + 1;

    // QMap sorts children by ascending keys. This must iterate children from largest to smallest key (MSB).     
    for (auto child = childItems_.end() - 1; child != childItems_.begin() - 1; --child)
    {
        MemoryVisualizationItem* current = child.value();
        if (current->isPresent())
        {
            bool overlaps = current->getLastAddress() >= lowestBitHandled;
            bool isOutsideRegister = current->getOffset() > registerMSB || current->getLastAddress() > registerMSB;

            current->setConflicted(overlaps || isOutsideRegister);
            if (overlaps)
            {
                // Walk in the opposite direction and mark any overlapping items conflicted.
                for (auto previous = child + 1; previous != childItems_.end(); ++previous)
                {
                    if ((*previous)->getOffset() <= current->getLastAddress())
                    {
                        (*previous)->setConflicted(true);
                    }
                }
            }

            lowestBitHandled = qMin(current->getOffset(), lowestBitHandled);
        }
    }
}
