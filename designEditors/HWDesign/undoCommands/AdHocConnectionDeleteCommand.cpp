//-----------------------------------------------------------------------------
// File: AdHocConnectionDeleteCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.02.2016
//
// Description:
// Undo command for removing a AdHocConnection in design diagram.
//-----------------------------------------------------------------------------

#include "AdHocConnectionDeleteCommand.h"

#include <common/GenericEditProvider.h>

#include <designEditors/common/Association/Association.h>
#include <designEditors/common/Association/AssociationRemoveCommand.h>

#include <designEditors/common/DesignDiagram.h>

#include <designEditors/HWDesign/AdHocConnectionItem.h>

#include <IPXACTmodels/Design/AdHocConnection.h>
#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: AdHocConnectionDeleteCommand::AdHocConnectionDeleteCommand()
//-----------------------------------------------------------------------------
AdHocConnectionDeleteCommand::AdHocConnectionDeleteCommand(DesignDiagram* diagram, 
    AdHocConnectionItem* adHocConnection, QUndoCommand* parent) :
QUndoCommand(parent),
adHocConnection_(adHocConnection),
diagram_(diagram),
del_(true)
{
    foreach(Association* association, adHocConnection_->getAssociations())
    {
        new AssociationRemoveCommand(association, diagram_, this);
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionDeleteCommand::~AdHocConnectionDeleteCommand()
//-----------------------------------------------------------------------------
AdHocConnectionDeleteCommand::~AdHocConnectionDeleteCommand()
{
    if (del_)
    {
        delete adHocConnection_;
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionDeleteCommand::undo()
//-----------------------------------------------------------------------------
void AdHocConnectionDeleteCommand::undo()
{
    // Execute child commands.
    QUndoCommand::undo();

    // Add the item back to the scene.
    diagram_->addItem(adHocConnection_);
    diagram_->clearSelection();

    diagram_->getDesign()->getAdHocConnections()->append(adHocConnection_->getAdHocConnection());
    diagram_->getDesign()->addRoute(adHocConnection_->getRouteExtension());

    adHocConnection_->connectEnds();
    adHocConnection_->setVisible(true);

    del_ = false;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionDeleteCommand::redo()
//-----------------------------------------------------------------------------
void AdHocConnectionDeleteCommand::redo()
{
    // Disconnect the ends.
    adHocConnection_->setSelected(false);
    adHocConnection_->disconnectEnds();

    diagram_->getDesign()->getAdHocConnections()->removeOne(adHocConnection_->getAdHocConnection());
    diagram_->getDesign()->removeRoute(adHocConnection_->getRouteExtension());

    // Remove the item from the scene.
    diagram_->removeItem(adHocConnection_);
    del_ = true;

    // Execute child commands.
    QUndoCommand::redo();
}
