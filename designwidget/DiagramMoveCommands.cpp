//-----------------------------------------------------------------------------
// File: DiagramMoveCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Määttä
// Date: 4.8.2011
//
// Description:
// Undo move commands for the design diagram.
//-----------------------------------------------------------------------------

#include "DiagramMoveCommands.h"

#include "diagraminterconnection.h"
#include "diagramport.h"
#include "diagramcomponent.h"
#include "diagraminterface.h"
#include "columnview/DiagramColumn.h"

//-----------------------------------------------------------------------------
// Function: ItemMoveCommand()
//-----------------------------------------------------------------------------
ItemMoveCommand::ItemMoveCommand(QGraphicsItem* item, QPointF const& oldPos,
                                 QUndoCommand* parent) : QUndoCommand(parent), item_(item),
                                                         oldPos_(oldPos), newPos_(item->scenePos())
{
}

//-----------------------------------------------------------------------------
// Function: ~ItemMoveCommand()
//-----------------------------------------------------------------------------
ItemMoveCommand::~ItemMoveCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ItemMoveCommand::undo()
{
    DiagramColumn* oldColumn = static_cast<DiagramColumn*>(item_->parentItem());

    item_->setPos(oldColumn->mapFromScene(oldPos_));
    oldColumn->onMoveItem(item_);

    DiagramColumn* newColumn = static_cast<DiagramColumn*>(item_->parentItem());
    newColumn->onReleaseItem(item_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ItemMoveCommand::redo()
{
    DiagramColumn* oldColumn = static_cast<DiagramColumn*>(item_->parentItem());

    item_->setPos(oldColumn->mapFromScene(newPos_));
    oldColumn->onMoveItem(item_);

    DiagramColumn* newColumn = static_cast<DiagramColumn*>(item_->parentItem());
    newColumn->onReleaseItem(item_);

    // Execute child commands.
    QUndoCommand::redo();
}

//-----------------------------------------------------------------------------
// Function: ConnectionMoveCommand()
//-----------------------------------------------------------------------------
ConnectionMoveCommand::ConnectionMoveCommand(DiagramInterconnection* conn,
                                             QList<QPointF> const& oldRoute,
                                             QUndoCommand* parent) : QUndoCommand(parent),
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
// Function: ItemMoveCommand()
//-----------------------------------------------------------------------------
PortMoveCommand::PortMoveCommand(DiagramConnectionEndPoint* port, QPointF const& oldPos,
                                 QUndoCommand* parent) : QUndoCommand(parent), port_(port),
                                 oldPos_(oldPos), newPos_(port->pos())
{
}

//-----------------------------------------------------------------------------
// Function: ~PortMoveCommand()
//-----------------------------------------------------------------------------
PortMoveCommand::~PortMoveCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void PortMoveCommand::undo()
{
    DiagramComponent* comp = static_cast<DiagramComponent*>(port_->parentItem());

    port_->setPos(oldPos_);
    comp->onMovePort(port_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void PortMoveCommand::redo()
{
    DiagramComponent* comp = static_cast<DiagramComponent*>(port_->parentItem());

    port_->setPos(newPos_);
    comp->onMovePort(port_);

    // Execute child commands.
    QUndoCommand::redo();
}
