//-----------------------------------------------------------------------------
// File: AddressSectionItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 18.9.2012
//
// Description:
// Implements the address block section class.
//-----------------------------------------------------------------------------

#include "AddressSectionItem.h"

#include "MemoryItem.h"
#include "AddressSubsection.h"

#include <common/utils.h>
#include <common/diagramgrid.h>
#include <common/DesignDiagram.h>

#include <models/addressblock.h>

#include <QFont>
#include <QPainter>
#include <QApplication>
#include <QGraphicsSceneHoverEvent>

//-----------------------------------------------------------------------------
// Function: AddressSectionItem::AddressSectionItem()
//-----------------------------------------------------------------------------
AddressSectionItem::AddressSectionItem(QString const& name, unsigned int startAddress,
                                       unsigned int range, QGraphicsItem* parent)
    : QGraphicsRectItem(QRectF(-WIDTH / 2, 0, WIDTH, DEFAULT_SECTION_HEIGHT), parent),
      name_(name),
      startAddress_(startAddress),
      range_(range),
      nameLabel_(new QGraphicsTextItem(this)),
      resizeIndex_(-1)
{
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsSelectable);
    setAcceptHoverEvents(true);

    setBrush(QBrush(QColor(0xa5,0xc3,0xef)));

    QFont font = nameLabel_->font();
    font.setWeight(QFont::Bold);

    nameLabel_->setFont(font);
    nameLabel_->setAcceptHoverEvents(false);
    nameLabel_->setTextWidth(WIDTH - ADDR_COLUMN_WIDTH);
    nameLabel_->setPos(QPointF(-WIDTH / 2, 0));
    nameLabel_->setHtml(QString("<center>") + name_ + "</center>");

    // Add the initial subsection which occupies the whole section.
    QSharedPointer<AddressSubsection> subsection(new AddressSubsection(this, WIDTH / 2 - ADDR_COLUMN_WIDTH,
                                                                       0, DEFAULT_SECTION_HEIGHT,
                                                                       startAddress_, startAddress_ + range_ - 1));
    subsection->setStartAddressFixed(true);
    subsections_.append(subsection);
}

//-----------------------------------------------------------------------------
// Function: AddressSectionItem::~AddressSectionItem()
//-----------------------------------------------------------------------------
AddressSectionItem::~AddressSectionItem()
{
}

//-----------------------------------------------------------------------------
// Function: AddressSectionItem::paint()
//-----------------------------------------------------------------------------
void AddressSectionItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsRectItem::paint(painter, option, widget);

    // Draw the vertical address divider.
    painter->setPen(QPen(Qt::black));
    painter->drawLine(QPointF(WIDTH / 2 - ADDR_COLUMN_WIDTH, rect().top()),
                      QPointF(WIDTH / 2 - ADDR_COLUMN_WIDTH, rect().bottom()));

    // Draw separator lines for the subsections.
    QPen pen(Qt::black);
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);

    if (subsections_.size() > 1)
    {
        painter->drawLine(QPointF(WIDTH / 2 - ADDR_COLUMN_WIDTH, subsections_.first()->getBottom()),
                          QPointF(WIDTH / 2, subsections_.first()->getBottom()));
    }

    for (int i = 1; i < subsections_.size() - 1; ++i)
    {
        painter->drawLine(QPointF(WIDTH / 2 - ADDR_COLUMN_WIDTH, subsections_.at(i)->getTop()),
                          QPointF(WIDTH / 2, subsections_.at(i)->getTop()));

        painter->drawLine(QPointF(WIDTH / 2 - ADDR_COLUMN_WIDTH, subsections_.at(i)->getBottom()),
                          QPointF(WIDTH / 2, subsections_.at(i)->getBottom()));
    }

    if (subsections_.size() > 1)
    {
        painter->drawLine(QPointF(WIDTH / 2 - ADDR_COLUMN_WIDTH, subsections_.last()->getTop()),
                          QPointF(WIDTH / 2, subsections_.last()->getTop()));
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSectionItem::hoverMoveEvent()
//-----------------------------------------------------------------------------
void AddressSectionItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    QGraphicsRectItem::hoverMoveEvent(event);
    updateCursor(event);
}

