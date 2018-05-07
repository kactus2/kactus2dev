//-----------------------------------------------------------------------------
// File: ReplaceComponentCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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

#include <editors/common/DesignDiagram.h>
#include <editors/common/Association/AssociationChangeEndpointCommand.h>

#include <editors/HWDesign/HWMoveCommands.h>
#include <editors/HWDesign/AdHocConnectionItem.h>
#include <editors/HWDesign/HWConnection.h>
#include <editors/HWDesign/BusPortItem.h>
#include <editors/HWDesign/AdHocPortItem.h>
#include <editors/HWDesign/AdHocInterfaceItem.h>
#include <editors/HWDesign/HWComponentItem.h>

#include <editors/HWDesign/undoCommands/HWComponentAddCommand.h>
#include <editors/HWDesign/undoCommands/ConnectionDeleteCommand.h>
#include <editors/HWDesign/undoCommands/ComponentDeleteCommand.h>
#include <editors/HWDesign/undoCommands/AdHocConnectionDeleteCommand.h>
#include <editors/HWDesign/HWChangeCommands.h>

//-----------------------------------------------------------------------------
// Function: ReplaceComponentCommand::ReplaceComponentCommand()
//-----------------------------------------------------------------------------
ReplaceComponentCommand::ReplaceComponentCommand(HWComponentItem* oldComp, HWComponentItem* newComp,
    DesignDiagram* diagram, QUndoCommand* parent):
QUndoCommand(parent),
position_(oldComp->scenePos()),
newComp_(newComp),
diagram_(diagram)
{
    new ItemMoveCommand(newComp_, newComp_->scenePos(), newComp_->getParentStack(), position_,
        oldComp->getParentStack(), diagram, this);

    new ItemMoveCommand(oldComp, position_, oldComp->getParentStack(), newComp_->scenePos(),
        newComp_->getParentStack(), diagram, this);

    QStringList connectionNames;
    changeConnections(oldComp, newComp_, connectionNames);
    changeConnections(newComp_, oldComp, connectionNames);

    foreach(Association* association, oldComp->getAssociations())
    {
        new AssociationChangeEndpointCommand(association, oldComp, newComp_, this);
    }

    QSharedPointer<Design> design = diagram->getDesign();
    new ComponentChangeNameCommand(oldComp, newComp_->name(), design, this);
    new ComponentChangeNameCommand(newComp_, oldComp->name(), design, this);
}

//-----------------------------------------------------------------------------
// Function: ReplaceComponentCommand::changeConnections()
//-----------------------------------------------------------------------------
void ReplaceComponentCommand::changeConnections(HWComponentItem* oldComponentItem, 
    HWComponentItem* newComponentItem, QStringList& connectionNames)
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
                createConnectionExchangeCommands(oldEndpoint, newEndpoint, connectionNames);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ReplaceComponentCommand::createConnectionExchangeCommands()
//-----------------------------------------------------------------------------
void ReplaceComponentCommand::createConnectionExchangeCommands(ConnectionEndpoint* oldEndpoint, 
    HWConnectionEndpoint* newEndpoint, QStringList& connectionNames)
{
    // Create a move command to move the port to the same place where it is in the old component.
    new PortMoveCommand(newEndpoint, newEndpoint->pos(), oldEndpoint->pos(), diagram_, this);

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
    newComp_->setPos(position_);

    // Execute child commands.
    QUndoCommand::redo();
}
