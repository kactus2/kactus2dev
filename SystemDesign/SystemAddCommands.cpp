//-----------------------------------------------------------------------------
// File: SystemAddCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.10.2011
//
// Description:
// Undo add commands for system designs.
//-----------------------------------------------------------------------------

#include "SystemAddCommands.h"

#include "SystemColumn.h"
#include "SystemComponentItem.h"

#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsConnection.h>

//-----------------------------------------------------------------------------
// Function: SWConnectionAddCommand()
//-----------------------------------------------------------------------------
SWConnectionAddCommand::SWConnectionAddCommand(QGraphicsScene* scene, GraphicsConnection* conn,
                                               QUndoCommand* parent)
    : QUndoCommand(parent),
      conn_(conn),
      scene_(scene),
      del_(false)
{
}

//-----------------------------------------------------------------------------
// Function: ~SWConnectionAddCommand()
//-----------------------------------------------------------------------------
SWConnectionAddCommand::~SWConnectionAddCommand()
{
    if (del_)
    {
        delete conn_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SWConnectionAddCommand::undo()
{
    // Remove the interconnection from the scene.
    scene_->removeItem(conn_);
    conn_->setSelected(false);

    // Disconnect the ends.
    conn_->disconnectEnds();
    del_ = true;
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void SWConnectionAddCommand::redo()
{
    // Add the back to the scene.
    if (!scene_->items().contains(conn_))
    {
        scene_->addItem(conn_);
    }

    // Connect the ends and set the interface modes and port map for the hierarchical end point.
    conn_->connectEnds();
    del_ = false;
}

//-----------------------------------------------------------------------------
// Function: SystemItemAddCommand()
//-----------------------------------------------------------------------------
SystemItemAddCommand::SystemItemAddCommand(IGraphicsItemStack* stack, QGraphicsItem* item,
                                           QUndoCommand* parent)
    : QUndoCommand(parent),
      item_(item),
      stack_(stack),
      del_(false)
{
}

//-----------------------------------------------------------------------------
// Function: ~SystemItemAddCommand()
//-----------------------------------------------------------------------------
SystemItemAddCommand::~SystemItemAddCommand()
{
    if (del_)
    {
        delete item_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SystemItemAddCommand::undo()
{
    // Remove the item from the column and the scene.
    stack_->removeItem(item_);
    item_->scene()->removeItem(item_);
    del_ = true;

    if (dynamic_cast<ComponentItem*>(item_) != 0)
    {
        emit componentInstanceRemoved(static_cast<ComponentItem*>(item_));
    }
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void SystemItemAddCommand::redo()
{
    // Add the item to the column.
    stack_->addItem(item_);
    del_ = false;

    if (dynamic_cast<ComponentItem*>(item_) != 0)
    {
        emit componentInstantiated(static_cast<ComponentItem*>(item_));
    }
}

//-----------------------------------------------------------------------------
// Function: SWPortAddCommand()
//-----------------------------------------------------------------------------
SWPortAddCommand::SWPortAddCommand(SystemComponentItem* component, QPointF const& pos,
                                           QUndoCommand* parent)
    : QUndoCommand(parent),
      component_(component),
      pos_(pos),
      port_(0),
      scene_(component->scene()),
      del_(false)
{
}

//-----------------------------------------------------------------------------
// Function: ~SWPortAddCommand()
//-----------------------------------------------------------------------------
SWPortAddCommand::~SWPortAddCommand()
{
    if (del_)
    {
        delete port_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SWPortAddCommand::undo()
{
    Q_ASSERT(port_ != 0);

    // Remove the port from the component and from the scene
    component_->removePort(port_);
    scene_->removeItem(port_);
    del_ = true;

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void SWPortAddCommand::redo()
{
    // Add a port to the component.
    if (port_ == 0)
    {
        port_ = component_->addPort(pos_);
    }
    else
    {
        component_->addPort(port_);
    }

    del_ = false;

    // Child commands need not be executed because the other ports change their position
    // in a deterministic way.
    //QUndoCommand::redo();
}