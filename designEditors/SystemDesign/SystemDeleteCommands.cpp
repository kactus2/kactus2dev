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

#include <designEditors/SystemDesign/ComGraphicsConnection.h>
#include <designEditors/SystemDesign/ComConnectionDeleteCommand.h>
#include <designEditors/SystemDesign/ApiGraphicsConnection.h>
#include <designEditors/SystemDesign/ApiConnectionDeleteCommand.h>

#include <designEditors/common/Association/Association.h>
#include <designEditors/common/Association/AssociationRemoveCommand.h>

//-----------------------------------------------------------------------------
// Function: SystemColumnDeleteCommand()
//-----------------------------------------------------------------------------
SystemColumnDeleteCommand::SystemColumnDeleteCommand(GraphicsColumnLayout* layout, SystemColumn* column,
                                                     QSharedPointer<Design> containingDesign,
                                                     QUndoCommand* parent /* = 0 */):
QUndoCommand(parent),
layout_(layout),
column_(column), del_(true)
{
    // Create child commands for removing connections.
    QList<GraphicsConnection*> connections;

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
                    if (!connections.contains(conn))
                    {
                        addConnectionDeleteCommand(conn);
                        connections.append(conn);
                    }
                }

                if (port->getOffPageConnector() != 0)
                {
                    foreach (GraphicsConnection* conn, port->getOffPageConnector()->getConnections())
                    {
                        if (!connections.contains(conn))
                        {
                            addConnectionDeleteCommand(conn);
                            connections.append(conn);
                        }
                    }
                }
            }
            foreach(Association* association, compItem->getAssociations())
            {
                new AssociationRemoveCommand(association, association->scene(), this);
            }
        }
        else if (item->type() == SWInterfaceItem::Type)
        {
            SWInterfaceItem* interface = static_cast<SWInterfaceItem*>(item);

            foreach (GraphicsConnection* conn, interface->getConnections())
            {
                if (!connections.contains(conn))
                {
                    addConnectionDeleteCommand(conn);
                    connections.append(conn);
                }
            }

            if (interface->getOffPageConnector() != 0)
            {
                foreach (GraphicsConnection* conn, interface->getOffPageConnector()->getConnections())
                {
                    if (!connections.contains(conn))
                    {
                        addConnectionDeleteCommand(conn);
                        connections.append(conn);
                    }
                }
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
// Function: SystemDeleteCommands::addConnectionDeleteCommand()
//-----------------------------------------------------------------------------
void SystemColumnDeleteCommand::addConnectionDeleteCommand(GraphicsConnection* connection)
{
    //                         new SWConnectionDeleteCommand(conn, this);
    //                         connections.append(conn);

    ApiGraphicsConnection* apiConnection = dynamic_cast<ApiGraphicsConnection*>(connection);
    if (apiConnection)
    {
        new ApiConnectionDeleteCommand(apiConnection, containingDesign_, this);
    }
    else
    {
        ComGraphicsConnection* comConnection = dynamic_cast<ComGraphicsConnection*>(connection);
        if (comConnection)
        {
            new ComConnectionDeleteCommand(comConnection, containingDesign_, this);
        }
    }
}
