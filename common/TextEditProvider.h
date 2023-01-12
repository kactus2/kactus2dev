//-----------------------------------------------------------------------------
// File: TextEditProvider.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 28.7.2011
//
// Description:
// Specialized edit provider for QPlainTextEdit.
//-----------------------------------------------------------------------------

#ifndef TEXTEDITPROVIDER_H
#define TEXTEDITPROVIDER_H

#include "IEditProvider.h"

#include <QPlainTextEdit>

//-----------------------------------------------------------------------------
//! TextEditProvider class.
//-----------------------------------------------------------------------------
class TextEditProvider : public IEditProvider
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] textEdit The text editor to which the provider applies.
     */
    explicit TextEditProvider(QPlainTextEdit& textEdit);

    /*!
     *  Destructor.
     */
    virtual ~TextEditProvider() = default;

    // Disable copying.
    TextEditProvider(TextEditProvider const& rhs) = delete;
    TextEditProvider& operator=(TextEditProvider const& rhs) = delete;

    /*!
     *  Stores a new command to the edit stack.
     *
     *      @param [in] command   The command to add.
     */   
    virtual void addCommand(QSharedPointer<QUndoCommand> command) override final;

    /*!
     *  Clears the undo & redo stacks.
     */
    virtual void clear() override final;

    /*!
     *  Performs an undo.
     */
    virtual void undo() override final;

    /*!
     *  Performs a redo.
     */
    virtual void redo() override final;

    /*!
     *  Returns true if undo is possible.
     */
    virtual bool canUndo() const override final;

    /*!
     *  Returns true if redo is possible.
     */
    virtual bool canRedo() const override final;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The text edit.
    QPlainTextEdit& textEdit_;    
};

//-----------------------------------------------------------------------------

#endif // TEXTEDITPROVIDER_H
