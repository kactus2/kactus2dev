//-----------------------------------------------------------------------------
// File: StickyNoteEditCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.06.2014
//
// Description:
// Edit command for sticky note items.
//-----------------------------------------------------------------------------

#ifndef STICKYNOTEEDITCOMMAND_H
#define STICKYNOTEEDITCOMMAND_H

#include <QUndoCommand>

#include <QString>
#include <QSharedPointer>

class StickyNote;

//-----------------------------------------------------------------------------
//! StickyNoteEditCommand class.
//-----------------------------------------------------------------------------
class StickyNoteEditCommand : public QUndoCommand
{
public:

    /*!
     *  <Description>.
     *
     *      @param [in] noteItem        The updated note.
     *      @param [in] newText         The new text on the note.
     *      @param [in] oldText         The previous text on the note.
     *      @param [in] newTimestamp    The edit timestamp.
     *      @param [in] oldTimestamp    The previous edit timestamp.
     *      @param [in] parent          The parent command.
     */
    StickyNoteEditCommand(StickyNote* noteItem, QString const& newText, QString const& oldText, 
        QString const& newTimestamp, QString const& oldTimestamp, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~StickyNoteEditCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

private:
    // Disable copying.
    StickyNoteEditCommand(StickyNoteEditCommand const& rhs);
    StickyNoteEditCommand& operator=(StickyNoteEditCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The graphics item.
    StickyNote* note_;

    //! The note's updated text content.
    QString newText_;

    //! The note's previous text content.
    QString oldText_;

    //! The note's updated timestamp.
    QString newTimestamp_;

    //! The note's previous timestamp.
    QString oldTimestamp_;

};

#endif //STICKYNOTEEDITCOMMAND_H