//-----------------------------------------------------------------------------
// File: SystemMoveCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.10.2011
//
// Description:
// Undo move commands for system designs.
//-----------------------------------------------------------------------------

#ifndef SYSTEMMOVECOMMANDS_H
#define SYSTEMMOVECOMMANDS_H

#include <QUndoCommand>
#include <QGraphicsScene>
#include <QString>

class SystemColumn;
class ComponentItem;
class IGraphicsItemStack;
class SWPortItem;
class GraphicsConnection;

//-----------------------------------------------------------------------------
//! SWConnectionMoveCommand class.
//-----------------------------------------------------------------------------
class SWConnectionMoveCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] conn      The SW connection.
     *      @param [in] oldRoute  The old route of the connection.
     *      @param [in] parent    The parent command.
     */
    SWConnectionMoveCommand(GraphicsConnection* conn, QList<QPointF> const& oldRoute,
                            QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SWConnectionMoveCommand();

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
    SWConnectionMoveCommand(SWConnectionMoveCommand const& rhs);
    SWConnectionMoveCommand& operator=(SWConnectionMoveCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The SW connection.
    GraphicsConnection* conn_;

    //! The old route of the connection.
    QList<QPointF> oldRoute_;

    //! The new route of the connection.
    QList<QPointF> newRoute_;
};

//-----------------------------------------------------------------------------
//! SystemItemMoveCommand class.
//-----------------------------------------------------------------------------
class SystemItemMoveCommand : public QUndoCommand
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
    SystemItemMoveCommand(QGraphicsItem* item, QPointF const& oldPos,
                          IGraphicsItemStack* oldStack, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SystemItemMoveCommand();

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
    SystemItemMoveCommand(SystemItemMoveCommand const& rhs);
    SystemItemMoveCommand& operator=(SystemItemMoveCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The graphics item.
    QGraphicsItem* item_;

    //! The old position of the item.
    QPointF oldPos_;

    //! The old parent stack.
    IGraphicsItemStack* oldStack_;

    //! The new position of the item.
    QPointF newPos_;

    //! The new parent stack.
    IGraphicsItemStack* newStack_;
};

//-----------------------------------------------------------------------------
//! SWPortMoveCommand class.
//-----------------------------------------------------------------------------
class SWPortMoveCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] port    The port to move.
     *      @param [in] oldPos  The port's old position.
     *      @param [in] parent  The parent command.
     */
    SWPortMoveCommand(SWPortItem* port, QPointF const& oldPos, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SWPortMoveCommand();

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
    SWPortMoveCommand(SWPortMoveCommand const& rhs);
    SWPortMoveCommand& operator=(SWPortMoveCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The port item.
    SWPortItem* port_;

    //! The old position of the port.
    QPointF oldPos_;

    //! The new position of the port.
    QPointF newPos_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMMOVECOMMANDS_H
