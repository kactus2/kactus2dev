//-----------------------------------------------------------------------------
// File: SegmentItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 01.11.2012
//
// Description:
// Segment graphics item.
//-----------------------------------------------------------------------------

#include "SegmentItem.h"

#include "AddressSubsection.h"

#include <common/utils.h>

#include <models/addressblock.h>
#include <models/component.h>
#include <models/addressspace.h>

#include <QPainter>

//-----------------------------------------------------------------------------
// Function: SegmentItem::SegmentItem()
//-----------------------------------------------------------------------------
SegmentItem::SegmentItem(QSharedPointer<Component> component,
                         QSharedPointer<AddressSpace> addressSpace,
                         QString const& name, unsigned int startAddress,
                         unsigned int range, QGraphicsItem* parent)
    : AddressSectionItem(name, startAddress, range, parent),
      component_(component),
      addressSpace_(addressSpace),
      subsections_()
{
    int y = 0;

    foreach (QSharedPointer<MemoryMapItem> item, addressSpace_->getLocalMemoryMap()->getItems())
    {
        QSharedPointer<AddressBlock> block = item.dynamicCast<AddressBlock>();

        if (block != 0)
        {
            unsigned int blockStartAddress = Utils::str2Int(block->getBaseAddress());
            unsigned int blockEndAddress = blockStartAddress + Utils::str2Int(block->getRange());

            if (blockStartAddress > getEndAddress() || blockEndAddress < getStartAddress())
            {
                continue;
            }

            AddressSubsection* subsection = new AddressSubsection(this, WIDTH / 2 - ADDR_COLUMN_WIDTH,
                                                                  y, y + 40,
                                                                  qMax(blockStartAddress, getStartAddress()),
                                                                  qMin(blockEndAddress, getEndAddress()));
            subsections_.append(QSharedPointer<AddressSubsection>(subsection));

            y += 50;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SegmentItem::~SegmentItem()
//-----------------------------------------------------------------------------
SegmentItem::~SegmentItem()
{
}

//-----------------------------------------------------------------------------
// Function: SegmentItem::paint()
//-----------------------------------------------------------------------------
void SegmentItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    AddressSectionItem::paint(painter, option, widget);

    foreach (QSharedPointer<AddressSubsection> subsection, subsections_)
    {
        painter->setPen(QPen(QColor(0, 0, 0), 0));
        painter->drawLine(WIDTH / 2 - ADDR_COLUMN_WIDTH, subsection->getTop(),
                          WIDTH / 2, subsection->getTop());
        painter->drawLine(WIDTH / 2 - ADDR_COLUMN_WIDTH, subsection->getBottom(),
                          WIDTH / 2, subsection->getBottom());

        painter->fillRect(WIDTH / 2 - ADDR_COLUMN_WIDTH + 1, subsection->getTop(), ADDR_COLUMN_WIDTH - 1,
                          subsection->getBottom() - subsection->getTop(),
                          QBrush(QColor(0, 0, 0), Qt::DiagCrossPattern));
    }
}
