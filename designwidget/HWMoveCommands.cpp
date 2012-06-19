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
    HWColumn* oldColumn = static_cast<HWColumn*>(item_->parentItem());

    item_->setPos(oldColumn->mapFromScene(oldPos_));
    oldColumn->onMoveItem(item_);

    HWColumn* newColumn = static_cast<HWColumn*>(item_->parentItem());
    newColumn->onReleaseItem(item_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ItemMoveCommand::redo()
{
    HWColumn* oldColumn = static_cast<HWColumn*>(item_->parentItem());

    item_->setPos(oldColumn->mapFromScene(newPos_));
    oldColumn->onMoveItem(item_);

    HWColumn* newColumn = static_cast<HWColumn*>(item_->parentItem());
    newColumn->onReleaseItem(item_);

    // Execute child commands.
    QUndoCommand::redo();
}

//-----------------------------------------------------------------------------
// Function: ItemMoveCommand()
//-----------------------------------------------------------------------------
PortMoveCommand::PortMoveCommand(HWConnectionEndpoint* port, QPointF const& oldPos,
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
