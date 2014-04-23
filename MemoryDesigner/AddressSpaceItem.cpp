//-----------------------------------------------------------------------------
// File: AddressSpaceItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.9.2012
//
// Description:
// Implements the address space item class.
//-----------------------------------------------------------------------------

#include "AddressSpaceItem.h"

#include "AddressSectionItem.h"
#include "SegmentItem.h"
#include "MemoryItem.h"
#include "MemoryColumn.h"
#include "MemoryDesignDiagram.h"

#include <library/LibraryManager/vlnv.h>

#include <common/utils.h>
#include <common/DesignDiagram.h>
#include <common/KactusColors.h>
#include <common/GenericEditProvider.h>
#include <common/layouts/VStackedLayout.h>

#include <models/component.h>
#include <models/addressspace.h>
#include <models/segment.h>

#include <QFont>
#include <QTextDocument>
#include <QPainter>

namespace
{
    /*!
     *  Sorting operator for sorting segments to ascending address offset order.
     */
    bool segmentSortOp(QSharedPointer<Segment const> lhs, QSharedPointer<Segment const> rhs)
    {
        return Utils::str2Uint(lhs->getAddressOffset()) < Utils::str2Uint(rhs->getAddressOffset());
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceItem()
//-----------------------------------------------------------------------------
AddressSpaceItem::AddressSpaceItem(LibraryInterface* libInterface, QString const& instanceName,
                                   QSharedPointer<Component> component,
                                   QSharedPointer<AddressSpace> addressSpace, QGraphicsItem *parent)
    : MemoryBaseItem(parent),
      libInterface_(libInterface),
      instanceName_(instanceName),
      component_(component),
      addressSpace_(addressSpace),
      nameLabel_(0),
      aubLabel_(),
      sectionLayout_(new VStackedLayout<AddressSectionItem>(SPACING)),
      sections_()
{
    Q_ASSERT_X(component, "AddressSpaceItem constructor",
               "Null component-pointer given as parameter");

    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsSelectable);
    setFlag(ItemIsMovable);
    setRect(QRectF(-WIDTH / 2, 0, NAME_COLUMN_WIDTH, 80));

    // Create the name label.
    nameLabel_ = new QGraphicsTextItem(addressSpace->getName(), this);
    QFont font = nameLabel_->font();
    font.setStyleStrategy(QFont::NoAntialias);
    font.setWeight(QFont::Bold);
    nameLabel_->setFont(font);
    nameLabel_->setRotation(-90.0);

    // Create the AUB label.
    aubLabel_ = new QGraphicsTextItem(this);
    font = aubLabel_->font();
    font.setPixelSize(9);
    font.setWeight(QFont::Bold);
    aubLabel_->setFont(font);
    aubLabel_->setTextWidth(NAME_COLUMN_WIDTH + 2);
    aubLabel_->setPos(-WIDTH / 2, 0.0);
    aubLabel_->setHtml(QString("<center>AUB<br>") + QString::number(addressSpace->getAddressUnitBits()) +
                       QString("</center>"));

    // Parse segments and add a section for each of them.
    // Unsegmented parts are also visualized.
    quint64 curAddress = 0;

    // Sort address spaces based on their start address.
    QList< QSharedPointer<Segment> > segments = addressSpace_->getSegments();
    qSort(segments.begin(), segments.end(), &segmentSortOp);

    foreach (QSharedPointer<Segment> segment, segments)
    {
        quint64 startAddress = Utils::str2Uint(segment->getAddressOffset());
        quint64 range = Utils::str2Uint(segment->getRange());

        // Check if there is unsegmented area before the segment.
        if (startAddress > curAddress)
        {
            AddressSectionItem* section = new SegmentItem(component_, addressSpace_,
                                                          tr("unsegmented"), curAddress, 
                                                          startAddress - curAddress, this);
            section->setColor(KactusColors::ADDRESS_SEGMENT_UNSEGMENTED);
            section->setPos(0.0, getHeight());
            addItem(section);
        }

        // Add the actual segment section.
        AddressSectionItem* section = new SegmentItem(component_, addressSpace_,
            segment->getName(), startAddress, range, this);
        section->setColor(KactusColors::ADDRESS_SEGMENT);
        section->setPos(0.0, getHeight());
        addItem(section);

        curAddress = startAddress + range;
    }

    quint64 range = Utils::str2Uint(addressSpace_->getRange());

    if (curAddress < range)
    {
        // Add unsegmented section which takes the rest of the address space.
        AddressSectionItem* section = new SegmentItem(component_, addressSpace_, tr("unsegmented"),
                                                      curAddress, range - curAddress, this);
        section->setColor(KactusColors::ADDRESS_SEGMENT_UNSEGMENTED);
        section->setPos(0.0, getHeight());
        addItem(section);
    }

    updateNameLabel(instanceName + "<br>" + addressSpace->getName());
    updateVisuals();
    updateSize();
}

//-----------------------------------------------------------------------------
// Function: ~AddressSpaceItem()
//-----------------------------------------------------------------------------
AddressSpaceItem::~AddressSpaceItem()
{
    emit destroyed(this);
}

//-----------------------------------------------------------------------------
// Function: updateComponent()
//-----------------------------------------------------------------------------
void AddressSpaceItem::updateVisuals()
{
    //     VLNV* vlnv = component_->getVlnv();
    //     QString toolTipText = "";
    //     setToolTip(toolTipText);

    if (component_->isBus())
    {
        setBrush(QBrush(KactusColors::HW_BUS_COMPONENT)); 
    }
    else
    {
        setBrush(QBrush(KactusColors::HW_COMPONENT));
    }
}

//-----------------------------------------------------------------------------
// Function: itemChange()
//-----------------------------------------------------------------------------
QVariant AddressSpaceItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange)
    {
        QPointF newPos = value.toPointF();
        return snapPointToGrid(newPos);
    }

    return QGraphicsItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: updateNameLabel()
