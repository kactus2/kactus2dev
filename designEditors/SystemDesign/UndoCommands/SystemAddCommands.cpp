//-----------------------------------------------------------------------------
// File: SystemAddCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.10.2011
//
// Description:
// Undo add commands for system designs.
//-----------------------------------------------------------------------------

#include "SystemAddCommands.h"

#include <common/graphicsItems/ComponentItem.h>

#include <designEditors/SystemDesign/HWMappingItem.h>
#include <designEditors/SystemDesign/SWComponentItem.h>
#include <designEditors/SystemDesign/SystemColumn.h>
#include <designEditors/SystemDesign/SystemComponentItem.h>
#include <designEditors/SystemDesign/SWInterfaceItem.h>

#include <IPXACTmodels/Design/ComponentInstance.h>
#include <IPXACTmodels/kactusExtensions/SWInstance.h>

//-----------------------------------------------------------------------------
// Function: SWPortAddCommand()
//-----------------------------------------------------------------------------
SWPortAddCommand::SWPortAddCommand(SystemComponentItem* component, QPointF const& pos, QUndoCommand* parent):
QUndoCommand(parent),
component_(component),
pos_(pos),
port_(0),
scene_(component->scene()),
del_(false)
{
}

//-----------------------------------------------------------------------------
// Function: SWPortAddCommand()
//-----------------------------------------------------------------------------
SWPortAddCommand::SWPortAddCommand(SystemComponentItem* component, SWPortItem* port, QUndoCommand* parent):
QUndoCommand(parent),
component_(component),
pos_(),
port_(port),
scene_(component->scene()),
del_(false)
{
}

//-----------------------------------------------------------------------------
// Function: ~SWPortAddCommand()
//-----------------------------------------------------------------------------
SWPortAddCommand::~SWPortAddCommand()
{
    if (del_)
    {
        delete port_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SWPortAddCommand::undo()
{
    Q_ASSERT(port_ != 0);

    // Remove the port from the component and from the scene
    component_->removePort(port_);
    scene_->removeItem(port_);
    del_ = true;

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void SWPortAddCommand::redo()
{
    // Add a port to the component.
    if (port_ == 0)
    {
        port_ = component_->addPort(pos_);
    }
    else
    {
        component_->addPort(port_);
    }

    del_ = false;

    // Child commands need not be executed because the other ports change their position
    // in a deterministic way.
    //QUndoCommand::redo();
}
