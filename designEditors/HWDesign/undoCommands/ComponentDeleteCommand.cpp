//-----------------------------------------------------------------------------
// File: ComponentDeleteCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.09.2015
//
// Description:
// Undo command for removing a component item in a design diagram.
//-----------------------------------------------------------------------------

#include "ComponentDeleteCommand.h"

#include "ConnectionDeleteCommand.h"

#include <common/graphicsItems/GraphicsColumn.h>
#include <common/graphicsItems/GraphicsConnection.h>

#include <designEditors/common/DesignDiagram.h>

#include <designEditors/HWDesign/HWComponentItem.h>
#include <designEditors/HWDesign/HWConnection.h>
#include <designEditors/HWDesign/HWConnectionEndpoint.h>
#include <designEditors/HWDesign/AdHocConnectionItem.h>
#include <designEditors/HWDesign/undoCommands/AdHocConnectionDeleteCommand.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>

#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: ComponentDeleteCommand::ComponentDeleteCommand()
//-----------------------------------------------------------------------------
ComponentDeleteCommand::ComponentDeleteCommand(DesignDiagram* diagram, GraphicsColumn* column,
    HWComponentItem* component, QUndoCommand* parent):
QUndoCommand(parent),
componentItem_(component),
column_(column),
diagram_(diagram),
del_(true)
{
    // Create child commands for removing interconnections.
    QList<GraphicsConnection*> connections;

    foreach (QGraphicsItem *item, componentItem_->childItems())
    {
        HWConnectionEndpoint* endpoint = dynamic_cast<HWConnectionEndpoint*>(item);
        if (endpoint)
        {
            QList<GraphicsConnection*> portConnections = endpoint->getConnections();
            if (endpoint->getOffPageConnector() != 0)
            {
                portConnections.append(endpoint->getOffPageConnector()->getConnections());
            }

            foreach (GraphicsConnection* connection, portConnections)
            {
                if (!connections.contains(connection))
                {
                    HWConnection* hwConnection = dynamic_cast<HWConnection*>(connection);
                    if (hwConnection)
                    {
                        new ConnectionDeleteCommand(diagram_, hwConnection, this);
                    }
                    else
                    {
                        AdHocConnectionItem* adHocConnection = dynamic_cast<AdHocConnectionItem*>(connection);
                        if (adHocConnection)
                        {
                            new AdHocConnectionDeleteCommand(diagram_, adHocConnection, this);
                        }
                    }

                    connections.append(connection);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDeleteCommand::~ComponentDeleteCommand()
//-----------------------------------------------------------------------------
ComponentDeleteCommand::~ComponentDeleteCommand()
{
    if (del_)
    {
        delete componentItem_;
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDeleteCommand::undo()
//-----------------------------------------------------------------------------
void ComponentDeleteCommand::undo()
{
    // Add the item back to the scene.
    diagram_->getDesign()->getComponentInstances()->append(componentItem_->getComponentInstance());
    column_->addItem(componentItem_);    
    del_ = false;

	emit componentInstantiated(componentItem_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: ComponentDeleteCommand::redo()
//-----------------------------------------------------------------------------
void ComponentDeleteCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    // Remove the item from the scene.
    column_->removeItem(componentItem_);
    diagram_->removeItem(componentItem_);
    diagram_->getDesign()->getComponentInstances()->removeOne(componentItem_->getComponentInstance());
    del_ = true;

	emit componentInstanceRemoved(componentItem_);
}
