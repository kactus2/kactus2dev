//-----------------------------------------------------------------------------
// File: ConnectionDeleteCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
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

#include <editors/common/Association/Association.h>
#include <editors/common/Association/AssociationRemoveCommand.h>

#include <editors/common/DesignDiagram.h>

#include <editors/HWDesign/HWConnection.h>

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
      diagram_(diagram),
      del_(true)
{
    for (Association* association : connection_->getAssociations())
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

    // Add the item back to the scene.
    diagram_->addItem(connection_);
    
    if (connection_->connectEnds())
    {    
        connection_->endpoint1()->updateInterface();
        connection_->endpoint2()->updateInterface();

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
