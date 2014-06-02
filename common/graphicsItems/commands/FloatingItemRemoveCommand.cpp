//-----------------------------------------------------------------------------
// File: FloatingItemRemoveCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.05.2014
//
// Description:
// Remove command for floating items.
//-----------------------------------------------------------------------------

#include "FloatingItemRemoveCommand.h"

//-----------------------------------------------------------------------------
// Function: FloatingItemRemoveCommand()
//-----------------------------------------------------------------------------
FloatingItemRemoveCommand::FloatingItemRemoveCommand(QGraphicsItem* item, QGraphicsScene* scene, 
    QUndoCommand* parent) : QUndoCommand(parent), item_(item),
    scene_(scene), del_(true)
{

}

//-----------------------------------------------------------------------------
// Function: ~FloatingItemRemoveCommand()
//-----------------------------------------------------------------------------
FloatingItemRemoveCommand::~FloatingItemRemoveCommand()
{
    if (del_)
    {
        delete item_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void FloatingItemRemoveCommand::undo()
{
    scene_->addItem(item_);
    del_ = false;

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void FloatingItemRemoveCommand::redo()
{
    scene_->removeItem(item_);
    del_ = true;

    QUndoCommand::redo();
}
