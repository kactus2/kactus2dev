//-----------------------------------------------------------------------------
// File: HWMoveCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Määttä
// Date: 4.8.2011
//
// Description:
// Undo move commands for the design diagram.
//-----------------------------------------------------------------------------

#include "HWMoveCommands.h"

#include "HWConnection.h"
#include "BusPortItem.h"
#include "HWComponentItem.h"
#include "BusInterfaceItem.h"
#include "columnview/HWColumn.h"

//-----------------------------------------------------------------------------
// Function: ItemMoveCommand()
//-----------------------------------------------------------------------------
PortMoveCommand::PortMoveCommand(HWConnectionEndpoint* port, QPointF const& oldPos,
                                 QUndoCommand* parent) : QUndoCommand(parent), port_(port),
                                 oldPos_(oldPos), newPos_(port->pos())
{
}

//-----------------------------------------------------------------------------
// Function: PortMoveCommand::PortMoveCommand()
//-----------------------------------------------------------------------------
PortMoveCommand::PortMoveCommand(HWConnectionEndpoint* port, QPointF const& oldPos,
                                 QPointF const& newPos, QUndoCommand* parent /*= 0*/)
    : QUndoCommand(parent),
      port_(port),
      oldPos_(oldPos),
      newPos_(newPos)
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
    HWComponentItem* comp = static_cast<HWComponentItem*>(port_->parentItem());

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
    HWComponentItem* comp = static_cast<HWComponentItem*>(port_->parentItem());

    port_->setPos(newPos_);
    comp->onMovePort(port_);

    // Execute child commands.
    QUndoCommand::redo();
}
