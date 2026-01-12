//-----------------------------------------------------------------------------
// File: HWMappingItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 30.4.2012
//
// Description:
// Graphics item for visualizing the underlying HW in system designs.
//-----------------------------------------------------------------------------

#include "HWMappingItem.h"

#include "SystemDesignDiagram.h"
#include "SWComponentItem.h"
#include "SWPortItem.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/CommonGraphicsUndoCommands.h>
#include <common/GenericEditProvider.h>
#include <common/KactusColors.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/ComponentInstance.h>

#include <QBrush>
#include <QUndoCommand>

//-----------------------------------------------------------------------------
// Function: HWMappingItem::HWMappingItem()
//-----------------------------------------------------------------------------
HWMappingItem::HWMappingItem(LibraryInterface* libInterface, QSharedPointer<Component> component,
                             QSharedPointer<ComponentInstance> instance):
SystemComponentItem(QRectF(-WIDTH/ 2, 0, WIDTH, 0), libInterface, instance, component, nullptr)
{
    setFlag(ItemIsMovable);
    
    HWMappingItem::updateComponent();
    HWMappingItem::updateItemPositions();
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::~HWMappingItem()
//-----------------------------------------------------------------------------
HWMappingItem::~HWMappingItem()
{
    // Remove this item from the column where it resides.
    auto column = dynamic_cast<IGraphicsItemStack*>(parentItem());

    if (column != nullptr)
    {
        column->removeItem(this);
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void HWMappingItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    ComponentItem::mousePressEvent(event);
    setZValue(1001.0);

    oldPos_ = scenePos();
    oldStack_ = dynamic_cast<IGraphicsItemStack*>(parentItem());

    // End the position update for all connections.
    for (QGraphicsItem *item: scene()->items())
    {
        auto conn = dynamic_cast<GraphicsConnection*>(item);

        if (conn != nullptr)
        {
            conn->beginUpdatePosition();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void HWMappingItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Discard mouse move if the diagram is protected.
    if (static_cast<DesignDiagram*>(scene())->isProtected())
    {
        return;
    }

    setConnectionUpdateDisabled(true);

    ComponentItem::mouseMoveEvent(event);

    if (oldStack_ != nullptr)
    {
        setPos(parentItem()->mapFromScene(oldStack_->mapStackToScene(pos())));

        auto column = dynamic_cast<IGraphicsItemStack*>(parentItem());
        Q_ASSERT(column != nullptr);
        column->onMoveItem(this);
    }

    setConnectionUpdateDisabled(false);

    // Update the port connections manually.
    for (QGraphicsItem *item : childItems())
    {
        if (item->type() != SWPortItem::Type)
        {
            continue;
        }

        auto port = static_cast<SWPortItem*>(item);

        for (GraphicsConnection* conn : port->getConnections())
        {
            conn->updatePosition();
        }

        for (GraphicsConnection* conn : port->getOffPageConnector()->getConnections())
        {
            conn->updatePosition();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void HWMappingItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    ComponentItem::mouseReleaseEvent(event);
    setZValue(0.0);

    auto diagram = dynamic_cast<DesignDiagram*>(scene());
    if (diagram && oldStack_ != nullptr)
    {
        auto column = dynamic_cast<IGraphicsItemStack*>(parentItem());
        Q_ASSERT(column != nullptr);
        column->onReleaseItem(this);

        QSharedPointer<QUndoCommand> cmd;

        if (scenePos() != oldPos_)
        {
            cmd = QSharedPointer<QUndoCommand>(new ItemMoveCommand(this, oldPos_, oldStack_, diagram));
        }
        else
        {
            cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
        }

        // End the position update for all connections.
        for (QGraphicsItem *item : scene()->items())
        {
            auto conn = dynamic_cast<GraphicsConnection*>(item);

            if (conn != nullptr)
            {
                conn->endUpdatePosition(cmd.data());
            }
        }
        
        // Add the undo command to the edit stack only if it has at least some real changes.
        if (cmd->childCount() > 0 || scenePos() != oldPos_)
        {
            diagram->getEditProvider()->addCommand(cmd);
            cmd->redo();
        }

        oldStack_ = nullptr;
    }
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::getHeight()
//-----------------------------------------------------------------------------
qreal HWMappingItem::getHeight()
{
    // Calculate the minimum height based on the stack contents.
    qreal stackHeight = getComponentStackHeight();

    // Determine the largest one from the stack height, minimum height (empty) and the height
    // calculated by the base class.
    return qMax<qreal>(qMax<qreal>(stackHeight, MIN_HEIGHT), SystemComponentItem::getHeight());
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::getWidth()
//-----------------------------------------------------------------------------
qreal HWMappingItem::getWidth() noexcept
{	
	return WIDTH;
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::addItem()
//-----------------------------------------------------------------------------
void HWMappingItem::addItem(QGraphicsItem* item, bool load)
{
	if (item->parentItem() != 0)
	{
		dynamic_cast<IGraphicsItemStack*>(item->parentItem())->removeItem(item);
	}

    // Map the position to the column's local coordinate system
    // and constrain the item to the horizontal center of the column.
    QPointF pos = mapFromScene(item->scenePos());
    pos.setX(0.0);

    item->setParentItem(this);
    item->setPos(pos);

    item->setFlag(ItemStacksBehindParent, false);

    auto compItem = static_cast<ComponentItem*>(item);

    if (load)
    {
        // Place the item at the correct index in the items list.
        for (int i = 0; i < swComponents_.size(); ++i)
        {
            if (item->y() < swComponents_.at(i)->y())
            {
                swComponents_.insert(i, compItem);

                offsetPortPositions(getComponentStackHeight() + SPACING);
                updateSize();
                return;
            }
        }

        // If we got here, the item must be placed at the end of the list.
        swComponents_.append(compItem);

        offsetPortPositions(getComponentStackHeight() + SPACING);
        updateSize();
    }
    else
    {
        swComponents_.append(compItem);
        
        layout_->updateItemMove(swComponents_, compItem, TOP_MARGIN);
        layout_->setItemPos(swComponents_, compItem, 0.0, TOP_MARGIN);
        updateItemPositions();
    }
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::removeItem()
//-----------------------------------------------------------------------------
void HWMappingItem::removeItem(QGraphicsItem* item)
{
    swComponents_.removeAll(static_cast<ComponentItem*>(item));
    item->setParentItem(nullptr);
    updateItemPositions();
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::onMoveItem()
//-----------------------------------------------------------------------------
void HWMappingItem::onMoveItem(QGraphicsItem* item)
{
    auto compItem = static_cast<ComponentItem*>(item);

    layout_->updateItemMove(swComponents_, compItem, TOP_MARGIN);
    offsetPortPositions(getComponentStackHeight() + SPACING);
    updateSize();

    // Check if the item is not overlapping the HW mapping item enough.
    QRectF intersection = sceneBoundingRect().intersected(item->sceneBoundingRect());

    QSharedPointer<ComponentInstance> swInstance = compItem->getComponentInstance();
    if (swInstance)
    {
        swInstance->setMapping(getComponentInstance()->getUuid());
    }

    auto swItem = static_cast<SWComponentItem*>(compItem);
    // Only non-imported SW components can be moved out of the HW mapping item.
    if (!swItem->isImported() && swItem->rect().height() - intersection.height() >= swItem->rect().height())
    {
        swComponents_.removeAll(compItem);

        // Let the parent component stack handle the mouse move.
        auto parentStack = dynamic_cast<IGraphicsItemStack*>(parentItem());
        Q_ASSERT(parentStack != nullptr);
        
        QPointF newPos = parentStack->mapStackFromScene(compItem->scenePos());
        compItem->setParentItem(parentItem());
        compItem->setPos(newPos);
        compItem->setFlag(ItemStacksBehindParent);

        updateItemPositions();
        setZValue(0.0);

        swInstance->setMapping("");

        parentStack->onMoveItem(compItem);
        return;
    }

    setZValue(1001.0);
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::onReleaseItem()
//-----------------------------------------------------------------------------
void HWMappingItem::onReleaseItem(QGraphicsItem* item)
{
    setZValue(0.0);
    layout_->setItemPos(swComponents_, static_cast<ComponentItem*>(item), 0.0, TOP_MARGIN);
    offsetPortPositions(getComponentStackHeight() + SPACING);
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::updateItemPositions()
//-----------------------------------------------------------------------------
void HWMappingItem::updateItemPositions()
{
    // Just update the item positions.
    layout_->updateItemPositions(swComponents_, 0.0, TOP_MARGIN);
    offsetPortPositions(getComponentStackHeight() + SPACING);
    updateSize();
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
bool HWMappingItem::isItemAllowed(QGraphicsItem* item) const
{
    return (item->type() == SWComponentItem::Type && componentModel()->isCpu());
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::getComponentStackHeight()
//-----------------------------------------------------------------------------
qreal HWMappingItem::getComponentStackHeight() const
{
    qreal stackHeight = TOP_MARGIN + BOTTOM_MARGIN;

    if (!swComponents_.isEmpty())
    {
        for (ComponentItem* item : swComponents_)
        {
            stackHeight += item->boundingRect().height();
        }

        stackHeight += (swComponents_.size() - 1) * SPACING;
    }

    return stackHeight;
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::updateComponent()
//-----------------------------------------------------------------------------
void HWMappingItem::updateComponent()
{
    ComponentItem::updateComponent();

    VLNV vlnv = componentModel()->getVlnv();

    // Check whether the component is packaged (valid vlnv) or not.
    if (vlnv.isValid())
    {
        if (!getLibraryInterface()->contains(vlnv))
        {
            setBrush(QBrush(KactusColors::MISSING_COMPONENT));
        }
        else
        {
            setBrush(QBrush(KactusColors::HW_COMPONENT));
        }
    }
    else
    {
        setBrush(QBrush(KactusColors::DRAFT_COMPONENT));
    }
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::getContentType()
//-----------------------------------------------------------------------------
ColumnTypes::ColumnContentType HWMappingItem::getContentType() const
{
    return ColumnTypes::COMPONENTS;
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::getLinkedHW()
//-----------------------------------------------------------------------------
HWMappingItem const* HWMappingItem::getLinkedHW() const
{
    return this;
}
