//-----------------------------------------------------------------------------
// File: StickyNoteMoveCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.05.2014
//
// Description:
// Move command for floating items.
//-----------------------------------------------------------------------------

#ifndef STICKYNOTEMOVECOMMAND_H
#define STICKYNOTEMOVECOMMAND_H

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QObject>
#include <QPointF>

//-----------------------------------------------------------------------------
//! StickyNoteMoveCommand class.
//-----------------------------------------------------------------------------
class StickyNoteMoveCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] item      The item to move.
     *      @param [in] oldPos    The item's old position.
     *      @param [in] parent    The parent command.
     */
    StickyNoteMoveCommand(QGraphicsItem* item, QPointF const& oldPos,  QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~StickyNoteMoveCommand();

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
    StickyNoteMoveCommand(StickyNoteMoveCommand const& rhs);
    StickyNoteMoveCommand& operator=(StickyNoteMoveCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The graphics item.
    QGraphicsItem* item_;

    //! The old position of the item.
    QPointF oldPos_;

    //! The new position of the item.
    QPointF newPos_;

};
//-----------------------------------------------------------------------------

#endif // STICKYNOTEMOVECOMMAND_H