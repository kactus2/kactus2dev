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

class SystemColumnLayout;
class SystemColumn;
class EndpointConnection;
class MappingComponentItem;
class ProgramEntityItem;
class EndpointItem;
class ComponentItem;
class IComponentStack;
class SWPortItem;

//-----------------------------------------------------------------------------
//! SystemColumnMoveCommand class.
//-----------------------------------------------------------------------------
class SystemColumnMoveCommand : public QUndoCommand
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
    SystemColumnMoveCommand(SystemColumnLayout* layout, SystemColumn* column,
                            QPointF const& oldPos, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~SystemColumnMoveCommand();

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
    SystemColumnMoveCommand(SystemColumnMoveCommand const& rhs);
    SystemColumnMoveCommand& operator=(SystemColumnMoveCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The column layout.
    SystemColumnLayout* layout_;

    //! The diagram column.
    SystemColumn* column_;

    //! The old position of the column.
    QPointF oldPos_;

    //! The new position of the column.
    QPointF newPos_;
};

//-----------------------------------------------------------------------------
//! EndpointConnectionMoveCommand class.
//-----------------------------------------------------------------------------
class EndpointConnectionMoveCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] conn      The endpoint connection.
     *      @param [in] oldRoute  The old route of the connection.
     *      @param [in] parent    The parent command.
     */
    EndpointConnectionMoveCommand(EndpointConnection* conn, QList<QPointF> const& oldRoute,
                                  QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~EndpointConnectionMoveCommand();

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
    EndpointConnectionMoveCommand(EndpointConnectionMoveCommand const& rhs);
    EndpointConnectionMoveCommand& operator=(EndpointConnectionMoveCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The endpoint connection.
    EndpointConnection* conn_;

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
    SystemItemMoveCommand(ComponentItem* item, QPointF const& oldPos,
                          IComponentStack* oldStack, QUndoCommand* parent = 0);

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
    ComponentItem* item_;

    //! The old position of the item.
    QPointF oldPos_;

    //! The old parent stack.
    IComponentStack* oldStack_;

    //! The new position of the item.
    QPointF newPos_;

    //! The new parent stack.
    IComponentStack* newStack_;
};

//-----------------------------------------------------------------------------
//! ProgramEntityMoveCommand class.
//-----------------------------------------------------------------------------
class ProgramEntityMoveCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] item    The item to move.
     *      @param [in] oldPos  The item's old position.
     *      @param [in] parent  The parent command.
     */
    ProgramEntityMoveCommand(ProgramEntityItem* item, QPointF const& oldPos, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ProgramEntityMoveCommand();

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
    ProgramEntityMoveCommand(ProgramEntityMoveCommand const& rhs);
    ProgramEntityMoveCommand& operator=(ProgramEntityMoveCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The program entity item.
    ProgramEntityItem* item_;

    //! The old position of the item.
    QPointF oldPos_;

    //! The new position of the item.
    QPointF newPos_;
};

//-----------------------------------------------------------------------------
//! EndpointMoveCommand class.
//-----------------------------------------------------------------------------
class EndpointMoveCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] item    The item to move.
     *      @param [in] oldPos  The item's old position.
     *      @param [in] parent  The parent command.
     */
    EndpointMoveCommand(EndpointItem* item, QPointF const& oldPos, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~EndpointMoveCommand();

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
    EndpointMoveCommand(EndpointMoveCommand const& rhs);
    EndpointMoveCommand& operator=(EndpointMoveCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The endpoint item.
    EndpointItem* item_;

    //! The old position of the item.
    QPointF oldPos_;

    //! The new position of the item.
    QPointF newPos_;
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
