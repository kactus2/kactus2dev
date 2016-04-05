//-----------------------------------------------------------------------------
// File: ReplaceSystemComponentCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 02.03.2016
//
// Description:
// Undo change command for replacing system component item.
//-----------------------------------------------------------------------------

#include "ReplaceSystemComponentCommand.h"

#include <common/graphicsItems/CommonGraphicsUndoCommands.h>
#include <common/graphicsItems/ConnectionUndoCommands.h>
#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsConnection.h>

#include <designEditors/common/Association/AssociationChangeEndpointCommand.h>
#include <designEditors/SystemDesign/SystemComponentItem.h>
#include <designEditors/SystemDesign/SWComponentItem.h>
#include <designEditors/SystemDesign/ComGraphicsConnection.h>
#include <designEditors/SystemDesign/ApiGraphicsConnection.h>

#include <designEditors/SystemDesign/UndoCommands/SystemMoveCommands.h>
#include <designEditors/SystemDesign/UndoCommands/SystemComponentDeleteCommand.h>
#include <designEditors/SystemDesign/UndoCommands/SystemComponentAddCommand.h>
#include <designEditors/SystemDesign/UndoCommands/ComConnectionExchangeCommand.h>
#include <designEditors/SystemDesign/UndoCommands/ApiConnectionExchangeCommand.h>
#include <designEditors/SystemDesign/UndoCommands/ComConnectionDeleteCommand.h>
#include <designEditors/SystemDesign/UndoCommands/ApiConnectionDeleteCommand.h>

#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: ReplaceSystemComponentCommand::ReplaceSystemComponentCommand()
//-----------------------------------------------------------------------------
ReplaceSystemComponentCommand::ReplaceSystemComponentCommand(SystemComponentItem* oldComp,
                                                             SystemComponentItem* newComp, bool existing,
                                                             bool keepOld, QSharedPointer<Design> containingDesign,
                                                             QUndoCommand* parent):
