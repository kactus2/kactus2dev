//-----------------------------------------------------------------------------
// File: FloatingItemMoveCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.05.2014
//
// Description:
// Move command for floating items.
//-----------------------------------------------------------------------------

#ifndef FLOATINGITEMMOVECOMMAND_H
#define FLOATINGITEMMOVECOMMAND_H

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QObject>
#include <QPointF>

//-----------------------------------------------------------------------------
//! FloatingItemMoveCommand class.
//-----------------------------------------------------------------------------
class FloatingItemMoveCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] item      The item to move.
     *      @param [in] oldPos    The item's old position.
     *      @param [in] oldStack  The item's old stack.
     *      @param [in] parent    The parent command.
     */
    FloatingItemMoveCommand(QGraphicsItem* item, QPointF const& oldPos,  QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~FloatingItemMoveCommand();

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
    FloatingItemMoveCommand(FloatingItemMoveCommand const& rhs);
    FloatingItemMoveCommand& operator=(FloatingItemMoveCommand const& rhs);

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

#endif // FLOATINGITEMMOVECOMMAND_H