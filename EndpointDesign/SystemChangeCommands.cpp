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

#include <common/graphicsItems/ComponentItem.h>

#include <models/component.h>

//-----------------------------------------------------------------------------
// Function: SWComponentPacketizeCommand()
//-----------------------------------------------------------------------------
SWComponentPacketizeCommand::SWComponentPacketizeCommand(ComponentItem* component,
                                                         VLNV const& vlnv,
                                                         QUndoCommand* parent) : QUndoCommand(parent),
                                                                                 component_(component),
                                                                                 vlnv_(vlnv)
{
}

//-----------------------------------------------------------------------------
// Function: ~SWComponentPacketizeCommand()
//-----------------------------------------------------------------------------
SWComponentPacketizeCommand::~SWComponentPacketizeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SWComponentPacketizeCommand::undo()
{
    // Set an empty VLNV.
    component_->componentModel()->setVlnv(VLNV());
    component_->updateComponent();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void SWComponentPacketizeCommand::redo()
{
    component_->componentModel()->setVlnv(vlnv_);
    component_->updateComponent();
}