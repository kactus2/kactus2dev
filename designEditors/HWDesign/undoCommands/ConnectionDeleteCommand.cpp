//-----------------------------------------------------------------------------
// File: ConnectionDeleteCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.09.2015
//
// Description:
// Undo command for removing a connection in design diagram.
//-----------------------------------------------------------------------------

#include "ConnectionDeleteCommand.h"
#include "PhysicalPortDeleteCommand.h"

#include "AdHocVisibilityChangeCommand.h"

#include <common/GenericEditProvider.h>

#include <designEditors/common/Association/Association.h>
#include <designEditors/common/Association/AssociationRemoveCommand.h>

#include <designEditors/common/DesignDiagram.h>

#include <designEditors/HWDesign/HWConnection.h>
#include <designEditors/HWDesign/BusInterfaceItem.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: ConnectionDeleteCommand::ConnectionDeleteCommand()
//-----------------------------------------------------------------------------
ConnectionDeleteCommand::ConnectionDeleteCommand(DesignDiagram* diagram, HWConnection* connection,
     QUndoCommand* parent) : QUndoCommand(parent),
      connection_(connection),
      mode1_(General::MASTER),
      mode2_(General::MASTER),
      portMaps_(),
      portsCopied_(false),
      diagram_(diagram),
      del_(true)
{
    QSharedPointer<BusInterface> busIf1 = connection_->endpoint1()->getBusInterface();
    QSharedPointer<BusInterface> busIf2 = connection_->endpoint2()->getBusInterface();

    QSharedPointer<Component> srcComponent;

    bool lastConnection = false;

    if (busIf1 != 0 && busIf1->getBusType().isValid() && !connection_->endpoint1()->isTypeLocked())
    {
        mode1_ = busIf1->getInterfaceMode();
        portMaps_ = *busIf1->getPortMaps();

        BusInterfaceItem* busIfItem = dynamic_cast<BusInterfaceItem*>(connection_->endpoint1());
        
        if (busIfItem != 0)
        {
            portsCopied_ = busIfItem->hasPortsCopied();
            lastConnection = busIfItem->getConnections().size() == 1;
            srcComponent = busIfItem->getOwnerComponent();
        }
    }

    if (busIf2 != 0 && busIf2->getBusType().isValid() && !connection_->endpoint2()->isTypeLocked())
    {
        mode2_ = busIf2->getInterfaceMode();
        portMaps_ = *busIf2->getPortMaps();

        BusInterfaceItem* busIfItem = dynamic_cast<BusInterfaceItem*>(connection_->endpoint2());

        if (busIfItem != 0)
        {
            portsCopied_ = busIfItem->hasPortsCopied();
            lastConnection = busIfItem->getConnections().size() == 1;
            srcComponent = busIfItem->getOwnerComponent();
        }
    }

    if (portsCopied_ && lastConnection)
    {
        foreach (QSharedPointer<PortMap> portMap, portMaps_)
        {
            QSharedPointer<Port> port = srcComponent->getPort(portMap->getPhysicalPort()->name_);
            DeletePhysicalPortCommand* delCmd = new DeletePhysicalPortCommand(srcComponent, port, this);

            // If the port is visible as ad-hoc in the current design, it must be hidden.
            if (diagram_->getDiagramAdHocPort(port->name()) != 0)
            {                               
                new AdHocVisibilityChangeCommand(diagram_, port->name(), false, delCmd);
            }        
        }
    }

    foreach(Association* association, connection_->getAssociations())
    {
        new AssociationRemoveCommand(association, diagram_, this);
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectionDeleteCommand::~ConnectionDeleteCommand()
//-----------------------------------------------------------------------------
ConnectionDeleteCommand::~ConnectionDeleteCommand()
{
    if (del_)
    {
        delete connection_;
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectionDeleteCommand::undo()
//-----------------------------------------------------------------------------
void ConnectionDeleteCommand::undo()
{
    // Execute child commands.
    QUndoCommand::undo();

    QSharedPointer<GenericEditProvider> editProvider = diagram_->getEditProvider().dynamicCast<GenericEditProvider>();
    editProvider->setState("portsCopied", portsCopied_);

    // Add the item back to the scene.
    diagram_->addItem(connection_);
    
    // Connect the ends and set the interface modes.
    if (connection_->connectEnds())
    {
    
        QSharedPointer<BusInterface> busIf1 = connection_->endpoint1()->getBusInterface();
        QSharedPointer<BusInterface> busIf2 = connection_->endpoint2()->getBusInterface();

        if (busIf1 != 0 && busIf1->getBusType().isValid() && !connection_->endpoint1()->isTypeLocked())
        {
            busIf1->setInterfaceMode(mode1_);
            busIf1->getPortMaps()->clear();
            busIf1->getPortMaps()->append(portMaps_);
            connection_->endpoint1()->updateInterface();
        }

        if (busIf2 != 0 && busIf2->getBusType().isValid() && !connection_->endpoint2()->isTypeLocked())
        {
            busIf2->setInterfaceMode(mode2_);
            busIf2->getPortMaps()->clear();
            busIf2->getPortMaps()->append(portMaps_);
            connection_->endpoint2()->updateInterface();
        }

        diagram_->getDesign()->getInterconnections()->append(connection_->getInterconnection());
        diagram_->getDesign()->addRoute(connection_->getRouteExtension());

    }

    del_ = false;
}

//-----------------------------------------------------------------------------
// Function: ConnectionDeleteCommand::redo()
//-----------------------------------------------------------------------------
void ConnectionDeleteCommand::redo()
{
    QSharedPointer<GenericEditProvider> editProvider = diagram_->getEditProvider().dynamicCast<GenericEditProvider>();
    editProvider->setState("portsCopied", portsCopied_);

    // Disconnect the ends.
    connection_->setSelected(false);

    diagram_->removeItem(connection_);

    connection_->disconnectEnds();

    // Remove the item from the scene.
    diagram_->getDesign()->getInterconnections()->removeOne(connection_->getInterconnection());
    diagram_->getDesign()->removeRoute(connection_->getRouteExtension());

    del_ = true;

    // Execute child commands.
    QUndoCommand::redo();
}
