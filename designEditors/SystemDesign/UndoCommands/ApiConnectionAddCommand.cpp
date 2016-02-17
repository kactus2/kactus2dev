//-----------------------------------------------------------------------------
// File: ApiConnectionAddCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.02.2016
//
// Description:
// Undo add commands for API connections.
//-----------------------------------------------------------------------------

#include "ApiConnectionAddCommand.h"

#include <designEditors/SystemDesign/ApiGraphicsConnection.h>
#include <designEditors/SystemDesign/SystemDesignDiagram.h>

#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: ApiConnectionAddCommand::ApiConnectionAddCommand()
//-----------------------------------------------------------------------------
ApiConnectionAddCommand::ApiConnectionAddCommand(QGraphicsScene* scene, ApiGraphicsConnection* connection,
                                                 QSharedPointer<Design> containingDesign, QUndoCommand* parent):
QUndoCommand(parent),
connection_(connection),
scene_(scene),
del_(false),
containingDesign_(containingDesign)
{

}

//-----------------------------------------------------------------------------
// Function: ApiConnectionAddCommand::~ApiConnectionAddCommand()
//-----------------------------------------------------------------------------
ApiConnectionAddCommand::~ApiConnectionAddCommand()
{
    if (del_)
    {
        delete connection_;
    }
}

//-----------------------------------------------------------------------------
// Function: ApiConnectionAddCommand::undo()
//-----------------------------------------------------------------------------
void ApiConnectionAddCommand::undo()
{
    scene_->removeItem(connection_);
    connection_->setSelected(false);

    connection_->disconnectEnds();

    QList<QSharedPointer<ApiInterconnection> > apiInterconnections = containingDesign_->getApiConnections();
    apiInterconnections.removeAll(connection_->getApiInterconnection());
    containingDesign_->setApiConnections(apiInterconnections);

    containingDesign_->removeRoute(connection_->getRouteExtension());

    del_ = true;
}

//-----------------------------------------------------------------------------
// Function: ApiConnectionAddCommand::redo()
//-----------------------------------------------------------------------------
void ApiConnectionAddCommand::redo()
{
    if (!scene_->items().contains(connection_))
    {
        scene_->addItem(connection_);
    }

    if (connection_->connectEnds())
    {
        QList<QSharedPointer<ApiInterconnection> > apiInterconnections = containingDesign_->getApiConnections();
        apiInterconnections.append(connection_->getApiInterconnection());
        containingDesign_->setApiConnections(apiInterconnections);
    }

    del_ = false;
}
