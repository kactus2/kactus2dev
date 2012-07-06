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
class IGraphicsItemStack;

//-----------------------------------------------------------------------------
//! PortMoveCommand class.
//-----------------------------------------------------------------------------
class PortMoveCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor which assumes that the port has already been moved to its new position.
     *
     *      @param [in] port    The port to move.
     *      @param [in] oldPos  The port's old position.
     *      @param [in] parent  The parent command.
     */
    PortMoveCommand(HWConnectionEndpoint* port, QPointF const& oldPos, QUndoCommand* parent = 0);

    /*!
     *  Constructor.
     *
     *      @param [in] port    The port to move.
     *      @param [in] oldPos  The port's old position.
     *      @param [in] newPos  The port's new position.
     *      @param [in] parent  The parent command.
     */
    PortMoveCommand(HWConnectionEndpoint* port, QPointF const& oldPos,
                    QPointF const& newPos, QUndoCommand* parent = 0);

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
