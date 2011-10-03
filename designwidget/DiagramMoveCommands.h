//-----------------------------------------------------------------------------
// File: DiagramMoveCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.8.2011
//
// Description:
// Undo move commands for the design diagram.
//-----------------------------------------------------------------------------

#ifndef DIAGRAMMOVECOMMANDS_H
#define DIAGRAMMOVECOMMANDS_H

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QString>

class DiagramInterconnection;
class DiagramPort;
class DiagramComponent;
class DiagramInterface;
class DiagramColumn;
class DiagramColumnLayout;

//-----------------------------------------------------------------------------
//! ColumnMoveCommand class.
//-----------------------------------------------------------------------------
class ColumnMoveCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] layout  The column layout.
     *      @param [in] column  The column to move.
     *      @param [in] oldPos  The column's old position.
     *      @param [in] parent  The parent command.
     */
    ColumnMoveCommand(DiagramColumnLayout* layout, DiagramColumn* column,
                      QPointF const& oldPos, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ColumnMoveCommand();

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
    ColumnMoveCommand(ColumnMoveCommand const& rhs);
    ColumnMoveCommand& operator=(ColumnMoveCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The column layout.
    DiagramColumnLayout* layout_;

    //! The diagram column.
    DiagramColumn* column_;

    //! The old position of the column.
    QPointF oldPos_;

    //! The new position of the column.
    QPointF newPos_;
};

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
//! ConnectionMoveCommand class.
//-----------------------------------------------------------------------------
class ConnectionMoveCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] conn      The interconnection.
     *      @param [in] oldRoute  The old route of the interconnection.
     *      @param [in] parent    The parent command.
     */
    ConnectionMoveCommand(DiagramInterconnection* conn, QList<QPointF> const& oldRoute,
                          QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ConnectionMoveCommand();

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
    ConnectionMoveCommand(ConnectionMoveCommand const& rhs);
    ConnectionMoveCommand& operator=(ConnectionMoveCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The interconnection.
    DiagramInterconnection* conn_;

    //! The old route of the interconnection.
    QList<QPointF> oldRoute_;

    //! The new route of the interconnection.
    QList<QPointF> newRoute_;
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
    PortMoveCommand(DiagramPort* port, QPointF const& oldPos, QUndoCommand* parent = 0);

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
    DiagramPort* port_;

    //! The old position of the port.
    QPointF oldPos_;

    //! The new position of the port.
    QPointF newPos_;
};

//-----------------------------------------------------------------------------

#endif // DIAGRAMMOVECOMMANDS_H
