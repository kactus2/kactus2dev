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
                                 IGraphicsItemStack* oldStack, QUndoCommand* parent)
    : QUndoCommand(parent),
      item_(item),
      oldPos_(oldPos),
      oldStack_(oldStack),
      newPos_(item->scenePos()),
      newStack_(dynamic_cast<IGraphicsItemStack*>(item->parentItem()))
{
    Q_ASSERT(oldStack != 0);
}

//-----------------------------------------------------------------------------
// Function: ItemMoveCommand::ItemMoveCommand()
//-----------------------------------------------------------------------------
ItemMoveCommand::ItemMoveCommand(QGraphicsItem* item, QPointF const& oldPos, IGraphicsItemStack* oldStack,
                                 QPointF const& newPos, IGraphicsItemStack* newStack,
                                 QUndoCommand* parent /*= 0*/)
    : QUndoCommand(parent),
      item_(item),
      oldPos_(oldPos),
      oldStack_(oldStack),
      newPos_(newPos),
      newStack_(newStack)
{
    Q_ASSERT(oldStack != 0);
    Q_ASSERT(newStack != 0);
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
    newStack_->removeItem(item_);

    item_->setPos(oldStack_->mapStackFromScene(oldPos_));
    oldStack_->addItem(item_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ItemMoveCommand::redo()
{
    oldStack_->removeItem(item_);

    item_->setPos(newStack_->mapStackFromScene(newPos_));
    newStack_->addItem(item_);

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
