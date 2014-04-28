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
#include "MemoryDesignDiagram.h"

#include <common/utils.h>
#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/DesignDiagram.h>

#include <IPXACTmodels/addressblock.h>

#include <QFont>
#include <QPainter>
#include <QApplication>
#include <QGraphicsSceneHoverEvent>

//-----------------------------------------------------------------------------
// Function: AddressSectionItem::AddressSectionItem()
//-----------------------------------------------------------------------------
AddressSectionItem::AddressSectionItem(QString const& name, quint64 startAddress,
                                       quint64 range, QGraphicsItem* parent)
    : MemoryBaseItem(parent),
      name_(name),
      startAddress_(startAddress),
      range_(range),
      nameLabel_(new QGraphicsTextItem(this)),
      usageType_(USAGE_UNSPECIFIED),
      typeLabel_(new QGraphicsTextItem(this)),
      mouseNearResizeArea_(false),
      startAddressLabel_(new QGraphicsTextItem(this)),
      endAddressLabel_(new QGraphicsTextItem(this))
{
    setRect(-WIDTH / 2, 0, WIDTH, MIN_SECTION_HEIGHT);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsSelectable);
    setAcceptHoverEvents(true);

    QFont font = nameLabel_->font();
    font.setWeight(QFont::Bold);
    font.setStyleStrategy(QFont::NoAntialias);

    nameLabel_->setFont(font);
    nameLabel_->setAcceptHoverEvents(false);
    nameLabel_->setTextWidth(WIDTH - ADDR_COLUMN_WIDTH);
    nameLabel_->setHtml(QString("<center>") + name_ + "</center>");
    nameLabel_->setRotation(-90.0);

    // Create the usage type label.
    font = typeLabel_->font();
    font.setPixelSize(9);
    font.setWeight(QFont::Bold);
    typeLabel_->setFont(font);
    typeLabel_->setTextWidth(WIDTH - ADDR_COLUMN_WIDTH + 2);
    typeLabel_->setPos(-WIDTH / 2, 0.0);
    typeLabel_->setHtml(QString("<center></center>"));

    // Create labels for addresses.
    font = startAddressLabel_->font();
    font.setWeight(QFont::Bold);

    startAddressLabel_->setFont(font);
    startAddressLabel_->setAcceptHoverEvents(false);
    startAddressLabel_->setPos(QPointF(WIDTH / 2 - ADDR_COLUMN_WIDTH, 0.0));
    startAddressLabel_->setHtml(toHexString(startAddress_));

    endAddressLabel_->setFont(font);
    endAddressLabel_->setAcceptHoverEvents(false);
    endAddressLabel_->setPos(QPointF(WIDTH / 2 - ADDR_COLUMN_WIDTH, MIN_SECTION_HEIGHT - 20.0));
    endAddressLabel_->setHtml(toHexString(startAddress_ + range_ - 1));

    setHeight(MIN_SECTION_HEIGHT);
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
        event->pos().x() >= WIDTH / 2 - ADDR_COLUMN_WIDTH &&
        qAbs(event->pos().y() - boundingRect().bottom()) < 10)
    {
        if (!mouseNearResizeArea_)
        {
            QApplication::setOverrideCursor(Qt::SizeVerCursor);
            mouseNearResizeArea_ = true;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSectionItem::hoverLeaveEvent()
//-----------------------------------------------------------------------------
void AddressSectionItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsRectItem::hoverLeaveEvent(event);

    if (mouseNearResizeArea_)
    {
        // Restore the old cursor.
        QApplication::restoreOverrideCursor();
        mouseNearResizeArea_ = false;
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void AddressSectionItem::mousePressEvent(QGraphicsSceneMouseEvent* /*event*/)
{
    if (mouseNearResizeArea_)
    {
        // TODO: Save the old height before resize.

//         qreal sceneBottom = mapToScene(QPointF(0.0, subsections_.at(resizeIndex_)->getBottom())).y();
//         static_cast<MemoryDesignDiagram*>(scene())->beginResizeSubsection(resizeIndex_ < subsections_.size() - 1, sceneBottom);
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
    if (mouseNearResizeArea_)
    {
        // Calculate the new bottom value for the resized subsection.
        int bottom = (static_cast<int>(event->pos().y() - 5.0) / 10) * 10;

        // Restrict so that the subsection does not shrink too small...
        bottom = qMax<int>(bottom, MIN_SECTION_HEIGHT);
        setHeight(bottom);

        //qreal sceneBottom = mapToScene(QPointF(0.0, bottom)).y();
        //static_cast<MemoryDesignDiagram*>(scene())->updateResizeSubsection(sceneBottom);
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
    if (mouseNearResizeArea_)
    {
        //static_cast<MemoryDesignDiagram*>(scene())->endResizeSubsection();
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
        event->pos().x() >= WIDTH / 2 - ADDR_COLUMN_WIDTH &&
        qAbs(event->pos().y() - boundingRect().bottom()) < 10)
    {
        if (!mouseNearResizeArea_)
        {
            QApplication::setOverrideCursor(Qt::SizeVerCursor);
            mouseNearResizeArea_ = true;
        }

        return;
    }

    if (mouseNearResizeArea_)
    {
        // Restore the old cursor.
        QApplication::restoreOverrideCursor();
        mouseNearResizeArea_ = false;
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

    nameLabel_->setTextWidth(height);
    nameLabel_->setPos(-WIDTH / 2 + GridSize / 2, height);

    startAddressLabel_->setPos(QPointF(WIDTH / 2 - ADDR_COLUMN_WIDTH, 0.0));
    endAddressLabel_->setPos(QPointF(WIDTH / 2 - ADDR_COLUMN_WIDTH, height - 20.0));
}

//-----------------------------------------------------------------------------
// Function: AddressSectionItem::mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void AddressSectionItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* /*event*/)
{
    // Check if the user double clicked in the address area and a new split should be performed.
//     if (!static_cast<DesignDiagram*>(scene())->isProtected() &&
//         event->pos().x() >= WIDTH / 2 - ADDR_COLUMN_WIDTH)
//     {
//         // Determine which subsection should be split.
//         int index = getSubsectionAt(event->pos());
//         AddressSubsection* subsection = subsections_.at(index).data();
// 
//         // Determine the new bottom y coordinate for the original subsection.
//         int bottom1 = qMax(20, static_cast<int>(event->pos().y() - 5.0) / 10 * 10);
//         int bottom2 = subsection->getBottom();
//         int top2 = bottom1 + SPACING;
// 
//         // Divide the address range in half by default.
//         quint64 startAddress2 = subsection->getStartAddress() + subsection->getRange() / 2;
//         quint64 endAddress2 = subsection->getEndAddress();
// 
//         // Shrink the old subsection and create the new one.
//         subsection->setBottom(bottom1);
//         subsection->setEndAddress(startAddress2 - 1);
// 
//         QSharedPointer<AddressSubsection> newSubsection(new AddressSubsection(this, WIDTH / 2 - ADDR_COLUMN_WIDTH,
//                                                                               top2, bottom2, startAddress2, endAddress2));
//         newSubsection->setMinStartAddress(subsection->getStartAddress() + 1);
// 
//         connect(newSubsection.data(), SIGNAL(startAddressEdited(AddressSubsection*)),
//                 this, SLOT(onSubsectionStartAddressEdited(AddressSubsection*)), Qt::UniqueConnection);
// 
//         subsections_.insert(index + 1, newSubsection);
//         fixSubsectionHeights();
//     }
}

//-----------------------------------------------------------------------------
// Function: AddressSectionItem::drawGuides()
//-----------------------------------------------------------------------------
void AddressSectionItem::drawGuides(QPainter* painter, QRectF const& rect) const
{
    QPen pen(Qt::black, 0);
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);

    painter->drawLine(rect.left(), sceneBoundingRect().top(),
                      rect.right(), sceneBoundingRect().top());

    painter->drawLine(rect.left(), sceneBoundingRect().bottom(),
                      rect.right(), sceneBoundingRect().bottom());
}

//-----------------------------------------------------------------------------
// Function: AddressSectionItem::setUsageType()
//-----------------------------------------------------------------------------
void AddressSectionItem::setUsageType(UsageType usageType)
{
    switch (usageType)
    {
    case USAGE_READ_ONLY:
        {
            typeLabel_->setHtml(QString("<center>R</center>"));
            typeLabel_->setToolTip(tr("Read-only"));
            break;
        }

    case USAGE_READ_WRITE:
        {
            typeLabel_->setHtml(QString("<center>RW</center>"));
            typeLabel_->setToolTip(tr("Read-write"));
            break;
        }

    case USAGE_READ_WRITE_ONCE:
        {
            typeLabel_->setHtml(QString("<center>RWO</center>"));
            typeLabel_->setToolTip(tr("Read-write-once"));
            break;
        }

    case USAGE_REGISTERS:
        {
            typeLabel_->setHtml(QString("<center>REG</center>"));
            typeLabel_->setToolTip(tr("Registers"));
            break;
        }

    default:
        {
            typeLabel_->setHtml(QString("<center></center>"));
            typeLabel_->setToolTip("");
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSubsection::toHexString()
//-----------------------------------------------------------------------------
QString AddressSectionItem::toHexString(quint64 address)
{
    return QString("0x") + QString("%1").arg(address, 8, 16, QChar('0')).toUpper();
}

//-----------------------------------------------------------------------------
// Function: AddressSectionItem::getStartAddress()
//-----------------------------------------------------------------------------
quint64 AddressSectionItem::getStartAddress() const
{
    return startAddress_;
}

//-----------------------------------------------------------------------------
// Function: AddressSectionItem::getEndAddress()
//-----------------------------------------------------------------------------
quint64 AddressSectionItem::getEndAddress() const
{
    return (startAddress_ + range_ - 1);
}

//-----------------------------------------------------------------------------
// Function: AddressSectionItem::drawStartAddressDivider()
//-----------------------------------------------------------------------------
void AddressSectionItem::drawStartAddressDivider(QPainter* painter, QRectF const& /*rect*/, int y, quint64 address) const
{
    if (y == static_cast<int>(sceneBoundingRect().top()) || y == static_cast<int>(sceneBoundingRect().bottom()))
    {
        return;
    }

    // Transform local left and right x coordinates to scene coordinates.
    qreal left = mapToScene(WIDTH / 2 - ADDR_COLUMN_WIDTH, 0.0).x();
    qreal right = mapToScene(WIDTH / 2, 0.0).x();

    QPen pen(Qt::black, 0);
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);

    // Draw the divider line.
    painter->drawLine(left, y, right, y);

    // Draw the address as text.
    painter->drawText(left + 2, y + 12, toHexString(address));
}

//-----------------------------------------------------------------------------
// Function: AddressSectionItem::drawEndAddressDivider()
//-----------------------------------------------------------------------------
void AddressSectionItem::drawEndAddressDivider(QPainter* painter, QRectF const& /*rect*/, int y, quint64 address) const
{
    if (y == static_cast<int>(sceneBoundingRect().top()) || y == static_cast<int>(sceneBoundingRect().bottom()))
    {
        return;
    }

    // Transform local left and right x coordinates to scene coordinates.
    qreal left = mapToScene(WIDTH / 2 - ADDR_COLUMN_WIDTH, 0.0).x();
    qreal right = mapToScene(WIDTH / 2, 0.0).x();

    QPen pen(Qt::black, 0);
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);

    // Draw the divider line.
    painter->drawLine(left, y, right, y);

    // Draw the address as text.
    painter->drawText(left + 2, y - 2, toHexString(address));
}

//-----------------------------------------------------------------------------
// Function: AddressSectionItem::setColor()
//-----------------------------------------------------------------------------
void AddressSectionItem::setColor(QColor const& color)
{
    setBrush(QBrush(color));

    startAddressLabel_->setHtml("<div style=\"background-color:" + color.name() + 
                                "; padding:10px 10px;\"><center>" +
                                toHexString(startAddress_) + "</center></div>");
    endAddressLabel_->setHtml("<div style=\"background-color:" + color.name() + 
                              "; padding:10px 10px;\"><center>" +
                              toHexString(startAddress_ + range_ - 1) + "</center></div>");
}
