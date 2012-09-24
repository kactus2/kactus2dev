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

#include <LibraryManager/vlnv.h>

#include <common/utils.h>
#include <common/DesignDiagram.h>
#include <common/layouts/VStackedLayout.h>

#include <models/component.h>
#include <models/addressspace.h>
#include <models/segment.h>

#include <QFont>
#include <QTextDocument>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: AddressSpaceItem()
//-----------------------------------------------------------------------------
AddressSpaceItem::AddressSpaceItem(LibraryInterface* libInterface, QSharedPointer<Component> component,
                                   QSharedPointer<AddressSpace> addressSpace, QGraphicsItem *parent)
    : QGraphicsRectItem(parent),
      libInterface_(libInterface),
      component_(component),
      addressSpace_(addressSpace),
      nameLabel_(0),
      aubLabel_(),
      sections_()
{
    Q_ASSERT_X(component, "AddressSpaceItem constructor",
               "Null component-pointer given as parameter");

    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsSelectable);
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
    aubLabel_->setHtml(QString("<center>AUB=") + QString::number(addressSpace->getAddressUnitBits()) +
                       QString("</center>"));

    // Parse segments and add a section for each of them.
    foreach (QSharedPointer<Segment> segment, addressSpace->getSegments())
    {
        if (segment != 0)
        {
            unsigned int startAddress = Utils::str2Int(segment->getAddressOffset());
            unsigned int range = Utils::str2Int(segment->getRange());

            AddressSectionItem* section = new AddressSectionItem(segment->getName(), startAddress, range, this);
            section->setPos(0.0, getHeight());
            addItem(section);
        }
    }

    updateNameLabel(component->getVlnv()->getName() + "<br>" + addressSpace->getName());
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

    setBrush(QBrush(QColor(0xc5,0xff, 0xab)));
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
void AddressSpaceItem::addItem(QGraphicsItem* item, bool load)
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
        VStackedLayout::updateItemMove(sections_, section, 0.0, SPACING);
        VStackedLayout::setItemPos(sections_, section, SECTION_X, 0.0, SPACING);
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

    VStackedLayout::updateItemMove(sections_, section, 0.0, SPACING);
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
    VStackedLayout::setItemPos(sections_, static_cast<AddressSectionItem*>(item), SECTION_X, 0.0, SPACING);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceItem::updateItemPositions()
//-----------------------------------------------------------------------------
void AddressSpaceItem::updateItemPositions()
{
    // Just update the item positions.
    VStackedLayout::updateItemPositions(sections_, SECTION_X, 0.0, SPACING);
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
    nameLabel_->setPos(-WIDTH / 2 + GridSize / 2, getHeight());
}