//-----------------------------------------------------------------------------
void AddressSpaceItem::updateNameLabel(QString const& text)
{
    nameLabel_->setHtml("<center>" + text + "</center>");
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceItem::getLibraryInterface()
//-----------------------------------------------------------------------------
LibraryInterface* AddressSpaceItem::getLibraryInterface()
{
    return libInterface_;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceItem::getAddressSpace()
//-----------------------------------------------------------------------------
QSharedPointer<AddressSpace> AddressSpaceItem::getAddressSpace()
{
    return addressSpace_;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceItem::getAddressSpace()
//-----------------------------------------------------------------------------
QSharedPointer<AddressSpace const> AddressSpaceItem::getAddressSpace() const
{
    return addressSpace_;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceItem::getParentStack()
//-----------------------------------------------------------------------------
IGraphicsItemStack* AddressSpaceItem::getParentStack()
{
    return dynamic_cast<IGraphicsItemStack*>(parentItem());
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceItem::addItem()
//-----------------------------------------------------------------------------
void AddressSpaceItem::addItem(QGraphicsItem* item, bool /*load*/)
{
    // Map the position to the column's local coordinate system
    // and constrain the item to the horizontal center of the column.
    QPointF pos = mapFromScene(item->scenePos());
    pos.setX(0.0);

    item->setParentItem(this);
    item->setPos(pos);

    item->setFlag(ItemStacksBehindParent, false);

    AddressSectionItem* section = static_cast<AddressSectionItem*>(item);

    //     if (load)
    //     {
    //         // Place the item at the correct index in the items list.
    //         for (int i = 0; i < sections_.size(); ++i)
    //         {
    //             if (item->y() < sections_.at(i)->y())
    //             {
    //                 sections_.insert(i, compItem);
    // 
    //                 offsetPortPositions(getHeight() + SPACING);
    //                 updateSize();
    //                 return;
    //             }
    //         }
    // 
    //         // If we got here, the item must be placed at the end of the list.
    //         sections_.append(compItem);
    // 
    //         offsetPortPositions(getHeight() + SPACING);
    //         updateSize();
    //     }
    //     else
    {
        sections_.append(section);
        sectionLayout_->updateItemMove(sections_, section, 0.0);
        sectionLayout_->setItemPos(sections_, section, SECTION_X, 0.0);
        updateItemPositions();
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceItem::removeItem()
//-----------------------------------------------------------------------------
void AddressSpaceItem::removeItem(QGraphicsItem* item)
{
    sections_.removeAll(static_cast<AddressSectionItem*>(item));
    item->setParentItem(0);
    updateItemPositions();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceItem::onMoveItem()
//-----------------------------------------------------------------------------
void AddressSpaceItem::onMoveItem(QGraphicsItem* item)
{
    AddressSectionItem* section = static_cast<AddressSectionItem*>(item);

    sectionLayout_->updateItemMove(sections_, section, 0.0);
    updateSize();

    // Check if the item is not overlapping the HW mapping item enough.
    //     QRectF intersection = sceneBoundingRect().intersected(item->sceneBoundingRect());
    // 
    //     // Only non-imported SW components can be moved out of the HW mapping item.
    //     if (!static_cast<SystemAddressSectionItem*>(compItem)->isImported() &&
    //         compItem->rect().height() - intersection.height() >= 3 * GridSize)
    //     {
    //         sections_.removeAll(compItem);
    // 
    //         // Let the parent component stack handle the mouse move.
    //         IGraphicsItemStack* parentStack = dynamic_cast<IGraphicsItemStack*>(parentItem());
    //         Q_ASSERT(parentStack != 0);
    // 
    //         QPointF newPos = parentStack->mapStackFromScene(compItem->scenePos());
    //         compItem->setParentItem(parentItem());
    //         compItem->setPos(newPos);
    //         compItem->setFlag(ItemStacksBehindParent);
    // 
    //         updateItemPositions();
    //         setZValue(0.0);
    // 
    //         parentStack->onMoveItem(compItem);
    //         return;
    //     }

    setZValue(1001.0);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceItem::onReleaseItem()
//-----------------------------------------------------------------------------
void AddressSpaceItem::onReleaseItem(QGraphicsItem* item)
{
    setZValue(0.0);
    sectionLayout_->setItemPos(sections_, static_cast<AddressSectionItem*>(item), SECTION_X, 0.0);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceItem::updateItemPositions()
//-----------------------------------------------------------------------------
void AddressSpaceItem::updateItemPositions()
{
    // Just update the item positions.
    sectionLayout_->updateItemPositions(sections_, SECTION_X, 0.0);
    updateSize();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceItem::mapStackToScene()
//-----------------------------------------------------------------------------
QPointF AddressSpaceItem::mapStackToScene(QPointF const& pos) const
{
    return mapToScene(pos);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceItem::mapStackFromScene()
//-----------------------------------------------------------------------------
QPointF AddressSpaceItem::mapStackFromScene(QPointF const& pos) const
{
    return mapFromScene(pos);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceItem::isItemAllowed()
//-----------------------------------------------------------------------------
bool AddressSpaceItem::isItemAllowed(QGraphicsItem* item) const
{
    return (item->type() == AddressSectionItem::Type);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceItem::getComponentStackHeight()
//-----------------------------------------------------------------------------
qreal AddressSpaceItem::getHeight() const
{
    if (sections_.empty())
    {
        return MIN_HEIGHT;
    }

    qreal stackHeight = 0;

    foreach (AddressSectionItem* item, sections_)
    {
        stackHeight += item->boundingRect().height();
    }

    stackHeight += (sections_.size() - 1) * SPACING;
    return stackHeight;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceItem::getContentType()
//-----------------------------------------------------------------------------
ColumnContentType AddressSpaceItem::getContentType() const
{
    return COLUMN_CONTENT_COMPONENTS;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceItem::updateSize()
//-----------------------------------------------------------------------------
void AddressSpaceItem::updateSize()
{
    QRectF oldRect = rect();
    oldRect.setHeight(getHeight());

    setRect(oldRect);

    IGraphicsItemStack* stack = dynamic_cast<IGraphicsItemStack*>(parentItem());

    if (stack != 0)
    {
        stack->updateItemPositions();
    }

    nameLabel_->setTextWidth(getHeight());
    nameLabel_->setPos(-WIDTH / 2 + 3, getHeight());
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void AddressSpaceItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Discard movement if the diagram is protected.
    if (static_cast<DesignDiagram*>(scene())->isProtected())
    {
        return;
    }

    QGraphicsRectItem::mouseMoveEvent(event);

    if (oldColumn_ != 0)
    {
        setPos(parentItem()->mapFromScene(oldColumn_->mapToScene(pos())));

        MemoryColumn* column = dynamic_cast<MemoryColumn*>(parentItem());
        Q_ASSERT(column != 0);
        column->onMoveItem(this);
    }
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void AddressSpaceItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsRectItem::mouseReleaseEvent(event);
    setZValue(0.0);

    if (oldColumn_ != 0)
    {
        MemoryColumn* column = dynamic_cast<MemoryColumn*>(parentItem());
        Q_ASSERT(column != 0);
        column->onReleaseItem(this);

//         QSharedPointer<QUndoCommand> cmd;
// 
//         if (scenePos() != oldPos_)
//         {
//             cmd = QSharedPointer<QUndoCommand>(new ItemMoveCommand(this, oldPos_, oldColumn_));
//         }
//         else
//         {
//             cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
//         }
// 
//         // Add the undo command to the edit stack only if it has at least some real changes.
//         if (scenePos() != oldPos_)
//         {
//             static_cast<DesignDiagram*>(scene())->getEditProvider().addCommand(cmd, false);
//         }

        oldColumn_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void AddressSpaceItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mousePressEvent(event);
    setZValue(1001.0);

    oldPos_ = scenePos();
    oldColumn_ = dynamic_cast<MemoryColumn*>(parentItem());
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceItem::getSections()
//-----------------------------------------------------------------------------
QList<AddressSectionItem*> const& AddressSpaceItem::getSections() const
{
    return sections_;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceItem::convertAddress()
//-----------------------------------------------------------------------------
quint64 AddressSpaceItem::convertAddress(quint64 address, MemoryBaseItem* source) const
{
    // Conversion is possible only if the source is a memory map.
    MemoryItem* memoryMapItem = dynamic_cast<MemoryItem*>(source);

    if (memoryMapItem == 0)
    {
        return address;
    }

    quint64 addressOffset = 0;

    if (!static_cast<MemoryDesignDiagram*>(scene())->isConnected(this, memoryMapItem, &addressOffset))
    {
        return address;
    }

    return address + addressOffset;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceItem::getComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component const> AddressSpaceItem::getComponent() const
{
    return component_;
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceItem::getInstanceName()
//-----------------------------------------------------------------------------
QString const& AddressSpaceItem::getInstanceName() const
{
    return instanceName_;
}
