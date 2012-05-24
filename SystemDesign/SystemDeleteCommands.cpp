//-----------------------------------------------------------------------------
// File: SystemDeleteCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 12.10.2011
//
// Description:
// Undo delete commands for system designs.
//-----------------------------------------------------------------------------

#include "SystemDeleteCommands.h"

#include "SystemColumnLayout.h"
#include "SystemColumn.h"
#include "SWConnection.h"

#include <common/graphicsItems/ComponentItem.h>

//-----------------------------------------------------------------------------
// Function: SystemColumnDeleteCommand()
//-----------------------------------------------------------------------------
SystemColumnDeleteCommand::SystemColumnDeleteCommand(SystemColumnLayout* layout, SystemColumn* column,
                                                     QUndoCommand* parent) : QUndoCommand(parent),
                                                                             layout_(layout),
                                                                             column_(column), del_(true)
{
    // Create child commands for removing connections.
    foreach (QGraphicsItem* item, column->childItems())
    {
        //TODO:
//         if (item->type() == MappingComponentItem::Type)
//         {
//             MappingComponentItem* comp = static_cast<MappingComponentItem*>(item);
// 
//             foreach (QGraphicsItem* childItem, comp->childItems())
//             {
//                 if (childItem->type() != ProgramEntityItem::Type)
//                 {
//                     continue;
//                 }
// 
//                 ProgramEntityItem* progEntity = static_cast<ProgramEntityItem*>(childItem);
// 
//                 foreach (EndpointItem* endpoint, progEntity->getEndpoints())
//                 {
//                     foreach (EndpointConnection* conn, endpoint->getConnections())
//                     {
//                         QUndoCommand* cmd = new EndpointConnectionDeleteCommand(conn, this);
//                     }
//                 }
//             }
//         }
    }
}

//-----------------------------------------------------------------------------
// Function: ~SystemColumnDeleteCommand()
//-----------------------------------------------------------------------------
SystemColumnDeleteCommand::~SystemColumnDeleteCommand()
{
    if (del_)
    {
        delete column_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SystemColumnDeleteCommand::undo()
{
    // Add the item back to the layout.
    layout_->addColumn(column_);
    del_ = false;

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void SystemColumnDeleteCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    // Remove the item from the layout.
    layout_->removeColumn(column_);
    del_ = true;
}

//-----------------------------------------------------------------------------
// Function: SWConnectionDeleteCommand()
//-----------------------------------------------------------------------------
SWConnectionDeleteCommand::SWConnectionDeleteCommand(SWConnection* conn, QUndoCommand* parent)
    : QUndoCommand(parent),
      conn_(conn),
      scene_(conn->scene()),
      del_(true)
{
}

//-----------------------------------------------------------------------------
// Function: ~SWConnectionDeleteCommand()
//-----------------------------------------------------------------------------
SWConnectionDeleteCommand::~SWConnectionDeleteCommand()
{
    if (del_)
    {
        delete conn_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SWConnectionDeleteCommand::undo()
{
    // Add the item back to the scene.
    scene_->addItem(conn_);

    // Connect the ends.
    conn_->connectEnds();
    del_ = false;
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void SWConnectionDeleteCommand::redo()
{
    // Remove the item from the scene.
    scene_->removeItem(conn_);

    // Disconnect the ends.
    conn_->disconnectEnds();
    del_ = true;
}

//-----------------------------------------------------------------------------
// Function: SystemItemDeleteCommand()
//-----------------------------------------------------------------------------
SystemItemDeleteCommand::SystemItemDeleteCommand(ComponentItem* item, QUndoCommand* parent)
    : QUndoCommand(parent),
      item_(item),
      stack_(dynamic_cast<IComponentStack*>(item->parentItem())),
      scene_(item->scene()),
      del_(true)
{
    // Create child commands for removing connections. TODO: Update when the old system design has been removed.
    foreach (QGraphicsItem* childItem, item->childItems())
    {
//         if (childItem->type() != ProgramEntityItem::Type)
//         {
//             continue;
//         }
// 
//         ProgramEntityItem* progEntity = static_cast<ProgramEntityItem*>(childItem);
// 
//         foreach (EndpointItem* endpoint, progEntity->getEndpoints())
//         {
//             foreach (EndpointConnection* conn, endpoint->getConnections())
//             {
//                 QUndoCommand* cmd = new EndpointConnectionDeleteCommand(conn, this);
//             }
//         }
    }
}

//-----------------------------------------------------------------------------
// Function: ~SystemItemDeleteCommand()
//-----------------------------------------------------------------------------
SystemItemDeleteCommand::~SystemItemDeleteCommand()
{
    if (del_)
    {
        delete item_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SystemItemDeleteCommand::undo()
{
    // Add the item back to the scene.
    stack_->addItem(item_);
    del_ = false;

    emit componentInstantiated(item_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void SystemItemDeleteCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    // Remove the item from the scene.
    stack_->removeItem(item_);
    scene_->removeItem(item_);
    del_ = true;

    emit componentInstanceRemoved(item_);
}