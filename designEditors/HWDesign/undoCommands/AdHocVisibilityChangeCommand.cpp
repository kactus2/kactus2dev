//-----------------------------------------------------------------------------
// File: AdHocVisibilityChangeCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.09.2015
//
// Description:
// Undo command for changing port adhoc visibility in design diagram.
//-----------------------------------------------------------------------------

#include "AdHocVisibilityChangeCommand.h"

#include <common/graphicsItems/GraphicsColumn.h>

#include <designEditors/common/DesignDiagram.h>

#include <designEditors/HWDesign/AdHocEnabled.h>
#include <designEditors/HWDesign/HWComponentItem.h>
#include <designEditors/HWDesign/HWConnection.h>
#include <designEditors/HWDesign/HWConnectionEndpoint.h>

#include <designEditors/HWDesign/undoCommands/ConnectionDeleteCommand.h>
#include <designEditors/HWDesign/undoCommands/ComponentDeleteCommand.h>

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityChangeCommand::AdHocVisibilityChangeCommand()
//-----------------------------------------------------------------------------
AdHocVisibilityChangeCommand::AdHocVisibilityChangeCommand(AdHocEnabled* dataSource, QString const& portName,
                                                           bool newVisibility, QUndoCommand* parent)
    : QUndoCommand(parent),
      dataSource_(dataSource),
      portName_(portName),
      pos_(),
      newVisibility_(newVisibility)
{
    if (!newVisibility_)
    {
        // Create child commands for removing interconnections.
        HWConnectionEndpoint* port = dataSource->getDiagramAdHocPort(portName);
        Q_ASSERT(port != 0);

        pos_ = port->scenePos();

        foreach (GraphicsConnection* connection, port->getConnections())
        {
            new ConnectionDeleteCommand(static_cast<DesignDiagram*>(port->scene()), 
                static_cast<HWConnection*>(connection), this);
        }

        foreach (GraphicsConnection* connection, port->getOffPageConnector()->getConnections())
        {
            new ConnectionDeleteCommand(static_cast<DesignDiagram*>(port->scene()), 
                static_cast<HWConnection*>(connection), this);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityChangeCommand::~AdHocVisibilityChangeCommand()
//-----------------------------------------------------------------------------
AdHocVisibilityChangeCommand::~AdHocVisibilityChangeCommand()
{

}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityChangeCommand::undo()
//-----------------------------------------------------------------------------
void AdHocVisibilityChangeCommand::undo()
{
    dataSource_->setPortAdHocVisible(portName_, !newVisibility_);

    if (!newVisibility_)
    {
        HWConnectionEndpoint* port = dataSource_->getDiagramAdHocPort(portName_);
        port->setPos(port->parentItem()->mapFromScene(pos_));

        if (port->type() == GFX_TYPE_DIAGRAM_ADHOC_INTERFACE) 
        {
            GraphicsColumn* column = static_cast<GraphicsColumn*>(port->parentItem());
            column->onMoveItem(port);
        }
        else if (port->type() == GFX_TYPE_DIAGRAM_ADHOC_PORT)
        {
            HWComponentItem* comp = static_cast<HWComponentItem*>(port->parentItem());
            comp->onMovePort(port);
        }
    }

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityChangeCommand::redo()
//-----------------------------------------------------------------------------
void AdHocVisibilityChangeCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    dataSource_->setPortAdHocVisible(portName_, newVisibility_);
}
