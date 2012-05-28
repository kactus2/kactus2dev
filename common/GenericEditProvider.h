//-----------------------------------------------------------------------------
// File: GenericEditProvider.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.7.2011
//
// Description:
// Generic edit provider implemented with stacks.
//-----------------------------------------------------------------------------

#ifndef GENERICEDITPROVIDER_H
#define GENERICEDITPROVIDER_H

#include "IEditProvider.h"

#include <QStack>
#include <QList>
#include <QSharedPointer>
#include <QUndoCommand>

//-----------------------------------------------------------------------------
//! GenericEditProvider class.
//-----------------------------------------------------------------------------
class GenericEditProvider : public IEditProvider
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] historySize The maximum number of undo commands kept in the history.
     */
    GenericEditProvider(int historySize);

    /*!
     *  Destructor.
     */
    virtual ~GenericEditProvider();

    /*!
     *  Stores a new command to the edit stack.
     *
     *      @param [in] command   The command to add.
     *      @param [in] autoExec  If true, the command's redo() is automatically executed
     *                            inside this function.
     */
    void addCommand(QSharedPointer<QUndoCommand> command, bool autoExec = true);

    /*!
     *  Clears the undo & redo stacks.
     */
    void clear();

    /*!
     *  Performs an undo.
     */
    virtual void undo();

    /*!
     *  Performs a redo.
     */
    virtual void redo();

    /*!
     *  Returns true if undo is possible.
     */
    virtual bool canUndo() const;

    /*!
     *  Returns true if redo is possible.
     */
    virtual bool canRedo() const;

    /*!
     *  Returns true if the an undo/redo is currently being performed.
     */
    bool isPerformingUndoRedo() const;

private:
    // Disable copying.
    GenericEditProvider(GenericEditProvider const& rhs);
    GenericEditProvider& operator=(GenericEditProvider const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The history size.
    int historySize_;

    //! Undo stack.
    QList< QSharedPointer<QUndoCommand> > undoStack_;

    //! Redo stack.
    QStack< QSharedPointer<QUndoCommand> > redoStack_;

    //! Boolean flag for undoing.
    bool undoing_;

    //! Boolean flag for redoing.
    bool redoing_;
};

//-----------------------------------------------------------------------------

#endif // GENERICEDITPROVIDER_H