//-----------------------------------------------------------------------------
// Function: AddressSectionItem::hoverEnterEvent()
//-----------------------------------------------------------------------------
void AddressSectionItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    QGraphicsRectItem::hoverEnterEvent(event);

    if (!static_cast<DesignDiagram*>(scene())->isProtected() &&
        event->pos().x() >= WIDTH / 2 - ADDR_COLUMN_WIDTH)
    {
        for (int i = 0; i < subsections_.size(); ++i)
        {
            if (std::abs(event->pos().y() - (subsections_.at(i)->getBottom() + 5)) <= 10)
            {
                if (resizeIndex_ == -1)
                {
                    QApplication::setOverrideCursor(Qt::SizeVerCursor);
                    resizeIndex_ = i;
                }

                break;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSectionItem::hoverLeaveEvent()
//-----------------------------------------------------------------------------
void AddressSectionItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsRectItem::hoverLeaveEvent(event);

    if (resizeIndex_ != -1)
    {
        // Restore the old cursor.
        QApplication::restoreOverrideCursor();
        resizeIndex_ = -1;
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void AddressSectionItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (resizeIndex_ != -1)
    {
        // TODO: Save the old height before resize.
    }
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void AddressSectionItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Discard mouse move if the diagram is protected.
    if (static_cast<DesignDiagram*>(scene())->isProtected())
    {
        return;
    }

    // If the mouse is moved near the resize area, change the column's width accordingly.
    if (resizeIndex_ != -1)
    {
        // Calculate the new bottom value for the resized subsection.
        int bottom = (static_cast<int>(event->pos().y() - 5.0) / 10) * 10;

        // Restrict so that the subsection does not shrink too small...
        bottom = qMax(bottom, subsections_.at(resizeIndex_)->getTop() + MIN_SUBSECTION_HEIGHT);

        // ...or make the next one too small.
        if (resizeIndex_ < subsections_.size() - 1)
        {
            bottom = qMin(bottom, subsections_.at(resizeIndex_ + 1)->getBottom() - MIN_SUBSECTION_HEIGHT - SPACING);
        }

        // Apply resize.
        subsections_.at(resizeIndex_)->setBottom(bottom);

        if (resizeIndex_ < subsections_.size() - 1)
        {
            subsections_.at(resizeIndex_ + 1)->setTop(bottom + SPACING);
        }

        setHeight(subsections_.last()->getBottom());
    }
    else
    {
        // Otherwise handle the movement of the column.
        QGraphicsRectItem::mouseMoveEvent(event);

//         setZValue(1001.0);
//         layout_->onMoveColumn(this);
    }
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void AddressSectionItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (resizeIndex_ != -1)
    {
//         if (desc_.getWidth() != oldWidth_)
//         {
//             QSharedPointer<QUndoCommand> cmd(new AddressSectionItemResizeCommand(this, oldWidth_));
//             static_cast<DesignDiagram*>(scene())->getEditProvider().addCommand(cmd, false);
//         }
    }
    else
    {
        QGraphicsRectItem::mouseReleaseEvent(event);

//         setZValue(0.0);
//         layout_->onReleaseColumn(this);
// 
//         // Create an undo command.
//         QSharedPointer<QUndoCommand> cmd = createMoveUndoCommand();
// 
//         // Add the undo command to the edit provider only if there were any actual changes.
//         if (cmd->childCount() > 0 || pos() != oldPos_)
//         {
//             static_cast<DesignDiagram*>(scene())->getEditProvider().addCommand(cmd, false);
//         }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSectionItem::updateCursor()
//-----------------------------------------------------------------------------
void AddressSectionItem::updateCursor(QGraphicsSceneHoverEvent* event)
{
    if (!static_cast<DesignDiagram*>(scene())->isProtected() &&
        event->pos().x() >= WIDTH / 2 - ADDR_COLUMN_WIDTH)
    {
        for (int i = 0; i < subsections_.size(); ++i)
        {
            if (std::abs(event->pos().y() - (subsections_.at(i)->getBottom() + 5)) <= 10)
            {
                if (resizeIndex_ == -1)
                {
                    QApplication::setOverrideCursor(Qt::SizeVerCursor);
                    resizeIndex_ = i;
                }

                return;
            }
        }
    }

    if (resizeIndex_ != -1)
    {
        // Restore the old cursor.
        QApplication::restoreOverrideCursor();
        resizeIndex_ = -1;
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSectionItem::setHeight()
//-----------------------------------------------------------------------------
void AddressSectionItem::setHeight(int height)
{
    setRect(QRectF(-WIDTH / 2, 0, WIDTH, height));

    IGraphicsItemStack* stack = dynamic_cast<IGraphicsItemStack*>(parentItem());

    if (stack != 0)
    {
        stack->updateItemPositions();
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSectionItem::mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void AddressSectionItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    // Check if the user double clicked in the address area and a new split should be performed.
    if (!static_cast<DesignDiagram*>(scene())->isProtected() &&
        event->pos().x() >= WIDTH / 2 - ADDR_COLUMN_WIDTH)
    {
        // Determine which subsection should be split.
        int index = getSubsectionAt(event->pos());
        AddressSubsection* subsection = subsections_.at(index).data();

        // Determine the new bottom y coordinate for the original subsection.
        int bottom1 = qMax(20, static_cast<int>(event->pos().y() - 5.0) / 10 * 10);
        int bottom2 = subsection->getBottom();
        int top2 = bottom1 + SPACING;

        // Divide the address range in half by default.
        unsigned int startAddress2 = subsection->getStartAddress() + subsection->getRange() / 2;
        unsigned int endAddress2 = subsection->getEndAddress();

        // Shrink the old subsection and create the new one.
        subsection->setBottom(bottom1);
        subsection->setEndAddress(startAddress2 - 1);

        QSharedPointer<AddressSubsection> newSubsection(new AddressSubsection(this, WIDTH / 2 - ADDR_COLUMN_WIDTH,
                                                                              top2, bottom2, startAddress2, endAddress2));
        newSubsection->setMinStartAddress(subsection->getStartAddress() + 1);

        connect(newSubsection.data(), SIGNAL(startAddressEdited(AddressSubsection*)),
                this, SLOT(onSubsectionStartAddressEdited(AddressSubsection*)), Qt::UniqueConnection);

        subsections_.insert(index + 1, newSubsection);
        fixSubsectionHeights();
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSectionItem::getSubsectionAt()
//-----------------------------------------------------------------------------
int AddressSectionItem::getSubsectionAt(QPointF const& pos) const
{
    int index = 0;

    for (; index < subsections_.size(); ++index)
    {
        if (pos.y() >= subsections_.at(index)->getTop() &&
            pos.y() < subsections_.at(index)->getBottom())
        {
            break;
        }
    }
    
    return index;
}

//-----------------------------------------------------------------------------
// Function: AddressSectionItem::fixSubsectionHeights()
//-----------------------------------------------------------------------------
void AddressSectionItem::fixSubsectionHeights()
{
    for (int i = 0; i < subsections_.size(); ++i)
    {
        AddressSubsection* subsection = subsections_.at(i).data();
        
        if (subsection->getBottom() - subsection->getTop() < MIN_SUBSECTION_HEIGHT)
        {
            subsection->setBottom(subsection->getTop() + MIN_SUBSECTION_HEIGHT);
            
            if (i < subsections_.size() - 1)
            {
                subsections_.at(i + 1)->setTop(subsection->getBottom() + SPACING);
            }
        }
    }

    setHeight(subsections_.last()->getBottom());
}

//-----------------------------------------------------------------------------
// Function: AddressSectionItem::onSubsectionStartAddressEdited()
//-----------------------------------------------------------------------------
void AddressSectionItem::onSubsectionStartAddressEdited(AddressSubsection* subsection)
{
    // Find the index of the subsection.
    for (int i = 0; i < subsections_.size(); ++i)
    {
        if (subsections_.at(i) == subsection)
        {
            subsections_.at(i - 1)->setEndAddress(subsection->getStartAddress() - 1);

            if (i < subsections_.size() - 1)
            {
                subsections_.at(i + 1)->setMinStartAddress(subsection->getStartAddress() + 1);
            }
            break;
        }
    }
}
