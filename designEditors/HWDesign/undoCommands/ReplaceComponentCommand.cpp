//-----------------------------------------------------------------------------
// File: ReplaceComponentCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.04.2016
//
// Description:
// Undo command for replacing a HW component instance with another one.
//-----------------------------------------------------------------------------

#include "ReplaceComponentCommand.h"

#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/ConnectionUndoCommands.h>
#include <common/graphicsItems/CommonGraphicsUndoCommands.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/graphicsItems/GraphicsColumn.h>
#include <common/graphicsItems/GraphicsConnection.h>

#include <designEditors/common/DesignDiagram.h>
#include <designEditors/common/Association/AssociationChangeEndpointCommand.h>

#include <designEditors/HWDesign/HWMoveCommands.h>
#include <designEditors/HWDesign/AdHocConnectionItem.h>
#include <designEditors/HWDesign/HWConnection.h>
#include <designEditors/HWDesign/BusPortItem.h>
#include <designEditors/HWDesign/AdHocPortItem.h>
#include <designEditors/HWDesign/AdHocInterfaceItem.h>
#include <designEditors/HWDesign/HWComponentItem.h>

#include <designEditors/HWDesign/undoCommands/HWComponentAddCommand.h>
#include <designEditors/HWDesign/undoCommands/ConnectionDeleteCommand.h>
#include <designEditors/HWDesign/undoCommands/ComponentDeleteCommand.h>
#include <designEditors/HWDesign/undoCommands/AdHocConnectionDeleteCommand.h>

//-----------------------------------------------------------------------------
// Function: ReplaceComponentCommand::ReplaceComponentCommand()
//-----------------------------------------------------------------------------
ReplaceComponentCommand::ReplaceComponentCommand(DesignDiagram* diagram, HWComponentItem* oldComp,
    HWComponentItem* newComp, bool existing, bool keepOld, QUndoCommand* parent):
