//-----------------------------------------------------------------------------
// File: PortDeleteCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 30.09.2015
//
// Description:
// Undo command for removing an interface item in design diagram.
//-----------------------------------------------------------------------------

#include "PortDeleteCommand.h"
#include "PhysicalPortDeleteCommand.h"

#include "ConnectionDeleteCommand.h"

#include <editors/HWDesign/HWComponentItem.h>
#include <editors/HWDesign/HWConnection.h>
#include <editors/HWDesign/HWConnectionEndpoint.h>

#include <editors/common/DesignDiagram.h>

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
    for (GraphicsConnection* conn : port_->getConnections())
    {
        new ConnectionDeleteCommand(diagram, static_cast<HWConnection*>(conn), this);
    }

    for (GraphicsConnection* conn : port_->getOffPageConnector()->getConnections())
    {
        new ConnectionDeleteCommand(diagram, static_cast<HWConnection*>(conn), this);
    }

    if (port_->getBusInterface()->getAllPortMaps())
    {
        auto const& allPortMaps = port_->getBusInterface()->getAllPortMaps();
        for (auto const& portMap : *allPortMaps)
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
