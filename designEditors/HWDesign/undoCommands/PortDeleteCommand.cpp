//-----------------------------------------------------------------------------
// File: PortDeleteCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.09.2015
//
// Description:
// Undo command for removing an interface item in design diagram.
//-----------------------------------------------------------------------------

#include "PortDeleteCommand.h"
#include "PhysicalPortDeleteCommand.h"

#include "ConnectionDeleteCommand.h"

#include <designEditors/HWDesign/HWComponentItem.h>
#include <designEditors/HWDesign/HWConnection.h>
#include <designEditors/HWDesign/HWConnectionEndpoint.h>

#include <designEditors/common/DesignDiagram.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>

//-----------------------------------------------------------------------------
// Function: PortDeleteCommand::PortDeleteCommand()
//-----------------------------------------------------------------------------
PortDeleteCommand::PortDeleteCommand(DesignDiagram* diagram, HWConnectionEndpoint* port, QUndoCommand* parent) :
QUndoCommand(parent), 
    port_(port), 
    componentItem_(static_cast<HWComponentItem*>(port->parentItem())),
    scene_(port->scene()), 
    del_(true)
{
    // Create child commands for removing interconnections.
    foreach (GraphicsConnection* conn, port_->getConnections())
    {
        new ConnectionDeleteCommand(diagram, static_cast<HWConnection*>(conn), this);
    }

    foreach (GraphicsConnection* conn, port_->getOffPageConnector()->getConnections())
    {
        new ConnectionDeleteCommand(diagram, static_cast<HWConnection*>(conn), this);
    }

    if (port_->getBusInterface()->getPortMaps())
    {
        foreach (QSharedPointer<PortMap> portMap, *port_->getBusInterface()->getPortMaps())
        {
            new DeletePhysicalPortCommand(componentItem_->componentModel(),
                componentItem_->componentModel()->getPort(portMap->getPhysicalPort()->name_),
                this);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortDeleteCommand::~PortDeleteCommand()
//-----------------------------------------------------------------------------
PortDeleteCommand::~PortDeleteCommand()
{
    if (del_)
    {
        delete port_;
    }
}

//-----------------------------------------------------------------------------
// Function: PortDeleteCommand::undo()
//-----------------------------------------------------------------------------
void PortDeleteCommand::undo()
{
    // Add the port to the parent component.
    componentItem_->addPort(port_);

    del_ = false;

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: PortDeleteCommand::redo()
//-----------------------------------------------------------------------------
void PortDeleteCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    // Remove the port from the parent component.
    componentItem_->removePort(port_);

    // Remove the port from the scene.
    scene_->removeItem(port_);
    del_ = true;
}
