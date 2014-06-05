//-----------------------------------------------------------------------------
// File: StickyNoteMoveCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.05.2014
//
// Description:
// Move command for floating items.
//-----------------------------------------------------------------------------

#include "StickyNoteMoveCommand.h"

//-----------------------------------------------------------------------------
// Function: StickyNoteMoveCommand()
//-----------------------------------------------------------------------------
StickyNoteMoveCommand::StickyNoteMoveCommand(QGraphicsItem* item, QPointF const& oldPos, QUndoCommand* parent)
    : QUndoCommand(parent),
    item_(item),
    oldPos_(oldPos),
    newPos_(item->scenePos())
{

}

//-----------------------------------------------------------------------------
// Function: ~StickyNoteMoveCommand()
//-----------------------------------------------------------------------------
StickyNoteMoveCommand::~StickyNoteMoveCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void StickyNoteMoveCommand::undo()
{
    item_->setPos(oldPos_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void StickyNoteMoveCommand::redo()
{
    item_->setPos(newPos_);

    // Execute child commands.
    QUndoCommand::redo();
}