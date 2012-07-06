//-----------------------------------------------------------------------------
// File: SystemMoveCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.10.2011
//
// Description:
// Undo move commands for system designs.
//-----------------------------------------------------------------------------

#include "SystemMoveCommands.h"

#include "SystemColumn.h"
#include "SystemComponentItem.h"

#include <common/graphicsItems/GraphicsConnection.h>

//-----------------------------------------------------------------------------
// Function: SWConnectionMoveCommand()
//-----------------------------------------------------------------------------
SWConnectionMoveCommand::SWConnectionMoveCommand(GraphicsConnection* conn,
                                                 QList<QPointF> const& oldRoute,
                                                 QUndoCommand* parent) : QUndoCommand(parent),
                                                 conn_(conn),
                                                 oldRoute_(oldRoute),
                                                 newRoute_(conn->route())
{
}

//-----------------------------------------------------------------------------
// Function: ~SWConnectionMoveCommand()
//-----------------------------------------------------------------------------
SWConnectionMoveCommand::~SWConnectionMoveCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SWConnectionMoveCommand::undo()
{
    conn_->setRoute(oldRoute_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void SWConnectionMoveCommand::redo()
{
    conn_->setRoute(newRoute_);
}

//-----------------------------------------------------------------------------
// Function: ItemMoveCommand()
//-----------------------------------------------------------------------------
SWPortMoveCommand::SWPortMoveCommand(SWPortItem* port, QPointF const& oldPos,
                                     QUndoCommand* parent)
    : QUndoCommand(parent),
      port_(port),
      oldPos_(oldPos),
      newPos_(port->pos())
{
}

//-----------------------------------------------------------------------------
// Function: SWPortMoveCommand::SWPortMoveCommand()
//-----------------------------------------------------------------------------
SWPortMoveCommand::SWPortMoveCommand(SWPortItem* port, QPointF const& oldPos,
                                     QPointF const& newPos, QUndoCommand* parent /*= 0*/)
    : QUndoCommand(parent),
      port_(port),
      oldPos_(oldPos),
      newPos_(newPos)
{

}

//-----------------------------------------------------------------------------
// Function: ~SWPortMoveCommand()
//-----------------------------------------------------------------------------
SWPortMoveCommand::~SWPortMoveCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SWPortMoveCommand::undo()
{
    SystemComponentItem* comp = static_cast<SystemComponentItem*>(port_->parentItem());

    port_->setPos(oldPos_);
    comp->onMovePort(port_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void SWPortMoveCommand::redo()
{
    SystemComponentItem* comp = static_cast<SystemComponentItem*>(port_->parentItem());

    port_->setPos(newPos_);
    comp->onMovePort(port_);

    // Execute child commands.
    QUndoCommand::redo();
}