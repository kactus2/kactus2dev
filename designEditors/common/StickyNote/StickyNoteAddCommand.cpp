//-----------------------------------------------------------------------------
// File: FloatingItemAddCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.05.2014
//
// Description:
// Add command for sticky note items.
//-----------------------------------------------------------------------------

#include "StickyNoteAddCommand.h"

#include "StickyNote.h"

#include <IPXACTmodels/VendorExtension.h>

//-----------------------------------------------------------------------------
// Function: StickyNoteAddCommand()
//-----------------------------------------------------------------------------
StickyNoteAddCommand::StickyNoteAddCommand(StickyNote* note, QGraphicsScene* scene, QPointF position,
    QUndoCommand* parent) : QUndoCommand(parent), note_(note),
    scene_(scene), position_(position), del_(false)
{

}

//-----------------------------------------------------------------------------
// Function: ~StickyNoteAddCommand()
//-----------------------------------------------------------------------------
StickyNoteAddCommand::~StickyNoteAddCommand()
{
    if (del_)
    {
        delete note_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void StickyNoteAddCommand::undo()
{
    // Remove the item from the stack and the scene.
    scene_->removeItem(note_);
    del_ = true;

    emit removeVendorExtension(note_->getVendorExtension());

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void StickyNoteAddCommand::redo()
{
    // Add the item to the stack.
    scene_->addItem(note_);
    note_->setPos(position_);    

    del_ = false;

    emit addVendorExtension(note_->getVendorExtension());

    QUndoCommand::redo();
}
