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

namespace
{
    /*!
     *  Sorting operator for sorting address blocks to ascending base address.
     */
    bool addressBlockSortOp(QSharedPointer<AddressBlock const> lhs, QSharedPointer<AddressBlock const> rhs)
    {
        return Utils::str2Int(lhs->getBaseAddress()) < Utils::str2Int(rhs->getBaseAddress());
    }
}

//-----------------------------------------------------------------------------
// Function: SegmentItem::SegmentItem()
//-----------------------------------------------------------------------------
SegmentItem::SegmentItem(QSharedPointer<Component> component,
                         QSharedPointer<AddressSpace> addressSpace,
                         QString const& name, quint64 startAddress,
                         quint64 range, QGraphicsItem* parent)
    : AddressSectionItem(name, startAddress, range, parent),
      component_(component),
      addressSpace_(addressSpace),
      subsections_()
{
    // Add local memory map address blocks as subsections.

    // First sort address blocks based on their base address.
    QList< QSharedPointer<AddressBlock> > blocks;

    foreach (QSharedPointer<MemoryMapItem> item, addressSpace_->getLocalMemoryMap()->getItems())
    {
        QSharedPointer<AddressBlock> block = item.dynamicCast<AddressBlock>();

        if (block != 0)
        {
            blocks.append(block);
        }
    }

    if (!blocks.empty())
    {
        qSort(blocks.begin(), blocks.end(), &addressBlockSortOp);

        foreach (QSharedPointer<AddressBlock> block, blocks)
        {
            quint64 blockStartAddress = Utils::str2Int(block->getBaseAddress());
            quint64 blockEndAddress = blockStartAddress + Utils::str2Int(block->getRange()) - 1;

            if (blockStartAddress > getEndAddress() || blockEndAddress < getStartAddress())
            {
                continue;
            }

            // Determine the top and bottom coordinates for the subsection.
            int top = SUBSECTION_SPACING;
            
            if (!subsections_.empty())
            {
                top = subsections_.back()->getBottom() + SUBSECTION_SPACING;
            }

            int bottom = top + SUBSECTION_HEIGHT;

            if (blockStartAddress <= getStartAddress())
            {
                top = 0;
                bottom = SUBSECTION_HEIGHT;
            }
            else if (blockEndAddress >= getEndAddress())
            {
                top = qMax<int>(rect().height() - SUBSECTION_HEIGHT, top);
                bottom = top + SUBSECTION_HEIGHT;
            }

            AddressSubsection* subsection = new AddressSubsection(this, WIDTH / 2 - ADDR_COLUMN_WIDTH,
                                                                  top, bottom,
                                                                  qMax(blockStartAddress, getStartAddress()),
                                                                  qMin(blockEndAddress, getEndAddress()));
            subsections_.append(QSharedPointer<AddressSubsection>(subsection));
        }
    }

    // Expand the segment if it is too small to contain all subsections.
    if (!subsections_.empty())
    {
        if (subsections_.back()->getEndAddress() == getEndAddress())
        {
            setHeight(subsections_.back()->getBottom());
        }
        else if (subsections_.back()->getBottom() + SUBSECTION_SPACING > rect().height())
        {
            setHeight(subsections_.back()->getBottom() + SUBSECTION_SPACING);
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

    // Visualize local memory map address blocks.
    foreach (QSharedPointer<AddressSubsection> subsection, subsections_)
    {
        // Draw start and end lines.
        painter->setPen(QPen(QColor(0, 0, 0), 0));
        painter->drawLine(WIDTH / 2 - ADDR_COLUMN_WIDTH, subsection->getTop(),
                          WIDTH / 2, subsection->getTop());
        painter->drawLine(WIDTH / 2 - ADDR_COLUMN_WIDTH, subsection->getBottom(),
                          WIDTH / 2, subsection->getBottom());

        // Fill with a diagonal pattern.
        painter->fillRect(WIDTH / 2 - ADDR_COLUMN_WIDTH + 1, subsection->getTop(), ADDR_COLUMN_WIDTH - 1,
                          subsection->getBottom() - subsection->getTop(),
                          QBrush(QColor(0, 0, 0), Qt::BDiagPattern));

        // Write address block name on top of the visualization.
    }
}

//-----------------------------------------------------------------------------
// Function: SegmentItem::setHeight()
//-----------------------------------------------------------------------------
void SegmentItem::setHeight(int height)
{
    // Restrict the height so that subsections won't go outside the segment.
    if (!subsections_.empty())
    {
        if (subsections_.back()->getEndAddress() < getEndAddress())
        {
            height = qMax(height, subsections_.back()->getBottom() + SUBSECTION_SPACING);
        }
        else if (subsections_.size() >= 2)
        {
            height = qMax(height, subsections_[subsections_.size() - 2]->getBottom() + SUBSECTION_SPACING + SUBSECTION_HEIGHT);
        }
    }

    AddressSectionItem::setHeight(height);

    // Check if we need to move the last subsection (goes to the end of the segment).
    if (!subsections_.empty() && subsections_.back()->getEndAddress() == getEndAddress())
    {
        subsections_.back()->setTop(height - SUBSECTION_HEIGHT);
        subsections_.back()->setBottom(height);
    }
}
