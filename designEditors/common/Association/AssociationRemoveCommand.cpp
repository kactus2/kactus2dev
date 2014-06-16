//-----------------------------------------------------------------------------
// File: FloatingItemRemoveCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 16.06.2014
//
// Description:
// Remove command for association items.
//-----------------------------------------------------------------------------

#include "AssociationRemoveCommand.h"

#include <designEditors/common/Association.h>

#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: AssociationRemoveCommand()
//-----------------------------------------------------------------------------
AssociationRemoveCommand::AssociationRemoveCommand(Association* association, QGraphicsScene* diagram, 
    QUndoCommand* parent) : 
    QUndoCommand(parent), association_(association),
    scene_(diagram), 
    del_(true)
{

}

//-----------------------------------------------------------------------------
// Function: ~AssociationRemoveCommand()
//-----------------------------------------------------------------------------
AssociationRemoveCommand::~AssociationRemoveCommand()
{
    if (del_)
    {
        delete association_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void AssociationRemoveCommand::undo()
{
    scene_->addItem(association_);
    del_ = false;

    association_->connect();

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void AssociationRemoveCommand::redo()
{
    scene_->removeItem(association_);
    del_ = true;

    association_->disconnect();

    QUndoCommand::redo();
}
