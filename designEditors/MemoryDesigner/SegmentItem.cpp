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

#include <IPXACTmodels/addressblock.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/addressspace.h>

#include <QPainter>

namespace
{
    /*!
     *  Sorting operator for sorting address blocks to ascending base address.
     */
    bool addressBlockSortOp(QSharedPointer<AddressBlock const> lhs, QSharedPointer<AddressBlock const> rhs)
    {
        return Utils::str2Uint(lhs->getBaseAddress()) < Utils::str2Uint(rhs->getBaseAddress());
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
            quint64 blockStartAddress = Utils::str2Uint(block->getBaseAddress());
            quint64 blockEndAddress = blockStartAddress + Utils::str2Uint(block->getRange()) - 1;

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

            if (blockStartAddress <= getStartAddress() && blockEndAddress >= getEndAddress())
            {
                top = 0;
                bottom = rect().height();
            }
            else if (blockStartAddress <= getStartAddress())
            {
                top = 0;
                bottom = SUBSECTION_HEIGHT;
            }
            else if (blockEndAddress >= getEndAddress())
            {
                top = qMax<int>(rect().height() - SUBSECTION_HEIGHT, top);
                bottom = top + SUBSECTION_HEIGHT;
            }

            AddressSubsection* subsection = new AddressSubsection(this, block->getName(),
                                                                  WIDTH / 2 - ADDR_COLUMN_WIDTH,
                                                                  top, bottom,
                                                                  qMax(blockStartAddress, getStartAddress()),
                                                                  qMin(blockEndAddress, getEndAddress()));
            subsections_.append(QSharedPointer<AddressSubsection>(subsection));
        }
    }

    // Expand the segment if it is too small to contain all subsections.
    foreach (QSharedPointer<AddressSubsection> subsection, subsections_)
    {
        if (subsection->getEndAddress() == getEndAddress())
        {
            setHeight(qMax<int>(rect().height(), subsection->getBottom()));
        }
        else if (subsection->getBottom() + SUBSECTION_SPACING > rect().height())
        {
            setHeight(subsection->getBottom() + SUBSECTION_SPACING);
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
        QBrush diagBrush(QColor(0, 0, 0), Qt::BDiagPattern);
        diagBrush.setTransform(painter->transform().inverted());

        painter->fillRect(WIDTH / 2 - ADDR_COLUMN_WIDTH + 1, subsection->getTop(), ADDR_COLUMN_WIDTH - 1,
                          subsection->getBottom() - subsection->getTop(), diagBrush);

        // Write address block name on top of the visualization.
        int centerY = 0;

        if (subsection->getStartAddress() == getStartAddress() && subsection->getEndAddress() == getEndAddress())
        {
            centerY = (subsection->getTop() + subsection->getBottom()) / 2;
        }
        else if (subsection->getStartAddress() == getStartAddress())
        {
            centerY = subsection->getBottom() - 15;
        }
        else if (subsection->getEndAddress() == getEndAddress())
        {
            centerY = subsection->getTop() + 15;
        }
        else
        {
            centerY = (subsection->getTop() + subsection->getBottom()) / 2;
        }

        QRect drawAreaRect(WIDTH / 2 - ADDR_COLUMN_WIDTH, centerY - 5, ADDR_COLUMN_WIDTH, 10);

        QFont font = painter->font();
        font.setBold(true);
        painter->setFont(font);

        QFontMetrics metrics(font);
        painter->fillRect(metrics.boundingRect(drawAreaRect, Qt::AlignCenter, subsection->getName()),
                          brush().color());

        painter->setPen(QPen(Qt::black, 0));
        painter->drawText(drawAreaRect, Qt::AlignCenter, subsection->getName());
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
        else if (subsections_.size() >= 2 &&
                 subsections_[subsections_.size() - 2]->getEndAddress() < getEndAddress())
        {
            height = qMax(height, subsections_[subsections_.size() - 2]->getBottom() + SUBSECTION_SPACING + SUBSECTION_HEIGHT);
        }
    }

    AddressSectionItem::setHeight(height);

    // Check if we need to move any subsections (goes to the end of the segment).
    foreach (QSharedPointer<AddressSubsection> subsection, subsections_)
    {
        if (subsection->getEndAddress() == getEndAddress())
        {
            subsection->setBottom(height);

            if (subsection->getStartAddress() > getStartAddress())
            {
                subsection->setTop(height - SUBSECTION_HEIGHT);
            }
        }
    }
}
