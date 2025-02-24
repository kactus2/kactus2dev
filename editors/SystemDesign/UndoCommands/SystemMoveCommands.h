//-----------------------------------------------------------------------------
// File: SystemMoveCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
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

class SWPortItem;
class GraphicsConnection;
class DesignDiagram;

//-----------------------------------------------------------------------------
//! SWConnectionMoveCommand class.
//-----------------------------------------------------------------------------
class SWConnectionMoveCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *    @param [in] conn      The SW connection.
     *    @param [in] oldRoute  The old route of the connection.
     *    @param [in] parent    The parent command.
     */
    SWConnectionMoveCommand(GraphicsConnection* conn, QList<QPointF> const& oldRoute, QUndoCommand* parent = 0);

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
//! SWPortMoveCommand class.
//-----------------------------------------------------------------------------
class SWPortMoveCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *    @param [in] port        The port to move.
     *    @param [in] oldPos      The port's old position.
     *    @param [in] diagram     Diagram containing the scene of the port.
     *    @param [in] parent      The parent command.
     */
    SWPortMoveCommand(SWPortItem* port, QPointF const& oldPos, DesignDiagram* diagram, QUndoCommand* parent = 0);

    /*!
     *  Constructor.
     *
     *    @param [in] port        The port to move.
     *    @param [in] oldPos      The port's old position.
     *    @param [in] newPos      The port's new position.
     *    @param [in] diagram     Diagram containing the scene of the port.
     *    @param [in] parent      The parent command.
     */
    SWPortMoveCommand(SWPortItem* port, QPointF const& oldPos, QPointF const& newPos, DesignDiagram* diagram,
        QUndoCommand* parent = 0);

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

    //! Diagram containing the scene of the port.
    DesignDiagram* diagram_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMMOVECOMMANDS_H
