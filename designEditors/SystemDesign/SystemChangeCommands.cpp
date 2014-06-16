//-----------------------------------------------------------------------------
// File: SystemChangeCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.10.2011
//
// Description:
// Undo change commands for system designs.
//-----------------------------------------------------------------------------

#include "SystemChangeCommands.h"

#include "SystemMoveCommands.h"
#include "SystemDeleteCommands.h"
#include "SWConnectionEndpoint.h"
#include "SystemComponentItem.h"
#include "SWComponentItem.h"

#include <common/graphicsItems/CommonGraphicsUndoCommands.h>
#include <common/graphicsItems/ConnectionUndoCommands.h>
#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsConnection.h>

#include <designEditors/common/Association/AssociationChangeEndpointCommand.h>

#include <IPXACTmodels/component.h>

//-----------------------------------------------------------------------------
// Function: TypeDefinitionChangeCommand()
//-----------------------------------------------------------------------------
TypeDefinitionChangeCommand::TypeDefinitionChangeCommand(SWConnectionEndpoint* endpoint,
                                                         VLNV const& oldType,
                                                         QUndoCommand* parent)
    : QUndoCommand(parent),
      endpoint_(endpoint),
      oldType_(oldType),
      newType_(endpoint->getTypeDefinition())
{
}

//-----------------------------------------------------------------------------
// Function: ~TypeDefinitionChangeCommand()
//-----------------------------------------------------------------------------
TypeDefinitionChangeCommand::~TypeDefinitionChangeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void TypeDefinitionChangeCommand::undo()
{
    endpoint_->setTypeDefinition(oldType_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void TypeDefinitionChangeCommand::redo()
{
    endpoint_->setTypeDefinition(newType_);
}

//-----------------------------------------------------------------------------
// Function: PropertyValuesChangeCommand()
//-----------------------------------------------------------------------------
PropertyValuesChangeCommand::PropertyValuesChangeCommand(SystemComponentItem* component, 
                                                         QMap<QString, QString> const& newPropertyValues, 
                                                         QUndoCommand* parent)
    : QUndoCommand(parent),
      component_(component),
      oldPropertyValues_(component->getPropertyValues()),
      newPropertyValues_(newPropertyValues)
{
}

//-----------------------------------------------------------------------------
// Function: ~PropertyValuesChangeCommand()
//-----------------------------------------------------------------------------
PropertyValuesChangeCommand::~PropertyValuesChangeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void PropertyValuesChangeCommand::undo()
{
    component_->setPropertyValues(oldPropertyValues_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void PropertyValuesChangeCommand::redo()
{
    component_->setPropertyValues(newPropertyValues_);
}

//-----------------------------------------------------------------------------
// Function: FileSetRefChangeCommand()
//-----------------------------------------------------------------------------
FileSetRefChangeCommand::FileSetRefChangeCommand(SWComponentItem* component, QString const& newFileSetRef,
                                                 QUndoCommand* parent)
    : QUndoCommand(parent),
      component_(component),
      oldFileSetRef_(component->getFileSetRef()),
      newFileSetRef_(newFileSetRef)
{
}

//-----------------------------------------------------------------------------
// Function: ~FileSetRefChangeCommand()
//-----------------------------------------------------------------------------
FileSetRefChangeCommand::~FileSetRefChangeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void FileSetRefChangeCommand::undo()
{
    component_->setFileSetRef(oldFileSetRef_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void FileSetRefChangeCommand::redo()
{
    component_->setFileSetRef(newFileSetRef_);
}

//-----------------------------------------------------------------------------
// Function: ReplaceSystemComponentCommand::ReplaceSystemComponentCommand()
//-----------------------------------------------------------------------------
ReplaceSystemComponentCommand::ReplaceSystemComponentCommand(SystemComponentItem* oldComp,
                                                             SystemComponentItem* newComp,
                                                             bool existing, bool keepOld,
                                                             QUndoCommand* parent)
    : QUndoCommand(parent),
      oldComp_(oldComp),
      newComp_(newComp),
      existing_(existing)
{
    foreach (ConnectionEndpoint* oldEndpoint, oldComp_->getEndpoints())
    {
        SWPortItem* newEndpoint = newComp_->getSWPort(oldEndpoint->name(), oldEndpoint->getType());
        
        if (newEndpoint != 0)
        {
            // Create a move command to move the port to the same place where it is in the old component.
            new SWPortMoveCommand(newEndpoint, newEndpoint->pos(), oldEndpoint->pos(), this);

            // Exchange connections between the endpoints.
            foreach (GraphicsConnection* conn, oldEndpoint->getConnections())
            {
                new ConnectionExchangeCommand(conn, oldEndpoint, newEndpoint, this);
            }

            foreach (GraphicsConnection* conn, oldEndpoint->getOffPageConnector()->getConnections())
            {
                new ConnectionExchangeCommand(conn, oldEndpoint->getOffPageConnector(),
                                              newEndpoint->getOffPageConnector(), this);
            }
        }
    }

    // Create a delete command for the old component if it should not be kept.
    if (!keepOld)
    {
        SystemComponentDeleteCommand* deleteCmd = new SystemComponentDeleteCommand(oldComp_, this);

        connect(deleteCmd, SIGNAL(componentInstantiated(ComponentItem*)),
            this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
        connect(deleteCmd, SIGNAL(componentInstanceRemoved(ComponentItem*)),
            this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);

        foreach(Association* association, oldComp_->getAssociations())
        {
            new AssociationChangeEndpointCommand(association, oldComp_, newComp_, this);
        }
    }

    // Create a move/add command for the new component.
    if (existing)
    {
        if (oldComp_->type() == newComp_->type())
        {
            new ItemMoveCommand(newComp_, newComp_->scenePos(),
                                newComp_->getParentStack(), oldComp_->scenePos(),
                                oldComp_->getParentStack(), this);
        }
    }
    else
    {
        ItemAddCommand* addCmd = new ItemAddCommand(oldComp_->getParentStack(), newComp_, this);

        connect(addCmd, SIGNAL(componentInstantiated(ComponentItem*)),
            this, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
        connect(addCmd, SIGNAL(componentInstanceRemoved(ComponentItem*)),
            this, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);
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
