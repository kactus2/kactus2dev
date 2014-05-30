//-----------------------------------------------------------------------------
// File: FloatingItemAddCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.05.2014
//
// Description:
// Add command for floating items.
//-----------------------------------------------------------------------------

#include "FloatingItemAddCommand.h"

//-----------------------------------------------------------------------------
// Function: FloatingItemAddCommand()
//-----------------------------------------------------------------------------
FloatingItemAddCommand::FloatingItemAddCommand(QGraphicsScene* scene, QGraphicsItem* item, QPointF position,
    QUndoCommand* parent) : QUndoCommand(parent), item_(item),
    scene_(scene), position_(position), del_(false)
{
}

//-----------------------------------------------------------------------------
// Function: ~FloatingItemAddCommand()
//-----------------------------------------------------------------------------
FloatingItemAddCommand::~FloatingItemAddCommand()
{
    if (del_)
    {
        delete item_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void FloatingItemAddCommand::undo()
{
    // Remove the item from the stack and the scene.
    scene_->removeItem(item_);
    del_ = true;

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void FloatingItemAddCommand::redo()
{
    // Add the item to the stack.
    scene_->addItem(item_);
    item_->setPos(position_);

    del_ = false;

    QUndoCommand::redo();
}
