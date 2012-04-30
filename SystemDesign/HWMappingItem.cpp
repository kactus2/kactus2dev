//-----------------------------------------------------------------------------
// File: HWMappingItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 30.4.2012
//
// Description:
// Graphics item for visualizing the underlying HW in system designs.
//-----------------------------------------------------------------------------

#include "HWMappingItem.h"

#include "SystemDesignDiagram.h"
#include "SWCompItem.h"

#include <common/GenericEditProvider.h>
#include <common/layouts/VStackedLayout.h>

#include <models/component.h>

#include <QBrush>
#include <QUndoCommand>

//-----------------------------------------------------------------------------
// Function: HWMappingItem::HWMappingItem()
//-----------------------------------------------------------------------------
HWMappingItem::HWMappingItem(QSharedPointer<Component> component,
                             QString const& instanceName,
                             QString const& displayName,
                             QString const& description,
                             QMap<QString, QString> const& configurableElementValues,
                             unsigned int id)
    : ComponentItem(QRectF(-WIDTH / 2, 0, WIDTH, 0), component, instanceName,
                    displayName, description, configurableElementValues, 0),
      id_(id),
      oldStack_(0),
      swComponents_(),
      oldPos_()
{
    setFlag(ItemIsMovable);
    setBrush(QBrush(QColor(0xa5,0xc3,0xef)));

    updateComponent();
    updateSize();

    mapComponent(new SWCompItem(component, "testi"));
    mapComponent(new SWCompItem(component, "testi2"));
    mapComponent(new SWCompItem(component, "testi3"));
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::~HWMappingItem()
//-----------------------------------------------------------------------------
HWMappingItem::~HWMappingItem()
{
    // Remove this item from the column where it resides.
    IComponentStack* column = dynamic_cast<IComponentStack*>(parentItem());

    if (column != 0)
    {
        column->removeItem(this);
    }
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::mapComponent()
//-----------------------------------------------------------------------------
void HWMappingItem::mapComponent(SWCompItem* item)
{
    addItem(item, false);
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::unmapComponent()
//-----------------------------------------------------------------------------
void HWMappingItem::unmapComponent(SWCompItem* item)
{
    removeItem(item);
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void HWMappingItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    ComponentItem::mousePressEvent(event);
    setZValue(1001.0);

    oldPos_ = scenePos();
    oldStack_ = dynamic_cast<IComponentStack*>(parentItem());

    // TODO: Begin the position update for the connections.
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void HWMappingItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Discard mouse move if the diagram is protected.
    if (static_cast<SystemDesignDiagram*>(scene())->isProtected())
    {
        return;
    }

    ComponentItem::mouseMoveEvent(event);

    if (oldStack_ != 0)
    {
        setPos(parentItem()->mapFromScene(oldStack_->mapStackToScene(pos())));

        IComponentStack* column = dynamic_cast<IComponentStack*>(parentItem());
        Q_ASSERT(column != 0);
        column->onMoveItem(this);
    }
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void HWMappingItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    ComponentItem::mouseReleaseEvent(event);
    setZValue(0.0);

    if (oldStack_ != 0)
    {
        IComponentStack* column = dynamic_cast<IComponentStack*>(parentItem());
        Q_ASSERT(column != 0);
        column->onReleaseItem(this);

        oldStack_ = 0;

        QSharedPointer<QUndoCommand> cmd;

//         if (scenePos() != oldPos_)
//         {
//             cmd = QSharedPointer<QUndoCommand>(new MappingCompMoveCommand(this, oldPos_));
//         }
//         else
//         {
//             cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
//         }
// 
//         // TODO: End the position update for the connections and clear the list.
//         
//         // Add the undo command to the edit stack only if it has at least some real changes.
//         if (cmd->childCount() > 0 || scenePos() != oldPos_)
//         {
//             static_cast<SystemDesignDiagram*>(scene())->getEditProvider().addCommand(cmd, false);
//         }
    }
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::updateSize()
//-----------------------------------------------------------------------------
void HWMappingItem::updateSize()
{
    // Update the SW component item positions.
    //VStackedLayout::updateItemPositions(swComponents_, 0.0, TOP_MARGIN, SPACING);

    // Update the component's size based on the item that is positioned at
    // the lowest level of them all.
    qreal bottom = TOP_MARGIN;

//     if (!swComponents_.empty())
//     {
//         bottom = swComponents_.back()->y() + swComponents_.back()->boundingRect().bottom();
//     }

    foreach (ComponentItem* item, swComponents_)
    {
        bottom += item->boundingRect().height();
    }

    bottom += (swComponents_.size() - 1) * SPACING;

    setRect(-WIDTH / 2, 0, WIDTH, bottom + BOTTOM_MARGIN);



    IComponentStack* column = dynamic_cast<IComponentStack*>(parentItem());

    if (column != 0)
    {
        column->updateItemPositions();
    }
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::addItem()
//-----------------------------------------------------------------------------
void HWMappingItem::addItem(ComponentItem* item, bool load)
{
    item->setParentItem(this);

    swComponents_.append(item);
    VStackedLayout::updateItemMove(swComponents_, item, TOP_MARGIN, SPACING);
    VStackedLayout::setItemPos(swComponents_, item, 0.0, TOP_MARGIN, SPACING);
    updateSize();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::removeItem()
//-----------------------------------------------------------------------------
void HWMappingItem::removeItem(ComponentItem* item)
{
    swComponents_.removeAll(item);
    item->setParentItem(0);
    updateSize();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::onMoveItem()
//-----------------------------------------------------------------------------
void HWMappingItem::onMoveItem(ComponentItem* item)
{
    // Check if the item is not inside the HW mapping item.
    if (!contains(item->pos() + item->boundingRect().center())) // TODO:
    {
        // Let the parent component stack handle the mouse move.
        SystemColumn* parentStack = static_cast<SystemColumn*>(parentItem());
        Q_ASSERT(parentStack != 0);

        swComponents_.removeAll(item);

        QPointF oldPos = item->scenePos();

        QPointF newPos = parentStack->mapStackFromScene(item->scenePos());
        item->setParentItem(parentStack);
        item->setPos(newPos);

        updateItemPositions();
        updateSize();
        setZValue(0.0);

        parentStack->onMoveItem(item);
        return;
    }

    setZValue(1001.0);
    VStackedLayout::updateItemMove(swComponents_, item, TOP_MARGIN, SPACING);
    updateSize();
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::onReleaseItem()
//-----------------------------------------------------------------------------
void HWMappingItem::onReleaseItem(ComponentItem* item)
{
    setZValue(0.0);
    VStackedLayout::setItemPos(swComponents_, item, 0.0, TOP_MARGIN, SPACING);
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::updateItemPositions()
//-----------------------------------------------------------------------------
void HWMappingItem::updateItemPositions()
{
    // Just update the item positions.
    VStackedLayout::updateItemPositions(swComponents_, 0.0, TOP_MARGIN, SPACING);
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::mapStackToScene()
//-----------------------------------------------------------------------------
QPointF HWMappingItem::mapStackToScene(QPointF const& pos) const
{
    return mapToScene(pos);
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::mapStackFromScene()
//-----------------------------------------------------------------------------
QPointF HWMappingItem::mapStackFromScene(QPointF const& pos) const
{
    return mapFromScene(pos);
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::isItemAllowed()
//-----------------------------------------------------------------------------
bool HWMappingItem::isItemAllowed(ComponentItem* item) const
{
    return (item->type() == SWCompItem::Type);
}
