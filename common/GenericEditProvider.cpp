//-----------------------------------------------------------------------------
// File: GenericEditProvider.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.7.2011
//
// Description:
// Generic edit provider implemented with stacks.
//-----------------------------------------------------------------------------

#include "GenericEditProvider.h"

//-----------------------------------------------------------------------------
// Function: GenericEditProvider()
//-----------------------------------------------------------------------------
GenericEditProvider::GenericEditProvider(int historySize)
    : historySize_(historySize),
      undoStack_(),
      redoStack_(),
      undoing_(false),
      redoing_(false)
{
}

//-----------------------------------------------------------------------------
// Function: ~GenericEditProvider()
//-----------------------------------------------------------------------------
GenericEditProvider::~GenericEditProvider()
{
}

//-----------------------------------------------------------------------------
// Function: addAction()
//-----------------------------------------------------------------------------
void GenericEditProvider::addCommand(QSharedPointer<QUndoCommand> command, bool autoExec)
{
    Q_ASSERT(undoStack_.size() <= historySize_);

    if (!undoing_ || !redoing_)
    {
        // Erase the redo stack.
        redoStack_.clear();

        // Add the action to the undo stack.
        undoStack_.append(command);

        // Execute the command if requested.
        if (autoExec)
        {
            command->redo();
        }

        // Check if the undo stack is too large and in that chase, remove the oldest undo command.
        if (undoStack_.size() > historySize_)
        {
            undoStack_.removeFirst();
        }

        // Inform others.
        emit editStateChanged();
        emit modified();
    }

    Q_ASSERT(undoStack_.size() <= historySize_);
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void GenericEditProvider::undo()
{
    // Check if we cannot undo.
    if (!canUndo())
    {
        return;
    }

    states_.clear();
    undoing_ = true;

    // Pop and undo the topmost command in the undo stack.
    QSharedPointer<QUndoCommand> command = undoStack_.last();
    undoStack_.removeLast();

    command->undo();

    // Push the command to the redo stack.
    redoStack_.push(command);

    undoing_ = false;
    emit editStateChanged();
    emit modified();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void GenericEditProvider::redo()
{
    // Check if we cannot redo.
    if (!canRedo())
    {
        return;
    }

    states_.clear();
    redoing_ = true;

    // Pop and redo the topmost command in the redo stack.
    QSharedPointer<QUndoCommand> command = redoStack_.pop();
    command->redo();

    // Push the command to the undo stack.
    undoStack_.append(command);

    redoing_ = false;
    emit editStateChanged();
    emit modified();
}

//-----------------------------------------------------------------------------
// Function: canUndo()
//-----------------------------------------------------------------------------
bool GenericEditProvider::canUndo() const
{
    return !undoStack_.empty();
}

//-----------------------------------------------------------------------------
// Function: canUndo()
//-----------------------------------------------------------------------------
bool GenericEditProvider::canRedo() const
{
    return !redoStack_.empty();
}

//-----------------------------------------------------------------------------
// Function: isPerformingUndoRedo()
//-----------------------------------------------------------------------------
bool GenericEditProvider::isPerformingUndoRedo() const
{
    return (undoing_ || redoing_);
}

//-----------------------------------------------------------------------------
// Function: clear()
//-----------------------------------------------------------------------------
void GenericEditProvider::clear()
{
    undoStack_.clear();
    redoStack_.clear();
    states_.clear();
    emit editStateChanged();
}

//-----------------------------------------------------------------------------
// Function: GenericEditProvider::setState()
//-----------------------------------------------------------------------------
void GenericEditProvider::setState(QString const& name, QVariant const& value)
{
    states_[name] = value;
}

//-----------------------------------------------------------------------------
// Function: GenericEditProvider::getState()
//-----------------------------------------------------------------------------
QVariant GenericEditProvider::getState(QString const& name) const
{
    return states_.value(name, QVariant());
}
