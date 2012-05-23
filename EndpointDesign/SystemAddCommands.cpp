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

#include "../SystemDesign/SystemColumnLayout.h"
#include "../SystemDesign/SystemColumn.h"
#include "../SystemDesign/SWConnection.h"
#include "../SystemDesign/SWComponentItem.h"

#include <common/graphicsItems/ComponentItem.h>

//-----------------------------------------------------------------------------
// Function: SystemColumnAddCommand()
//-----------------------------------------------------------------------------
SystemColumnAddCommand::SystemColumnAddCommand(SystemColumnLayout* layout, QString const& name,
                                               QUndoCommand* parent) : QUndoCommand(parent), layout_(layout),
                                                                       name_(name), column_(0), del_(false)
{
}

//-----------------------------------------------------------------------------
// Function: ~SystemColumnAddCommand()
//-----------------------------------------------------------------------------
SystemColumnAddCommand::~SystemColumnAddCommand()
{
    if (del_)
    {
        delete column_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SystemColumnAddCommand::undo()
{
    Q_ASSERT(column_ != 0);

    // Remove the column from the layout.
    layout_->removeColumn(column_);
    del_ = true;
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void SystemColumnAddCommand::redo()
{
    // If this is the first time, create the column.
    if (column_ == 0)
    {
        layout_->addColumn(name_);
        column_ = layout_->getColumns().back();
    }
    else
    {
        // Otherwise add the existing column to the layout.
        layout_->addColumn(column_);
    }

    del_ = false;
}

//-----------------------------------------------------------------------------
// Function: SWConnectionAddCommand()
//-----------------------------------------------------------------------------
SWConnectionAddCommand::SWConnectionAddCommand(QGraphicsScene* scene, SWConnection* conn,
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
SystemItemAddCommand::SystemItemAddCommand(IComponentStack* stack, ComponentItem* item,
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

    emit componentInstanceRemoved(item_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void SystemItemAddCommand::redo()
{
    // Add the item to the column.
    stack_->addItem(item_);
    del_ = false;

    emit componentInstantiated(item_);
}

//-----------------------------------------------------------------------------
// Function: SWPortAddCommand()
//-----------------------------------------------------------------------------
SWPortAddCommand::SWPortAddCommand(SWComponentItem* component, QPointF const& pos,
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