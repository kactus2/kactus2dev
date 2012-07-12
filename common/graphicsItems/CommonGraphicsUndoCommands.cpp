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

#include <designwidget/HWComponentItem.h>

//-----------------------------------------------------------------------------
// Function: ItemAddCommand()
//-----------------------------------------------------------------------------
ItemAddCommand::ItemAddCommand(IGraphicsItemStack* stack, QGraphicsItem* item,
                               QUndoCommand* parent) : QUndoCommand(parent), item_(item),
                               stack_(stack), del_(false)
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

    if (dynamic_cast<ComponentItem*>(item_) != 0)
    {
        emit componentInstanceRemoved(static_cast<ComponentItem*>(item_));
    }

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

    if (dynamic_cast<ComponentItem*>(item_) != 0)
    {
        emit componentInstantiated(static_cast<ComponentItem*>(item_));
    }

    // Child commands need not be executed because the other items change their position
    // in a deterministic way.
    //QUndoCommand::redo();
}

//-----------------------------------------------------------------------------
// Function: ItemMoveCommand()
//-----------------------------------------------------------------------------
ItemMoveCommand::ItemMoveCommand(QGraphicsItem* item, QPointF const& oldPos,
                                 IGraphicsItemStack* oldStack, QUndoCommand* parent)
                                 : QUndoCommand(parent),
                                 item_(item),
                                 oldPos_(oldPos),
                                 oldStack_(oldStack),
                                 newPos_(item->scenePos()),
                                 newStack_(dynamic_cast<IGraphicsItemStack*>(item->parentItem()))
{
    Q_ASSERT(oldStack != 0);
}

//-----------------------------------------------------------------------------
// Function: ItemMoveCommand::ItemMoveCommand()
//-----------------------------------------------------------------------------
ItemMoveCommand::ItemMoveCommand(QGraphicsItem* item, QPointF const& oldPos, IGraphicsItemStack* oldStack,
                                 QPointF const& newPos, IGraphicsItemStack* newStack,
                                 QUndoCommand* parent /*= 0*/)
                                 : QUndoCommand(parent),
                                 item_(item),
                                 oldPos_(oldPos),
                                 oldStack_(oldStack),
                                 newPos_(newPos),
                                 newStack_(newStack)
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

    // Execute child commands.
    QUndoCommand::redo();
}