//-----------------------------------------------------------------------------
// File: SystemChangeCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 19.10.2011
//
// Description:
// Undo change commands for system designs.
//-----------------------------------------------------------------------------

#include "SystemChangeCommands.h"

#include <common/graphicsItems/CommonGraphicsUndoCommands.h>
#include <common/graphicsItems/ConnectionUndoCommands.h>
#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsConnection.h>

#include <editors/common/Association/AssociationChangeEndpointCommand.h>

#include <editors/SystemDesign/UndoCommands/SystemMoveCommands.h>
#include <editors/SystemDesign/UndoCommands/SystemDeleteCommands.h>
#include <editors/SystemDesign/UndoCommands/SystemComponentDeleteCommand.h>

#include <editors/SystemDesign/SWConnectionEndpoint.h>
#include <editors/SystemDesign/SystemComponentItem.h>
#include <editors/SystemDesign/SWComponentItem.h>

#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: TypeDefinitionChangeCommand()
//-----------------------------------------------------------------------------
TypeDefinitionChangeCommand::TypeDefinitionChangeCommand(SWConnectionEndpoint* endpoint, VLNV const& oldType,
                                                         QUndoCommand* parent):
QUndoCommand(parent),
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
                                                         QUndoCommand* parent):
QUndoCommand(parent),
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
                                                 QUndoCommand* parent):
QUndoCommand(parent),
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
