//-----------------------------------------------------------------------------
// File: AdHocTieOffConnectionDeleteCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 20.10.2017
//
// Description:
// Undo command for deleting ad hoc tie off connection.
//-----------------------------------------------------------------------------

#include "AdHocTieOffConnectionDeleteCommand.h"

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/AdHocConnection.h>

//-----------------------------------------------------------------------------
// Function: AdHocTieOffConnectionDeleteCommand::AdHocTieOffConnectionDeleteCommand()
//-----------------------------------------------------------------------------
AdHocTieOffConnectionDeleteCommand::AdHocTieOffConnectionDeleteCommand(QSharedPointer<Design> containingDesign,
    QSharedPointer<AdHocConnection> connection, QUndoCommand* parent):
QUndoCommand(parent),
containingDesign_(containingDesign),
tieOffConnection_(connection)
{

}

//-----------------------------------------------------------------------------
// Function: AdHocTieOffConnectionDeleteCommand::~AdHocTieOffConnectionDeleteCommand()
//-----------------------------------------------------------------------------
AdHocTieOffConnectionDeleteCommand::~AdHocTieOffConnectionDeleteCommand()
{

}

//-----------------------------------------------------------------------------
// Function: AdHocTieOffConnectionDeleteCommand::undo()
//-----------------------------------------------------------------------------
void AdHocTieOffConnectionDeleteCommand::undo()
{
    if (containingDesign_ && tieOffConnection_ &&
        !containingDesign_->getAdHocConnections()->contains(tieOffConnection_))
    {
        containingDesign_->getAdHocConnections()->append(tieOffConnection_);
    }

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: AdHocTieOffConnectionDeleteCommand::redo()
//-----------------------------------------------------------------------------
void AdHocTieOffConnectionDeleteCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    if (containingDesign_ && tieOffConnection_ &&
        containingDesign_->getAdHocConnections()->contains(tieOffConnection_))
    {
        containingDesign_->getAdHocConnections()->removeAll(tieOffConnection_);
    }
}
