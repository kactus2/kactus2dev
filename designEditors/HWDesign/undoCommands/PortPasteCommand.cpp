//-----------------------------------------------------------------------------
// File: PortPasteCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 15.02.2016
//
// Description:
// Paste undo/redo command for interface items in component instances.
//-----------------------------------------------------------------------------

#include "PortPasteCommand.h"

#include <designEditors/HWDesign/BusPortItem.h>
#include <designEditors/HWDesign/HWComponentItem.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: PortPasteCommand::PortPasteCommand()
//-----------------------------------------------------------------------------
PortPasteCommand::PortPasteCommand(HWComponentItem* destComponent, BusPortItem* port, QUndoCommand* parent) : 
QUndoCommand(parent),
    component_(destComponent), 
    port_(port), 
    scene_(destComponent->scene())
{
   
}

//-----------------------------------------------------------------------------
// Function: PortPasteCommand::~PortPasteCommand()
//-----------------------------------------------------------------------------
PortPasteCommand::~PortPasteCommand()
{
    if (del_)
    {
        delete port_;
    }
}

//-----------------------------------------------------------------------------
// Function: PortPasteCommand::undo()
//-----------------------------------------------------------------------------
void PortPasteCommand::undo()
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
// Function: PortPasteCommand::redo()
//-----------------------------------------------------------------------------
void PortPasteCommand::redo()
{
    QUndoCommand::redo();

    Q_ASSERT(port_ != 0);

    // Copy a port to the component.
    component_->addPort(port_);
    port_->updateInterface();

    del_ = false;
}
