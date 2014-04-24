//-----------------------------------------------------------------------------
// File: MemoryItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.9.2012
//
// Description:
// Implements the memory map item class.
//-----------------------------------------------------------------------------

#include "MemoryItem.h"

#include "AddressSectionItem.h"
#include "AddressBlockItem.h"
#include "AddressSpaceItem.h"
#include "MemoryColumn.h"
#include "MemoryDesignDiagram.h"

#include <library/LibraryManager/vlnv.h>

#include <common/utils.h>
#include <common/DesignDiagram.h>
#include <common/KactusColors.h>
#include <common/layouts/VStackedLayout.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/memorymap.h>
#include <IPXACTmodels/addressblock.h>

#include <QFont>
#include <QTextDocument>
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
// Function: MemoryItem()
//-----------------------------------------------------------------------------
MemoryItem::MemoryItem(LibraryInterface* libInterface, QString const& instanceName,
                       QSharedPointer<Component> component,
                       QSharedPointer<MemoryMap> memoryMap, QGraphicsItem *parent)
    : MemoryBaseItem(parent),
      libInterface_(libInterface),
      instanceName_(instanceName),
      component_(component),
      memoryMap_(memoryMap),
      nameLabel_(0),
      aubLabel_(),
      sectionLayout_(new VStackedLayout<AddressSectionItem>(SPACING)),
      sections_(),
      oldColumn_(0),
      oldPos_()
{
    Q_ASSERT_X(component, "MemoryItem constructor",
        "Null component-pointer given as parameter");

    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsSelectable);
    setFlag(ItemIsMovable);
    setRect(QRectF(-WIDTH / 2, 0, NAME_COLUMN_WIDTH, 80));

    // Create the name label.
    nameLabel_ = new QGraphicsTextItem(memoryMap->getName(), this);
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
    aubLabel_->setHtml(QString("<center>AUB<br>") + QString::number(memoryMap->getAddressUnitBits()) +
                       QString("</center>"));
    
    // Parse address blocks and add a section for each of them.
    // "Holes" are visualized as 'no memory'.

    // First sort address blocks based on their base address.
    QList< QSharedPointer<AddressBlock> > blocks;
    
    foreach (QSharedPointer<MemoryMapItem> item, memoryMap->getItems())
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
        quint64 curAddress = Utils::str2Uint(blocks.first()->getBaseAddress());

        foreach (QSharedPointer<AddressBlock> block, blocks)
        {
            quint64 startAddress = Utils::str2Uint(block->getBaseAddress());
            quint64 range = Utils::str2Uint(block->getRange());

            // Check if there is a hole in the memory before the next address block.
            if (startAddress > curAddress)
            {
                AddressSectionItem* section = new AddressBlockItem(component_, memoryMap_,
                                                                   tr("no memory"), curAddress, 
                                                                   startAddress - curAddress, this);
                section->setColor(KactusColors::ADDRESS_SEGMENT_UNSEGMENTED);
                section->setPos(0.0, getHeight());
                addItem(section);
            }

            AddressSectionItem* section = new AddressBlockItem(component_, memoryMap_,
                                                               block->getName(), startAddress, range, this);
            section->setColor(KactusColors::MEMORY_BLOCK);
            section->setPos(0.0, getHeight());

            if (block->getUsage() == General::MEMORY)
            {
                switch (block->getAccess())
                {
                case General::READ_ONLY:
                    {
                        section->setUsageType(AddressSectionItem::USAGE_READ_ONLY);
                        break;
                    }

                case General::READ_WRITE:
                    {
                        section->setUsageType(AddressSectionItem::USAGE_READ_WRITE);
                        break;
                    }

                case General::READ_WRITEONCE:
                    {
                        section->setUsageType(AddressSectionItem::USAGE_READ_WRITE_ONCE);
                        break;
                    }

                default:
                    {
                        section->setUsageType(AddressSectionItem::USAGE_UNSPECIFIED);
                        break;
                    }
                }
            }
            else if (block->getUsage() == General::REGISTER)
            {
                section->setUsageType(AddressSectionItem::USAGE_REGISTERS);
            }

            addItem(section);

            curAddress = startAddress + range;
        }
    }

    updateNameLabel(component->getVlnv()->getName() + "<br>" + memoryMap->getName());
    updateVisuals();
    updateSize();
}

//-----------------------------------------------------------------------------
// Function: ~MemoryItem()
//-----------------------------------------------------------------------------
MemoryItem::~MemoryItem()
{
    emit destroyed(this);
}

