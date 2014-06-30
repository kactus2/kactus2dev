//-----------------------------------------------------------------------------
// File: StickyNoteEditCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.06.2014
//
// Description:
// Edit command for sticky note items.
//-----------------------------------------------------------------------------

#include "StickyNoteEditCommand.h"

#include "StickyNote.h"

//-----------------------------------------------------------------------------
// Function: StickyNoteEditCommand()
//-----------------------------------------------------------------------------
StickyNoteEditCommand::StickyNoteEditCommand(StickyNote* noteItem, 
    QString const& newText, QString const& oldText, 
    QString const& newTimestamp, QString const& oldTimestamp, 
    QUndoCommand* parent) : 
    QUndoCommand(parent),
    note_(noteItem),
    newText_(newText),
    oldText_(oldText),
    newTimestamp_(newTimestamp),
    oldTimestamp_(oldTimestamp)
{
  
}

//-----------------------------------------------------------------------------
// Function: ~StickyNoteEditCommand()
//-----------------------------------------------------------------------------
StickyNoteEditCommand::~StickyNoteEditCommand()
{

}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void StickyNoteEditCommand::undo()
{
    note_->setText(oldText_);
    note_->setTimestamp(oldTimestamp_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void StickyNoteEditCommand::redo()
{
    note_->setText(newText_);
    note_->setTimestamp(newTimestamp_);

    QUndoCommand::redo();
}
