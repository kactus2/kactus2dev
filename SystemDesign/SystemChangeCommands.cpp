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

#include "SWConnectionEndpoint.h"
#include "SystemComponentItem.h"
#include "SWComponentItem.h"

#include <common/graphicsItems/ComponentItem.h>

#include <models/component.h>

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