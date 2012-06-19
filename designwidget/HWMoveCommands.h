//-----------------------------------------------------------------------------
// File: HWMoveCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.8.2011
//
// Description:
// Undo move commands for the design diagram.
//-----------------------------------------------------------------------------

#ifndef HWMOVECOMMANDS_H
#define HWMOVECOMMANDS_H

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QString>

class HWConnection;
class BusPortItem;
class HWComponentItem;
class BusInterfaceItem;
class HWConnectionEndpoint;

//-----------------------------------------------------------------------------
//! ItemMoveCommand class.
//-----------------------------------------------------------------------------
class ItemMoveCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] item    The item to move.
     *      @param [in] oldPos  The item's old position.
     *      @param [in] parent  The parent command.
     */
    ItemMoveCommand(QGraphicsItem* item, QPointF const& oldPos, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ItemMoveCommand();

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
    ItemMoveCommand(ItemMoveCommand const& rhs);
    ItemMoveCommand& operator=(ItemMoveCommand const& rhs);

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
//! PortMoveCommand class.
//-----------------------------------------------------------------------------
class PortMoveCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] port    The port to move.
     *      @param [in] oldPos  The port's old position.
     *      @param [in] parent  The parent command.
     */
    PortMoveCommand(HWConnectionEndpoint* port, QPointF const& oldPos, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~PortMoveCommand();

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
    PortMoveCommand(PortMoveCommand const& rhs);
    PortMoveCommand& operator=(PortMoveCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The graphics port.
    HWConnectionEndpoint* port_;

    //! The old position of the port.
    QPointF oldPos_;

    //! The new position of the port.
    QPointF newPos_;
};

//-----------------------------------------------------------------------------

#endif // HWMOVECOMMANDS_H
