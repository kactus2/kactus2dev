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

#include "SystemColumn.h"
#include "SystemComponentItem.h"
#include "SWPortItem.h"
#include "SWInterfaceItem.h"
#include "SWComponentItem.h"

#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsConnection.h>

//-----------------------------------------------------------------------------
// Function: SystemColumnDeleteCommand()
//-----------------------------------------------------------------------------
SystemColumnDeleteCommand::SystemColumnDeleteCommand(GraphicsColumnLayout* layout, SystemColumn* column,
                                                     QUndoCommand* parent) : QUndoCommand(parent),
                                                                             layout_(layout),
                                                                             column_(column), del_(true)
{
    // Create child commands for removing connections.
    foreach (QGraphicsItem* item, column->childItems())
    {
        if (item->type() == SWComponentItem::Type)
        {
            SWComponentItem* compItem = static_cast<SWComponentItem*>(item);

            foreach (QGraphicsItem* childItem, compItem->childItems())
            {
                if (childItem->type() != SWPortItem::Type)
                {
                    continue;
                }

                SWPortItem* port = static_cast<SWPortItem*>(childItem);

                foreach (GraphicsConnection* conn, port->getConnections())
                {
                    QUndoCommand* cmd = new SWConnectionDeleteCommand(conn, this);
                }
            }
        }
        else if (item->type() == SWInterfaceItem::Type)
        {
            SWInterfaceItem* interface = static_cast<SWInterfaceItem*>(item);

            foreach (GraphicsConnection* conn, interface->getConnections())
            {
                QUndoCommand* cmd = new SWConnectionDeleteCommand(conn, this);
            }
        }
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
SWConnectionDeleteCommand::SWConnectionDeleteCommand(GraphicsConnection* conn, QUndoCommand* parent)
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
    // Execute child commands.
    QUndoCommand::undo();

    // Add the item back to the scene.
    scene_->addItem(conn_);

    // Connect the ends.
    conn_->connectEnds();

    scene_->clearSelection();
    conn_->setSelected(true);
    del_ = false;
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void SWConnectionDeleteCommand::redo()
{
    // Remove the item from the scene.
    conn_->setSelected(false);
    scene_->removeItem(conn_);

    // Disconnect the ends.
    conn_->disconnectEnds();
    del_ = true;

    // Execute child commands.
    QUndoCommand::redo();
}

//-----------------------------------------------------------------------------
// Function: SystemComponentDeleteCommand()
//-----------------------------------------------------------------------------
SystemComponentDeleteCommand::SystemComponentDeleteCommand(ComponentItem* item, QUndoCommand* parent)
    : QUndoCommand(parent),
      item_(item),
      stack_(dynamic_cast<IGraphicsItemStack*>(item->parentItem())),
      scene_(item->scene()),
      del_(true)
{
    // Create child commands for removing connections.
    foreach (QGraphicsItem* childItem, item->childItems())
    {
        if (childItem->type() != SWPortItem::Type)
        {
            continue;
        }

        SWPortItem* port = static_cast<SWPortItem*>(childItem);

        foreach (GraphicsConnection* conn, port->getConnections())
        {
            QUndoCommand* cmd = new SWConnectionDeleteCommand(conn, this);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ~SystemComponentDeleteCommand()
//-----------------------------------------------------------------------------
SystemComponentDeleteCommand::~SystemComponentDeleteCommand()
{
    if (del_)
    {
        delete item_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SystemComponentDeleteCommand::undo()
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
void SystemComponentDeleteCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    // Remove the item from the scene.
    stack_->removeItem(item_);
    scene_->removeItem(item_);
    del_ = true;

    emit componentInstanceRemoved(item_);
}

//-----------------------------------------------------------------------------
// Function: SWPortDeleteCommand()
//-----------------------------------------------------------------------------
SWPortDeleteCommand::SWPortDeleteCommand(SWPortItem* port, QUndoCommand* parent) :
    QUndoCommand(parent),
    port_(port),
    parent_(static_cast<SystemComponentItem*>(port->parentItem())),
    scene_(port->scene()),
    del_(true)
{
    // Create child commands for removing connection.
    foreach (GraphicsConnection* conn, port_->getConnections())
    {
        QUndoCommand* cmd = new SWConnectionDeleteCommand(conn, this);
    }

    foreach (GraphicsConnection* conn, port_->getOffPageConnector()->getConnections())
    {
        QUndoCommand* cmd =
            new SWConnectionDeleteCommand(conn, this);
    }
}

//-----------------------------------------------------------------------------
// Function: ~SWPortDeleteCommand()
//-----------------------------------------------------------------------------
SWPortDeleteCommand::~SWPortDeleteCommand()
{
    if (del_)
    {
        delete port_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SWPortDeleteCommand::undo()
{
    // Add the port back to the scene.
    scene_->addItem(port_);

    // Add the port to the parent component.
    parent_->addPort(port_);
    del_ = false;

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void SWPortDeleteCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    // Remove the port from the parent component.
    parent_->removePort(port_);

    // Remove the port from the scene.
    scene_->removeItem(port_);
    del_ = true;
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceDeleteCommand()
//-----------------------------------------------------------------------------
SWInterfaceDeleteCommand::SWInterfaceDeleteCommand(SWInterfaceItem* interface,
                                                   QUndoCommand* parent)
    : QUndoCommand(parent),
      interface_(interface),
      parent_(dynamic_cast<IGraphicsItemStack*>(interface->parentItem())),
      scene_(interface->scene()),
      del_(true)
{
    // Create child commands for removing interconnections.
    foreach (GraphicsConnection* conn, interface_->getConnections())
    {
        QUndoCommand* cmd = new SWConnectionDeleteCommand(conn, this);
    }
}

//-----------------------------------------------------------------------------
// Function: ~SWInterfaceDeleteCommand()
//-----------------------------------------------------------------------------
SWInterfaceDeleteCommand::~SWInterfaceDeleteCommand()
{
    if (del_)
    {
        delete interface_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SWInterfaceDeleteCommand::undo()
{
    // Add the interface back to the scene.
    parent_->addItem(interface_);
    del_ = false;

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void SWInterfaceDeleteCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    // Remove the interface from the scene.
    parent_->removeItem(interface_);
    scene_->removeItem(interface_);
    del_ = true;
}