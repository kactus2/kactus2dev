//-----------------------------------------------------------------------------
// File: ConnectionUndoCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 5.6.2012
//
// Description:
// Undo commands for graphical connections.
//-----------------------------------------------------------------------------

#include "ConnectionUndoCommands.h"

#include "GraphicsConnection.h"

#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: ConnectionMoveCommand()
//-----------------------------------------------------------------------------
ConnectionMoveCommand::ConnectionMoveCommand(GraphicsConnection* conn,
                                             QList<QPointF> const& oldRoute,
                                             QUndoCommand* parent)
    : QUndoCommand(parent),
      conn_(conn), oldRoute_(oldRoute),
      newRoute_(conn->route())
{
}

//-----------------------------------------------------------------------------
// Function: ~ConnectionMoveCommand()
//-----------------------------------------------------------------------------
ConnectionMoveCommand::~ConnectionMoveCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ConnectionMoveCommand::undo()
{
    conn_->setRoute(oldRoute_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ConnectionMoveCommand::redo()
{
    conn_->setRoute(newRoute_);
}

//-----------------------------------------------------------------------------
// Function: ConnectionChangeCommand()
//-----------------------------------------------------------------------------
ConnectionChangeCommand::ConnectionChangeCommand(GraphicsConnection* connection, 
                                                 const QString& newName, 
                                                 const QString& newDescription,
                                                 QUndoCommand* parent /*= 0*/ )
    : QUndoCommand(parent),
      connection_(connection),
      newName_(newName),
      newDescription_(newDescription),
      oldName_(connection->name()),
      oldDescription_(connection->description())
{
}

//-----------------------------------------------------------------------------
// Function: ~ConnectionChangeCommand()
//-----------------------------------------------------------------------------
ConnectionChangeCommand::~ConnectionChangeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ConnectionChangeCommand::undo()
{
    connection_->setName(oldName_);
    connection_->setDescription(oldDescription_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ConnectionChangeCommand::redo()
{
    connection_->setName(newName_);
    connection_->setDescription(newDescription_);
}

//-----------------------------------------------------------------------------
// Function: ConnectionExchangeCommand::ConnectionExchangeCommand()
//-----------------------------------------------------------------------------
ConnectionExchangeCommand::ConnectionExchangeCommand(GraphicsConnection* connection,
                                                     ConnectionEndpoint* oldEndpoint,
                                                     ConnectionEndpoint* newEndpoint,
                                                     QUndoCommand* parent)
    : QUndoCommand(parent),
      conn_(connection),
      oldEndpoint_(oldEndpoint),
      newEndpoint_(newEndpoint)
{
}

//-----------------------------------------------------------------------------
// Function: ConnectionExchangeCommand::~ConnectionExchangeCommand()
//-----------------------------------------------------------------------------
ConnectionExchangeCommand::~ConnectionExchangeCommand()
{

}

//-----------------------------------------------------------------------------
// Function: ConnectionExchangeCommand::undo()
//-----------------------------------------------------------------------------
void ConnectionExchangeCommand::undo()
{
    if (conn_->endpoint1() == newEndpoint_)
    {
        conn_->setEndpoint1(oldEndpoint_);
    }
    else
    {
        conn_->setEndpoint2(oldEndpoint_);
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectionExchangeCommand::redo()
//-----------------------------------------------------------------------------
void ConnectionExchangeCommand::redo()
{
    if (conn_->endpoint1() == oldEndpoint_)
    {
        conn_->setEndpoint1(newEndpoint_);
    }
    else
    {
        conn_->setEndpoint2(newEndpoint_);
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectionDeleteCommand()
//-----------------------------------------------------------------------------
ConnectionToggleOffPageCommand::ConnectionToggleOffPageCommand(GraphicsConnection* conn, QUndoCommand* parent)
    : QUndoCommand(parent),
    conn_(conn)
{
}

//-----------------------------------------------------------------------------
// Function: ~ConnectionToggleOffPageCommand()
//-----------------------------------------------------------------------------
ConnectionToggleOffPageCommand::~ConnectionToggleOffPageCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ConnectionToggleOffPageCommand::undo()
{
    QUndoCommand::undo();

    conn_->scene()->clearSelection();
    conn_->toggleOffPage();
    conn_->setSelected(true);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ConnectionToggleOffPageCommand::redo()
{
    conn_->scene()->clearSelection();
    conn_->toggleOffPage();
    conn_->setSelected(true);

    QUndoCommand::redo();
}

