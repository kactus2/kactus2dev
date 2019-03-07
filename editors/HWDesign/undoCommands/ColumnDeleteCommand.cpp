//-----------------------------------------------------------------------------
// File: ColumnDeleteCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 30.09.2015
//
// Description:
// Undo command for removing a column in a design diagram.
//-----------------------------------------------------------------------------

#include "ColumnDeleteCommand.h"
#include "ComponentDeleteCommand.h"
#include "ConnectionDeleteCommand.h"

#include <common/graphicsItems/GraphicsItemTypes.h>
#include <common/graphicsItems/GraphicsColumn.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>

#include <editors/common/DesignDiagram.h>
#include <editors/common/Association/AssociationRemoveCommand.h>

#include <editors/HWDesign/HWComponentItem.h>
#include <editors/HWDesign/HWConnection.h>
#include <editors/HWDesign/HWConnectionEndpoint.h>
#include <editors/HWDesign/HierarchicalPortItem.h>
#include <editors/HWDesign/HWDesignDiagram.h>

#include <editors/HWDesign/undoCommands/TopAdHocVisibilityChangeCommand.h>

#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: ColumnDeleteCommand::ColumnDeleteCommand()
//-----------------------------------------------------------------------------
ColumnDeleteCommand::ColumnDeleteCommand(DesignDiagram* diagram, GraphicsColumnLayout* layout,
    GraphicsColumn* column, QUndoCommand* parent): 
QUndoCommand(parent),
design_(diagram->getDesign()),
layout_(layout),
columnItem_(column),
del_(true)
{
    // Create child commands for removing interconnections.
    QList<GraphicsConnection*> connections;

    foreach (QGraphicsItem* item, column->childItems())
    {
        if (item->type() == HWComponentItem::Type)
        {
            HWComponentItem* comp = static_cast<HWComponentItem*>(item);

            new ComponentDeleteCommand(diagram, column, comp, this);
        }
        else if (item->type() == GFX_TYPE_DIAGRAM_INTERFACE)
        {
            HWConnectionEndpoint* endpoint = static_cast<HWConnectionEndpoint*>(item);

            foreach (GraphicsConnection* connection, endpoint->getConnections())
            {
                if (!connections.contains(connection))
                {
                    new ConnectionDeleteCommand(diagram, static_cast<HWConnection*>(connection), this);
                    connections.append(connection);
                }
            }

            if (endpoint->getOffPageConnector() != 0)
            {
                foreach (GraphicsConnection* connection, endpoint->getOffPageConnector()->getConnections())
                {
                    if (!connections.contains(connection))
                    {
                        new ConnectionDeleteCommand(diagram, static_cast<HWConnection*>(connection), this);
                        connections.append(connections);
                    }
                }
            }
        }
        else if (item->type() == GFX_TYPE_DIAGRAM_ADHOC_INTERFACE)
        {
            HWDesignDiagram* hwDiagram = dynamic_cast<HWDesignDiagram*>(diagram);
            if (hwDiagram)
            {
                HierarchicalPortItem* adHocInterface = dynamic_cast<HierarchicalPortItem*>(item);

                new TopAdHocVisibilityChangeCommand(hwDiagram, adHocInterface->name(), false, this);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ColumnDeleteCommand::~ColumnDeleteCommand()
//-----------------------------------------------------------------------------
ColumnDeleteCommand::~ColumnDeleteCommand()
{
    if (del_)
    {
        delete columnItem_;
    }
}

//-----------------------------------------------------------------------------
// Function: ColumnDeleteCommand::undo()
//-----------------------------------------------------------------------------
void ColumnDeleteCommand::undo()
{
    // Add the item back to the layout.
    design_->addColumn(columnItem_->getColumnDesc());
    layout_->addColumn(columnItem_);
    del_ = false;

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: ColumnDeleteCommand::redo()
//-----------------------------------------------------------------------------
void ColumnDeleteCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    // Remove the item from the layout.
    layout_->removeColumn(columnItem_);
    design_->removeColumn(columnItem_->getColumnDesc());

    del_ = true;
}
