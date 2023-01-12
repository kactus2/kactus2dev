//-----------------------------------------------------------------------------
// File: HWMoveCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 4.8.2011
//
// Description:
// Undo move commands for the design diagram.
//-----------------------------------------------------------------------------

#include "HWMoveCommands.h"

#include "HWConnection.h"
#include "HWComponentItem.h"
#include "columnview/HWColumn.h"

#include <common/graphicsItems/GraphicsColumnConstants.h>

#include <editors/common/DesignDiagram.h>
#include <editors/HWDesign/HWConnectionEndpoint.h>

//-----------------------------------------------------------------------------
// Function: ItemMoveCommand()
//-----------------------------------------------------------------------------
PortMoveCommand::PortMoveCommand(HWConnectionEndpoint* port, QPointF const& oldPos, DesignDiagram* diagram,
    QUndoCommand* parent):
QUndoCommand(parent),
port_(port),
oldPos_(oldPos),
newPos_(port->pos()),
diagram_(diagram)
{
}

//-----------------------------------------------------------------------------
// Function: PortMoveCommand::PortMoveCommand()
//-----------------------------------------------------------------------------
PortMoveCommand::PortMoveCommand(HWConnectionEndpoint* port, QPointF const& oldPos, QPointF const& newPos,
    DesignDiagram* diagram, QUndoCommand* parent):
QUndoCommand(parent),
port_(port),
oldPos_(oldPos),
newPos_(newPos),
diagram_(diagram)
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
    if (comp)
    {
        port_->setPos(oldPos_);
        comp->onMovePort(port_);
    }

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void PortMoveCommand::redo()
{
    HWComponentItem* comp = static_cast<HWComponentItem*>(port_->parentItem());
    if (comp)
    {
        port_->setPos(newPos_);
        comp->onMovePort(port_);
    }

    // Execute child commands.
    QUndoCommand::redo();

    if (port_->scenePos().y() + GraphicsColumnConstants::MIN_Y_PLACEMENT > port_->scene()->height())
    {
        diagram_->resetSceneRectangleForItems();
    }
}
