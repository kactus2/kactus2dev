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

#include "SystemMoveCommands.h"

#include "SystemDesignDiagram.h"
#include "SWComponentItem.h"
#include "SWPortItem.h"

#include <designEditors/HWDesign/HWMoveCommands.h>

#include <library/LibraryManager/libraryinterface.h>

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/CommonGraphicsUndoCommands.h>
#include <common/GenericEditProvider.h>
#include <common/layouts/VStackedLayout.h>

#include <IPXACTmodels/component.h>

#include <QBrush>
#include <QUndoCommand>

//-----------------------------------------------------------------------------
// Function: HWMappingItem::HWMappingItem()
//-----------------------------------------------------------------------------
HWMappingItem::HWMappingItem(LibraryInterface* libInterface,
                             QSharedPointer<Component> component,
                             QString const& instanceName,
                             QString const& displayName,
                             QString const& description,
									  QString const& uuid,
                             QMap<QString, QString> const& configurableElementValues)
    : SystemComponentItem(QRectF(-WIDTH / 2, 0, WIDTH, 0), libInterface, component, instanceName,
                      displayName, description, uuid, configurableElementValues, 0),
      oldStack_(0),
      layout_(new VStackedLayout<ComponentItem>(SPACING)),
      swComponents_(),
      oldPos_()
{
    setFlag(ItemIsMovable);
    
    updateComponent();
    updateItemPositions();
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::~HWMappingItem()
//-----------------------------------------------------------------------------
HWMappingItem::~HWMappingItem()
{
    // Remove this item from the column where it resides.
    IGraphicsItemStack* column = dynamic_cast<IGraphicsItemStack*>(parentItem());

    if (column != 0)
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
    foreach (QGraphicsItem *item, scene()->items())
    {
        GraphicsConnection* conn = dynamic_cast<GraphicsConnection*>(item);

        if (conn != 0)
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

    if (oldStack_ != 0)
    {
        setPos(parentItem()->mapFromScene(oldStack_->mapStackToScene(pos())));

        IGraphicsItemStack* column = dynamic_cast<IGraphicsItemStack*>(parentItem());
        Q_ASSERT(column != 0);
        column->onMoveItem(this);
    }

    setConnectionUpdateDisabled(false);

    // Update the port connections manually.
    foreach (QGraphicsItem *item, childItems())
    {
        if (item->type() != SWPortItem::Type)
        {
            continue;
        }

        SWPortItem* port = static_cast<SWPortItem*>(item);

        foreach (GraphicsConnection* conn, port->getConnections())
        {
            conn->updatePosition();
        }

        foreach (GraphicsConnection* conn, port->getOffPageConnector()->getConnections())
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

    if (oldStack_ != 0)
    {
        IGraphicsItemStack* column = dynamic_cast<IGraphicsItemStack*>(parentItem());
        Q_ASSERT(column != 0);
        column->onReleaseItem(this);

        QSharedPointer<QUndoCommand> cmd;

        if (scenePos() != oldPos_)
        {
            cmd = QSharedPointer<QUndoCommand>(new ItemMoveCommand(this, oldPos_, oldStack_));
        }
        else
        {
            cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
        }

        // End the position update for all connections.
        foreach (QGraphicsItem *item, scene()->items())
        {
            GraphicsConnection* conn = dynamic_cast<GraphicsConnection*>(item);

            if (conn != 0)
            {
                conn->endUpdatePosition(cmd.data());
            }
        }
        
        // Add the undo command to the edit stack only if it has at least some real changes.
        if (cmd->childCount() > 0 || scenePos() != oldPos_)
        {
            static_cast<SystemDesignDiagram*>(scene())->getEditProvider().addCommand(cmd);
        }

        oldStack_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::getHeight()
//-----------------------------------------------------------------------------
qreal HWMappingItem::getHeight() const
{
    // Calculate the minimum height based on the stack contents.
    qreal stackHeight = getComponentStackHeight();

    // Determine the largest one from the stack height, minimum height (empty) and the height
    // calculated by the base class.
    return qMax<qreal>(qMax<qreal>(stackHeight, MIN_HEIGHT), SystemComponentItem::getHeight());
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::addItem()
//-----------------------------------------------------------------------------
void HWMappingItem::addItem(QGraphicsItem* item, bool load)
{
    // Map the position to the column's local coordinate system
    // and constrain the item to the horizontal center of the column.
    QPointF pos = mapFromScene(item->scenePos());
    pos.setX(0.0);

    item->setParentItem(this);
    item->setPos(pos);

    item->setFlag(ItemStacksBehindParent, false);

    ComponentItem* compItem = static_cast<ComponentItem*>(item);

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
    item->setParentItem(0);
    updateItemPositions();
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::onMoveItem()
//-----------------------------------------------------------------------------
void HWMappingItem::onMoveItem(QGraphicsItem* item)
{
    ComponentItem* compItem = static_cast<ComponentItem*>(item);

    layout_->updateItemMove(swComponents_, compItem, TOP_MARGIN);
    offsetPortPositions(getComponentStackHeight() + SPACING);
    updateSize();

    // Check if the item is not overlapping the HW mapping item enough.
    QRectF intersection = sceneBoundingRect().intersected(item->sceneBoundingRect());

    // Only non-imported SW components can be moved out of the HW mapping item.
    if (!static_cast<SystemComponentItem*>(compItem)->isImported() &&
        compItem->rect().height() - intersection.height() >= 3 * GridSize)
    {
        swComponents_.removeAll(compItem);

        // Let the parent component stack handle the mouse move.
        IGraphicsItemStack* parentStack = dynamic_cast<IGraphicsItemStack*>(parentItem());
        Q_ASSERT(parentStack != 0);
        
        QPointF newPos = parentStack->mapStackFromScene(compItem->scenePos());
        compItem->setParentItem(parentItem());
        compItem->setPos(newPos);
        compItem->setFlag(ItemStacksBehindParent);

        updateItemPositions();
        setZValue(0.0);

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
    return (componentModel()->isCpu() && item->type() == SWComponentItem::Type);
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::getComponentStackHeight()
//-----------------------------------------------------------------------------
qreal HWMappingItem::getComponentStackHeight() const
{
    qreal stackHeight = TOP_MARGIN + BOTTOM_MARGIN;

    if (!swComponents_.isEmpty())
    {
        foreach (ComponentItem* item, swComponents_)
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

    VLNV* vlnv = componentModel()->getVlnv();

    // Check whether the component is packaged (valid vlnv) or not.
    if (vlnv->isValid())
    {
        if (!getLibraryInterface()->contains(*vlnv))
        {
            setBrush(QBrush(QColor(0xe8, 0xc5, 0xc5)));
        }
        else
        {
            setBrush(QBrush(QColor(0xa5,0xc3,0xef)));
        }
    }
    else
    {
        setBrush(QBrush(QColor(217, 217, 217)));
    }
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::getContentType()
//-----------------------------------------------------------------------------
ColumnContentType HWMappingItem::getContentType() const
{
    return COLUMN_CONTENT_COMPONENTS;
}

//-----------------------------------------------------------------------------
// Function: HWMappingItem::getLinkedHW()
//-----------------------------------------------------------------------------
HWMappingItem const* HWMappingItem::getLinkedHW() const
{
    return this;
}