QUndoCommand(parent),
oldComp_(oldComp),
newComp_(newComp),
existing_(existing)
{
    // Create a move/add command for the new component.
    if (existing)
    {
        if (oldComp_->type() == newComp_->type())
        {
            new ItemMoveCommand(newComp_, newComp_->scenePos(), newComp_->getParentStack(), oldComp_->scenePos(),
                oldComp_->getParentStack(), this);

            if (keepOld)
            {
                new ItemMoveCommand(oldComp_, oldComp_->scenePos(), oldComp_->getParentStack(),
                    newComp_->scenePos(), newComp_->getParentStack(), this);
            }
        }
    }
    else
    {
        SystemComponentAddCommand* addCmd =
            new SystemComponentAddCommand(oldComp_->getParentStack(), newComp_, containingDesign, this);

        connect(addCmd, SIGNAL(componentInstantiated(ComponentItem*)),
            this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
        connect(addCmd, SIGNAL(componentInstanceRemoved(ComponentItem*)),
            this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);
    }

    changeConnections(oldComp_, newComp_, containingDesign);

    // Create a delete command for the old component if it should not be kept.
    if (!keepOld)
    {
        SystemComponentDeleteCommand* deleteCmd =
            new SystemComponentDeleteCommand(oldComp_, containingDesign, this);

        connect(deleteCmd, SIGNAL(componentInstantiated(ComponentItem*)),
            this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
        connect(deleteCmd, SIGNAL(componentInstanceRemoved(ComponentItem*)),
            this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

        foreach(Association* association, oldComp_->getAssociations())
        {
            new AssociationChangeEndpointCommand(association, oldComp_, newComp_, this);
        }
    }
    else
    {
        changeConnections(newComp_, oldComp_, containingDesign);
    }
}

//-----------------------------------------------------------------------------
// Function: ReplaceSystemComponentCommand::changeConnections()
//-----------------------------------------------------------------------------
void ReplaceSystemComponentCommand::changeConnections(SystemComponentItem* oldComponent,
    SystemComponentItem* newComponent, QSharedPointer<Design> design)
{
    QStringList connectionNames;

    foreach (ConnectionEndpoint* oldEndpoint, oldComponent->getEndpoints())
    {
        if (!oldEndpoint->getConnections().isEmpty() ||
            !oldEndpoint->getOffPageConnector()->getConnections().isEmpty())
        {
            SWPortItem* newEndpoint = newComponent->getSWPortMatchingOtherEndPoint(oldEndpoint);

            if (newEndpoint != 0)
            {
                // Create a move command to move the port to the same place where it is in the old component.
                new SWPortMoveCommand(newEndpoint, newEndpoint->pos(), oldEndpoint->pos(), this);

                // Exchange connections between the endpoints.
                foreach (GraphicsConnection* connection, oldEndpoint->getConnections())
                {
                    createConnectionExchangeCommand(connection, oldEndpoint, newEndpoint);
                }

                foreach (GraphicsConnection* connection, oldEndpoint->getOffPageConnector()->getConnections())
                {
                    createConnectionExchangeCommand(connection, oldEndpoint->getOffPageConnector(),
                        newEndpoint->getOffPageConnector());
                }
            }
            else
            {
                foreach (GraphicsConnection* connection, oldEndpoint->getConnections())
                {
                    if (!connectionNames.contains(connection->name()))
                    {
                        createConnectionDeleteCommand(connection, design);
                        connectionNames.append(connection->name());
                    }
                }
                foreach (GraphicsConnection* connection, oldEndpoint->getOffPageConnector()->getConnections())
                {
                    if (!connectionNames.contains(connection->name()))
                    {
                        createConnectionDeleteCommand(connection, design);
                        connectionNames.append(connection->name());
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ReplaceSystemComponentCommand::createConnectionExchangeCommand()
//-----------------------------------------------------------------------------
void ReplaceSystemComponentCommand::createConnectionExchangeCommand(GraphicsConnection* connection,
    ConnectionEndpoint* oldEndpoint, ConnectionEndpoint* newEndpoint)
{
    ComGraphicsConnection* comConnection = dynamic_cast<ComGraphicsConnection*>(connection);
    ApiGraphicsConnection* apiConnection = dynamic_cast<ApiGraphicsConnection*>(connection);
    if (comConnection)
    {
        new ComConnectionExchangeCommand(comConnection, oldEndpoint, newEndpoint, this);
    }
    else if (apiConnection)
    {
        new ApiConnectionExchangeCommand(apiConnection, oldEndpoint, newEndpoint, this);
    }
    else
    {
        new ConnectionExchangeCommand(connection, oldEndpoint, newEndpoint, this);
    }
}

//-----------------------------------------------------------------------------
// Function: ReplaceSystemComponentCommand::createConnectionDeleteCommands()
//-----------------------------------------------------------------------------
void ReplaceSystemComponentCommand::createConnectionDeleteCommand(GraphicsConnection* connection,
    QSharedPointer<Design> design)
{
    ComGraphicsConnection* comConnection = dynamic_cast<ComGraphicsConnection*>(connection);
    if (comConnection)
    {
        new ComConnectionDeleteCommand(comConnection, design, this);
    }
    else
    {
        ApiGraphicsConnection* apiConnection = dynamic_cast<ApiGraphicsConnection*>(connection);
        if (apiConnection)
        {
            new ApiConnectionDeleteCommand(apiConnection, design, this);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ReplaceSystemComponentCommand::~ReplaceSystemComponentCommand()
//-----------------------------------------------------------------------------
ReplaceSystemComponentCommand::~ReplaceSystemComponentCommand()
{
}

//-----------------------------------------------------------------------------
// Function: ReplaceSystemComponentCommand::undo()
//-----------------------------------------------------------------------------
void ReplaceSystemComponentCommand::undo()
{
    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: ReplaceSystemComponentCommand::redo()
//-----------------------------------------------------------------------------
void ReplaceSystemComponentCommand::redo()
{
    if (newComp_->type() == oldComp_->type() || !existing_)
    {
        // Place the new component to the exact same location as the old one.
        newComp_->setParentItem(0);
        newComp_->setPos(oldComp_->scenePos());
    }

    // Execute child commands.
    QUndoCommand::redo();
}
