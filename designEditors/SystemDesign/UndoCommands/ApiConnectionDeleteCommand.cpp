//-----------------------------------------------------------------------------
// File: ApiConnectionDeleteCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.02.2016
//
// Description:
// Undo delete commands for API connections in system design.
//-----------------------------------------------------------------------------

#include "ApiConnectionDeleteCommand.h"

#include <designEditors/common/Association/Association.h>
#include <designEditors/common/Association/AssociationRemoveCommand.h>
#include <designEditors/SystemDesign/ApiGraphicsConnection.h>

#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: ApiConnectionDeleteCommand::ApiConnectionDeleteCommand()
//-----------------------------------------------------------------------------
ApiConnectionDeleteCommand::ApiConnectionDeleteCommand(ApiGraphicsConnection* connection,
                                                       QSharedPointer<Design> containingDesign,
                                                       QUndoCommand* parent /* = 0 */):
QUndoCommand(parent),
connection_(connection),
scene_(connection_->scene()),
containingDesign_(containingDesign),
deleteConnection_(true)
{
    foreach (Association* association, connection_->getAssociations())
    {
        new AssociationRemoveCommand(association, scene_, this);
    }
}

//-----------------------------------------------------------------------------
// Function: ApiConnectionDeleteCommand::~ApiConnectionDeleteCommand()
//-----------------------------------------------------------------------------
ApiConnectionDeleteCommand::~ApiConnectionDeleteCommand()
{
    if (deleteConnection_)
    {
        delete connection_;
    }
}

//-----------------------------------------------------------------------------
// Function: ApiConnectionDeleteCommand::undo()
//-----------------------------------------------------------------------------
void ApiConnectionDeleteCommand::undo()
{
    QUndoCommand::undo();

    scene_->addItem(connection_);

    if (connection_->connectEnds())
    {
        QSharedPointer<ApiInterconnection> apiConnection = connection_->getApiInterconnection();
        QList<QSharedPointer<ApiInterconnection> > containedApis = containingDesign_->getApiConnections();
        containedApis.append(apiConnection);

        containingDesign_->setApiConnections(containedApis);
        containingDesign_->addRoute(connection_->getRouteExtension());
    }

    scene_->clearSelection();
    deleteConnection_ = false;
}

//-----------------------------------------------------------------------------
// Function: ApiConnectionDeleteCommand::redo()
//-----------------------------------------------------------------------------
void ApiConnectionDeleteCommand::redo()
{
    connection_->setSelected(false);
    scene_->removeItem(connection_);

    connection_->disconnectEnds();

    QSharedPointer<ApiInterconnection> removedConnection = connection_->getApiInterconnection();
    QList<QSharedPointer<ApiInterconnection> > containedApis = containingDesign_->getApiConnections();
    containedApis.removeAll(removedConnection);
    containingDesign_->setApiConnections(containedApis);

    containingDesign_->removeRoute(connection_->getRouteExtension());

    deleteConnection_ = true;

    QUndoCommand::redo();
}
