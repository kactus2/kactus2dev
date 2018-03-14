//-----------------------------------------------------------------------------
// File: CommonGraphicsUndoCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 6.7.2012
//
// Description:
// Common graphics item undo commands.
//-----------------------------------------------------------------------------

#include "CommonGraphicsUndoCommands.h"

#include "IGraphicsItemStack.h"

#include <common/graphicsItems/GraphicsColumnConstants.h>

#include <designEditors/common/DesignDiagram.h>
#include <designEditors/HWDesign/HWComponentItem.h>
#include <designEditors/SystemDesign/HWMappingItem.h>
#include <designEditors/SystemDesign/SWComponentItem.h>

#include <IPXACTmodels/Design/ComponentInstance.h>

//-----------------------------------------------------------------------------
// Function: ItemAddCommand()
//-----------------------------------------------------------------------------
ItemAddCommand::ItemAddCommand(IGraphicsItemStack* stack, QGraphicsItem* item, DesignDiagram* diagram,
    QUndoCommand* parent):
QUndoCommand(parent),
item_(item),
stack_(stack),
del_(false),
diagram_(diagram)
{
}

//-----------------------------------------------------------------------------
// Function: ~ItemAddCommand()
//-----------------------------------------------------------------------------
ItemAddCommand::~ItemAddCommand()
{
    if (del_)
    {
        delete item_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ItemAddCommand::undo()
{
    // Remove the item from the stack and the scene.
    stack_->removeItem(item_);
    item_->scene()->removeItem(item_);
    del_ = true;

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ItemAddCommand::redo()
{
    // Add the item to the stack.
    stack_->addItem(item_);
    del_ = false;

    // Child commands need not be executed because the other items change their position
    // in a deterministic way.
    //QUndoCommand::redo();

    if (item_->scenePos().y() + GraphicsColumnConstants::MIN_Y_PLACEMENT > item_->scene()->height())
    {
        diagram_->resetSceneRectangleForItems();
    }
}

//-----------------------------------------------------------------------------
// Function: ItemMoveCommand()
//-----------------------------------------------------------------------------
ItemMoveCommand::ItemMoveCommand(QGraphicsItem* item, QPointF const& oldPos, IGraphicsItemStack* oldStack,
    DesignDiagram* diagram, QUndoCommand* parent):
QUndoCommand(parent),
item_(item),
oldPos_(oldPos),
oldStack_(oldStack),
newPos_(item->scenePos()),
newStack_(dynamic_cast<IGraphicsItemStack*>(item->parentItem())),
diagram_(diagram)
{
    Q_ASSERT(oldStack != 0);
}

//-----------------------------------------------------------------------------
// Function: ItemMoveCommand::ItemMoveCommand()
//-----------------------------------------------------------------------------
ItemMoveCommand::ItemMoveCommand(QGraphicsItem* item, QPointF const& oldPos, IGraphicsItemStack* oldStack,
    QPointF const& newPos, IGraphicsItemStack* newStack, DesignDiagram* diagram, QUndoCommand* parent):
QUndoCommand(parent),
item_(item),
oldPos_(oldPos),
oldStack_(oldStack),
newPos_(newPos),
newStack_(newStack),
diagram_(diagram)
{
    Q_ASSERT(oldStack != 0);
    Q_ASSERT(newStack != 0);
}

//-----------------------------------------------------------------------------
// Function: ~ItemMoveCommand()
//-----------------------------------------------------------------------------
ItemMoveCommand::~ItemMoveCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ItemMoveCommand::undo()
{
    newStack_->removeItem(item_);

    item_->setPos(oldPos_);
    oldStack_->addItem(item_);

    SystemComponentItem* systemItem = dynamic_cast<SystemComponentItem*>(item_);
    if (systemItem)
    {
        systemItem->getComponentInstance()->setPosition(oldPos_);
        QSharedPointer<ComponentInstance> swInstance = systemItem->getComponentInstance();
        if (swInstance)
        {
            HWMappingItem* hwMapItem = dynamic_cast<HWMappingItem*>(oldStack_);
            if (hwMapItem && hwMapItem->getComponentInstance())
            {
                swInstance->setMapping(hwMapItem->getComponentInstance()->getUuid());
            }
            else
            {
                swInstance->setMapping("");
            }
        }
    }

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ItemMoveCommand::redo()
{
    oldStack_->removeItem(item_);

    item_->setPos(newPos_);
    newStack_->addItem(item_);

    SystemComponentItem* systemItem = dynamic_cast<SystemComponentItem*>(item_);
    if (systemItem)
    {
        systemItem->getComponentInstance()->setPosition(newPos_);
        QSharedPointer<ComponentInstance> swInstance = systemItem->getComponentInstance();
        if (swInstance)
        {
            HWMappingItem* hwMapItem = dynamic_cast<HWMappingItem*>(newStack_);
            if (hwMapItem && hwMapItem->getComponentInstance())
            {
                swInstance->setMapping(hwMapItem->getComponentInstance()->getUuid());
            }
            else
            {
                swInstance->setMapping("");
            }
        }
    }

    // Execute child commands.
    QUndoCommand::redo();

    if (item_->scenePos().y() + GraphicsColumnConstants::MIN_Y_PLACEMENT > item_->scene()->height())
    {
        diagram_->resetSceneRectangleForItems();
    }
}
