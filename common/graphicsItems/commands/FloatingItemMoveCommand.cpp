//-----------------------------------------------------------------------------
// File: FloatingItemMoveCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.05.2014
//
// Description:
// Move command for floating items.
//-----------------------------------------------------------------------------

#include "FloatingItemMoveCommand.h"

//-----------------------------------------------------------------------------
// Function: FloatingItemMoveCommand()
//-----------------------------------------------------------------------------
FloatingItemMoveCommand::FloatingItemMoveCommand(QGraphicsItem* item, QPointF const& oldPos, QUndoCommand* parent)
    : QUndoCommand(parent),
    item_(item),
    oldPos_(oldPos),
    newPos_(item->scenePos())
{

}

//-----------------------------------------------------------------------------
// Function: ~FloatingItemMoveCommand()
//-----------------------------------------------------------------------------
FloatingItemMoveCommand::~FloatingItemMoveCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void FloatingItemMoveCommand::undo()
{
    item_->setPos(oldPos_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void FloatingItemMoveCommand::redo()
{
    item_->setPos(newPos_);

    // Execute child commands.
    QUndoCommand::redo();
}