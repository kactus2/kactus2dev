//-----------------------------------------------------------------------------
// File: AssociationAddCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 16.06.2014
//
// Description:
// Add command for association items.
//-----------------------------------------------------------------------------

#include "AssociationAddCommand.h"


#include <designEditors/common/DesignDiagram.h>
#include <designEditors/common/Association.h>

//-----------------------------------------------------------------------------
// Function: AssociationAddCommand()
//-----------------------------------------------------------------------------
AssociationAddCommand::AssociationAddCommand(Association* association, QGraphicsScene* scene, 
    QUndoCommand* parent) : 
    QUndoCommand(parent), association_(association),
    scene_(scene), 
    del_(false)
{

}

//-----------------------------------------------------------------------------
// Function: ~AssociationAddCommand()
//-----------------------------------------------------------------------------
AssociationAddCommand::~AssociationAddCommand()
{
    if (del_)
    {
        delete association_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void AssociationAddCommand::undo()
{
    scene_->removeItem(association_);
    del_ = true;

    association_->disconnect();

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void AssociationAddCommand::redo()
{
    scene_->addItem(association_);
    del_ = false;

    association_->connect();

    QUndoCommand::redo();
}