//-----------------------------------------------------------------------------
// Function: updateComponent()
//-----------------------------------------------------------------------------
void MemoryItem::updateVisuals()
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
QVariant MemoryItem::itemChange(GraphicsItemChange change, const QVariant &value)
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
void MemoryItem::updateNameLabel(QString const& text)
{
    nameLabel_->setHtml("<center>" + text + "</center>");
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getLibraryInterface()
//-----------------------------------------------------------------------------
LibraryInterface* MemoryItem::getLibraryInterface()
{
    return libInterface_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getMemoryMap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryMap> MemoryItem::getMemoryMap()
{
    return memoryMap_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getMemoryMap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryMap const> MemoryItem::getMemoryMap() const
{
    return memoryMap_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getParentStack()
//-----------------------------------------------------------------------------
IGraphicsItemStack* MemoryItem::getParentStack()
{
    return dynamic_cast<IGraphicsItemStack*>(parentItem());
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::addItem()
//-----------------------------------------------------------------------------
void MemoryItem::addItem(QGraphicsItem* item, bool /*load*/)
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
// Function: MemoryItem::removeItem()
//-----------------------------------------------------------------------------
void MemoryItem::removeItem(QGraphicsItem* item)
{
    sections_.removeAll(static_cast<AddressSectionItem*>(item));
    item->setParentItem(0);
    updateItemPositions();
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::onMoveItem()
//-----------------------------------------------------------------------------
void MemoryItem::onMoveItem(QGraphicsItem* item)
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
// Function: MemoryItem::onReleaseItem()
//-----------------------------------------------------------------------------
void MemoryItem::onReleaseItem(QGraphicsItem* item)
{
    setZValue(0.0);
    sectionLayout_->setItemPos(sections_, static_cast<AddressSectionItem*>(item), SECTION_X, 0.0);
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::updateItemPositions()
//-----------------------------------------------------------------------------
void MemoryItem::updateItemPositions()
{
    // Just update the item positions.
    sectionLayout_->updateItemPositions(sections_, SECTION_X, 0.0);
    updateSize();
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::mapStackToScene()
//-----------------------------------------------------------------------------
QPointF MemoryItem::mapStackToScene(QPointF const& pos) const
{
    return mapToScene(pos);
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::mapStackFromScene()
//-----------------------------------------------------------------------------
QPointF MemoryItem::mapStackFromScene(QPointF const& pos) const
{
    return mapFromScene(pos);
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::isItemAllowed()
//-----------------------------------------------------------------------------
bool MemoryItem::isItemAllowed(QGraphicsItem* item) const
{
    return (item->type() == AddressSectionItem::Type);
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getComponentStackHeight()
//-----------------------------------------------------------------------------
qreal MemoryItem::getHeight() const
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
// Function: MemoryItem::getContentType()
//-----------------------------------------------------------------------------
ColumnContentType MemoryItem::getContentType() const
{
    return COLUMN_CONTENT_COMPONENTS;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::updateSize()
//-----------------------------------------------------------------------------
void MemoryItem::updateSize()
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
void MemoryItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
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
void MemoryItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
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
void MemoryItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mousePressEvent(event);
    setZValue(1001.0);

    oldPos_ = scenePos();
    oldColumn_ = dynamic_cast<MemoryColumn*>(parentItem());
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getAddressBlockSections()
//-----------------------------------------------------------------------------
QList<AddressSectionItem*> const& MemoryItem::getSections() const
{
    return sections_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::convertAddress()
//-----------------------------------------------------------------------------
quint64 MemoryItem::convertAddress(quint64 address, MemoryBaseItem* source) const
{
    // Conversion is possible only if the source is an address base.
    AddressSpaceItem* addrSpaceItem = dynamic_cast<AddressSpaceItem*>(source);

    if (addrSpaceItem == 0)
    {
        return address;
    }

    quint64 addressOffset = 0;

    if (!static_cast<MemoryDesignDiagram*>(scene())->isConnected(addrSpaceItem, this, &addressOffset))
    {
        return address;
    }

    return address - addressOffset;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component const> MemoryItem::getComponent() const
{
    return component_;
}

//-----------------------------------------------------------------------------
// Function: MemoryItem::getInstanceName()
//-----------------------------------------------------------------------------
QString const& MemoryItem::getInstanceName() const
{
    return instanceName_;
}
