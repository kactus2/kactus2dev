//-----------------------------------------------------------------------------
// File: InterfaceDeleteCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.09.2015
//
// Description:
// Undo command for removing top interface item in design diagram.
//-----------------------------------------------------------------------------

#include "InterfaceDeleteCommand.h"

#include "AdHocVisibilityChangeCommand.h"
#include "ConnectionDeleteCommand.h"
#include "PhysicalPortDeleteCommand.h"

#include <designEditors/HWDesign/BusInterfaceItem.h>
#include <designEditors/HWDesign/HWComponentItem.h>
#include <designEditors/HWDesign/HWConnection.h>
#include <designEditors/HWDesign/HWConnectionEndpoint.h>

#include <common/graphicsItems/GraphicsColumn.h>

#include <designEditors/common/DesignDiagram.h>
#include <designEditors/common/Association/AssociationRemoveCommand.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: InterfaceDeleteCommand::InterfaceDeleteCommand()
//-----------------------------------------------------------------------------
InterfaceDeleteCommand::InterfaceDeleteCommand(DesignDiagram* diagram, BusInterfaceItem* interface,
                                               bool removePorts, QUndoCommand* parent):
QUndoCommand(parent),
interface_(interface),
busIf_(interface_->getBusInterface()),
parent_(static_cast<GraphicsColumn*>(interface->parentItem())),
diagram_(diagram),
del_(true)
{
    if (removePorts)
    {
        // Create copies of the related ports and remove commands for them.
        QList<QSharedPointer<Port> > ports = interface_->getPorts();
        foreach (QSharedPointer<Port> port, ports)
        {
            DeletePhysicalPortCommand* delCmd = new DeletePhysicalPortCommand(interface_->getOwnerComponent(), port, this);

            // If the port is visible as ad-hoc in the current design, it must be hidden.
            if (diagram->getDiagramAdHocPort(port->name()) != 0)
            {                               
                new AdHocVisibilityChangeCommand(diagram, port->name(), false, delCmd);
            }
        }
    }

    // Create child commands for removing interconnections.
    foreach (GraphicsConnection* conn, interface_->getConnections())
    {
        new ConnectionDeleteCommand(diagram, static_cast<HWConnection*>(conn), this);
    }

    foreach (GraphicsConnection* conn, interface_->getOffPageConnector()->getConnections())
    {
        new ConnectionDeleteCommand(diagram, static_cast<HWConnection*>(conn), this);
    }
}

//-----------------------------------------------------------------------------
// Function: InterfaceDeleteCommand::~InterfaceDeleteCommand()
//-----------------------------------------------------------------------------
InterfaceDeleteCommand::~InterfaceDeleteCommand()
{
    if (del_)
    {
        delete interface_;
    }
}

//-----------------------------------------------------------------------------
// Function: InterfaceDeleteCommand::undo()
//-----------------------------------------------------------------------------
void InterfaceDeleteCommand::undo()
{
    // Add the interface back to the scene.
    parent_->addItem(interface_);
    del_ = false;

    // Redefine the interface.
    if (busIf_ != 0)
    {
        interface_->define(busIf_);

        diagram_->getDesign()->getVendorExtensions()->append(interface_->getDataExtension());
    }

    // Execute child commands.
    QUndoCommand::undo();

    interface_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: InterfaceDeleteCommand::redo()
//-----------------------------------------------------------------------------
void InterfaceDeleteCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    // Remove the interface from the scene.
    parent_->removeItem(interface_);
    diagram_->removeItem(interface_);
    diagram_->getDesign()->getVendorExtensions()->removeOne(interface_->getDataExtension());

    emit interfaceDeleted();
    del_ = true;
}