QUndoCommand(parent),
oldComp_(oldComp),
newComp_(newComp)
{
    // Create a move/add command for the new component.
    if (existing)
    {
        new ItemMoveCommand(newComp_, newComp_->scenePos(), newComp_->getParentStack(), oldComp_->scenePos(),
            oldComp_->getParentStack(), this);

        if (keepOld)
        {
            new ItemMoveCommand(oldComp_, oldComp_->scenePos(), oldComp_->getParentStack(), newComp_->scenePos(),
                newComp_->getParentStack(), this);
        }
    }
    else
    {
        HWComponentAddCommand* addCmd =
            new HWComponentAddCommand(diagram->getDesign(), oldComp_->getParentStack(), newComp_, this);

        connect(addCmd, SIGNAL(componentInstantiated(ComponentItem*)),
            this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
        connect(addCmd, SIGNAL(componentInstanceRemoved(ComponentItem*)),
            this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);
    }

    QStringList connectionNames;

    changeConnections(existing, connectionNames, oldComp_, newComp_, diagram);

    // Create a delete command for the old component if it should not be kept.
    if (!keepOld)
    {
        GraphicsColumn* column = diagram->getLayout()->findColumnAt(oldComp_->scenePos());
        ComponentDeleteCommand* deleteCmd = new ComponentDeleteCommand(diagram, column, oldComp_, this);

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
        changeConnections(true, connectionNames, newComp_, oldComp_, diagram);
    }
}

//-----------------------------------------------------------------------------
// Function: ReplaceComponentCommand::changeConnections()
//-----------------------------------------------------------------------------
void ReplaceComponentCommand::changeConnections(bool keepExistingComponent, QStringList& connectionNames,
    HWComponentItem* oldComponentItem, HWComponentItem* newComponentItem, DesignDiagram* diagram)
{
    foreach (ConnectionEndpoint* oldEndpoint, oldComponentItem->getEndpoints())
    {
        if (!oldEndpoint->getConnections().isEmpty() || 
            (oldEndpoint->getOffPageConnector() && !oldEndpoint->getOffPageConnector()->getConnections().isEmpty()))
        {
            HWConnectionEndpoint* newEndpoint = 0;

            if (oldEndpoint->type() == BusPortItem::Type)
            {
                newEndpoint = newComponentItem->getBusPort(oldEndpoint->name());
            }
            else if (oldEndpoint->type() == AdHocPortItem::Type)
            {
                newEndpoint = newComponentItem->getAdHocPort(oldEndpoint->name());
            }

            if (newEndpoint != 0)
            {
                createConnectionExchangeCommands(connectionNames, oldEndpoint, newEndpoint);
            }
            else if (keepExistingComponent)
            {
                createConnectionDeleteCommands(connectionNames, oldEndpoint, diagram);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ReplaceComponentCommand::createConnectionExchangeCommands()
//-----------------------------------------------------------------------------
void ReplaceComponentCommand::createConnectionExchangeCommands(QStringList& connectionNames,
    ConnectionEndpoint* oldEndpoint, HWConnectionEndpoint* newEndpoint)
{
    // Create a move command to move the port to the same place where it is in the old component.
    new PortMoveCommand(newEndpoint, newEndpoint->pos(), oldEndpoint->pos(), this);

    // Exchange connections between the endpoints.
    foreach (GraphicsConnection* connection, oldEndpoint->getConnections())
    {
        if (!connectionNames.contains(connection->name()))
        {
            new ConnectionExchangeCommand(connection, oldEndpoint, newEndpoint, this);
            connectionNames.append(connection->name());
        }
    }

    if (oldEndpoint->getOffPageConnector() &&
        !oldEndpoint->getOffPageConnector()->getConnections().isEmpty())
    {
        foreach (GraphicsConnection* connection, oldEndpoint->getOffPageConnector()->getConnections())
        {
            if (!connectionNames.contains(connection->name()))
            {
                new ConnectionExchangeCommand(connection, oldEndpoint->getOffPageConnector(),
                    newEndpoint->getOffPageConnector(), this);
                connectionNames.append(connection->name());
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ReplaceComponentCommand::createConnectionDeleteCommands()
//-----------------------------------------------------------------------------
void ReplaceComponentCommand::createConnectionDeleteCommands(QStringList& connectionNames,
    ConnectionEndpoint* deletedEndPoint, DesignDiagram* diagram)
{
    foreach (GraphicsConnection* connection, deletedEndPoint->getConnections())
    {
        createSingleConnectionDeleteCommand(connectionNames, connection, diagram);
    }

    if (deletedEndPoint->getOffPageConnector() && 
        !deletedEndPoint->getOffPageConnector()->getConnections().isEmpty())
    {
        foreach (GraphicsConnection* connection, deletedEndPoint->getOffPageConnector()->getConnections())
        {
            createSingleConnectionDeleteCommand(connectionNames, connection, diagram);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ReplaceComponentCommand::createSingleConnectionDeleteCommand()
//-----------------------------------------------------------------------------
void ReplaceComponentCommand::createSingleConnectionDeleteCommand(QStringList& connectionNames,
    GraphicsConnection* connection, DesignDiagram* diagram)
{
    if (!connectionNames.contains(connection->name()))
    {
        HWConnection* hwConnection = dynamic_cast<HWConnection*>(connection);
        if (hwConnection)
        {
            new ConnectionDeleteCommand(diagram, hwConnection, this);
            connectionNames.append(connection->name());
        }
        else
        {
            AdHocConnectionItem* adHocConnection = dynamic_cast<AdHocConnectionItem*>(connection);
            if (adHocConnection)
            {
                new AdHocConnectionDeleteCommand(diagram, adHocConnection, this);
                connectionNames.append(connection->name());
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ReplaceComponentCommand::~ReplaceComponentCommand()
//-----------------------------------------------------------------------------
ReplaceComponentCommand::~ReplaceComponentCommand()
{
}

//-----------------------------------------------------------------------------
// Function: ReplaceComponentCommand::undo()
//-----------------------------------------------------------------------------
void ReplaceComponentCommand::undo()
{
    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: ReplaceComponentCommand::redo()
//-----------------------------------------------------------------------------
void ReplaceComponentCommand::redo()
{
    // Place the new component to the exact same location as the old one.
    newComp_->setParentItem(0);
    newComp_->setPos(oldComp_->scenePos());

    // Execute child commands.
    QUndoCommand::redo();
}
