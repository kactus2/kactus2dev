//-----------------------------------------------------------------------------
// File: SWPortDeleteCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.02.2016
//
// Description:
// Undo delete command for SW port item in system design.
//-----------------------------------------------------------------------------

#include "SWPortDeleteCommand.h"

#include <designEditors/SystemDesign/UndoCommands/ComConnectionDeleteCommand.h>
#include <designEditors/SystemDesign/UndoCommands/ApiConnectionDeleteCommand.h>

#include <designEditors/SystemDesign/ApiGraphicsConnection.h>
#include <designEditors/SystemDesign/ComGraphicsConnection.h>
#include <designEditors/SystemDesign/SystemComponentItem.h>
#include <designEditors/SystemDesign/SWPortItem.h>

#include <IPXACTmodels/Design/ComponentInstance.h>

//-----------------------------------------------------------------------------
// Function: SWPortDeleteCommand::SWPortDeleteCommand()
//-----------------------------------------------------------------------------
SWPortDeleteCommand::SWPortDeleteCommand(SWPortItem* port, QSharedPointer<Design> containingDesign,
                                         QUndoCommand* parent):
QUndoCommand(parent),
port_(port),
parent_(static_cast<SystemComponentItem*>(port->parentItem())),
scene_(port->scene()),
del_(true)
{
    QList<GraphicsConnection*> connections = port_->getConnections();
    connections.append(port_->getOffPageConnector()->getConnections());

    if (!connections.isEmpty())
    {
        if (port_->getApiInterface())
        {
            foreach (GraphicsConnection* connection, connections)
            {
                ApiGraphicsConnection* apiConnection = dynamic_cast<ApiGraphicsConnection*>(connection);
                new ApiConnectionDeleteCommand(apiConnection, containingDesign, this);
            }
        }
        else if (port_->getComInterface())
        {
            foreach (GraphicsConnection* connection, connections)
            {
                ComGraphicsConnection* comConnection = dynamic_cast<ComGraphicsConnection*>(connection);
                new ComConnectionDeleteCommand(comConnection, containingDesign, this);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SWPortDeleteCommand::~SWPortDeleteCommand()
//-----------------------------------------------------------------------------
SWPortDeleteCommand::~SWPortDeleteCommand()
{
    if (del_)
    {
        delete port_;
    }
}

//-----------------------------------------------------------------------------
// Function: SWPortDeleteCommand::undo()
//-----------------------------------------------------------------------------
void SWPortDeleteCommand::undo()
{
    // Add the endpoint back to the scene.
    scene_->addItem(port_);

    // Add the endpoint to the parent component.
    parent_->addPort(port_);
    del_ = false;

    QSharedPointer<ComponentInstance> containingInstance = parent_->getComponentInstance();
    containingInstance->updateApiInterfacePosition(port_->name(), port_->pos());

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: SWPortDeleteCommand::redo()
//-----------------------------------------------------------------------------
void SWPortDeleteCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    // Remove the endpoint from the parent component.
    parent_->removePort(port_);

    QSharedPointer<ComponentInstance> containingInstance = parent_->getComponentInstance();
    QMap<QString, QPointF> apiPositions = containingInstance->getApiInterfacePositions();
    QMap<QString, QPointF> comPositions = containingInstance->getComInterfacePositions();

    if (apiPositions.contains(port_->name()))
    {
        containingInstance->removeApiInterfacePosition(port_->name());
    }
    if (comPositions.contains(port_->name()))
    {
        containingInstance->removeComInterfacePosition(port_->name());
    }

    // Remove the endpoint from the scene.
    scene_->removeItem(port_);
    del_ = true;
}
