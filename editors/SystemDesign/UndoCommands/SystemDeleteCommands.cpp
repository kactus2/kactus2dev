//-----------------------------------------------------------------------------
// File: SystemDeleteCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 12.10.2011
//
// Description:
// Undo delete commands for system designs.
//-----------------------------------------------------------------------------

#include "SystemDeleteCommands.h"

#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/graphicsItems/ComponentItem.h>

#include <editors/common/DesignDiagram.h>
#include <editors/SystemDesign/SystemColumn.h>
#include <editors/SystemDesign/SystemComponentItem.h>
#include <editors/SystemDesign/SWPortItem.h>
#include <editors/SystemDesign/SWInterfaceItem.h>
#include <editors/SystemDesign/SWComponentItem.h>
#include <editors/SystemDesign/ComGraphicsConnection.h>
#include <editors/SystemDesign/UndoCommands/ComConnectionDeleteCommand.h>
#include <editors/SystemDesign/ApiGraphicsConnection.h>
#include <editors/SystemDesign/UndoCommands/ApiConnectionDeleteCommand.h>
#include <editors/SystemDesign/UndoCommands/SystemComponentDeleteCommand.h>

#include <editors/common/Association/Association.h>
#include <editors/common/Association/AssociationRemoveCommand.h>

#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: SystemColumnDeleteCommand()
//-----------------------------------------------------------------------------
SystemColumnDeleteCommand::SystemColumnDeleteCommand(DesignDiagram* diagram, SystemColumn* column,
    QUndoCommand* parent):
QUndoCommand(parent),
layout_(diagram->getLayout().data()),
graphicalColumn_(column),
canDelete_(true),
containingDesign_(diagram->getDesign()),
removedColumn_(getDeletedColumn())
{
    // Create child commands for removing connections.
    QList<GraphicsConnection*> connections;

    foreach (QGraphicsItem* item, column->childItems())
    {
        if (item->type() == SWComponentItem::Type)
        {
            SWComponentItem* systemComponentItem = static_cast<SWComponentItem*>(item);

            foreach (QGraphicsItem* childItem, systemComponentItem->childItems())
            {
                if (childItem->type() != SWPortItem::Type)
                {
                    continue;
                }

                SWPortItem* port = static_cast<SWPortItem*>(childItem);

                foreach (GraphicsConnection* graphicalConnection, port->getConnections())
                {
                    if (!connections.contains(graphicalConnection))
                    {
                        addConnectionDeleteCommand(graphicalConnection);
                        connections.append(graphicalConnection);
                    }
                }

                if (port->getOffPageConnector() != 0)
                {
                    foreach (GraphicsConnection* offPageConnection, port->getOffPageConnector()->getConnections())
                    {
                        if (!connections.contains(offPageConnection))
                        {
                            addConnectionDeleteCommand(offPageConnection);
                            connections.append(offPageConnection);
                        }
                    }
                }
            }
            foreach(Association* association, systemComponentItem->getAssociations())
            {
                new AssociationRemoveCommand(association, association->scene(), this);
            }

            new SystemComponentDeleteCommand(systemComponentItem, containingDesign_, this);

        }
        else if (item->type() == SWInterfaceItem::Type)
        {
            SWInterfaceItem* interface = static_cast<SWInterfaceItem*>(item);

            foreach (GraphicsConnection* conn, interface->getConnections())
            {
                if (!connections.contains(conn))
                {
                    addConnectionDeleteCommand(conn);
                    connections.append(conn);
                }
            }

            if (interface->getOffPageConnector() != 0)
            {
                foreach (GraphicsConnection* conn, interface->getOffPageConnector()->getConnections())
                {
                    if (!connections.contains(conn))
                    {
                        addConnectionDeleteCommand(conn);
                        connections.append(conn);
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ~SystemColumnDeleteCommand()
//-----------------------------------------------------------------------------
SystemColumnDeleteCommand::~SystemColumnDeleteCommand()
{
    if (canDelete_)
    {
        delete graphicalColumn_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SystemColumnDeleteCommand::undo()
{
    // Add the item back to the layout.
    layout_->addColumn(graphicalColumn_);
    canDelete_ = false;

    containingDesign_->addColumn(removedColumn_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void SystemColumnDeleteCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    // Remove the item from the layout.
    layout_->removeColumn(graphicalColumn_);

    if (removedColumn_)
    {
        containingDesign_->removeColumn(removedColumn_);
    }

    canDelete_ = true;
}

//-----------------------------------------------------------------------------
// Function: SystemDeleteCommands::addConnectionDeleteCommand()
//-----------------------------------------------------------------------------
void SystemColumnDeleteCommand::addConnectionDeleteCommand(GraphicsConnection* connection)
{
    ApiGraphicsConnection* apiConnection = dynamic_cast<ApiGraphicsConnection*>(connection);
    if (apiConnection)
    {
        new ApiConnectionDeleteCommand(apiConnection, containingDesign_, this);
    }
    else
    {
        ComGraphicsConnection* comConnection = dynamic_cast<ComGraphicsConnection*>(connection);
        if (comConnection)
        {
            new ComConnectionDeleteCommand(comConnection, containingDesign_, this);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemDeleteCommands::getDeletedColumn()
//-----------------------------------------------------------------------------
QSharedPointer<ColumnDesc> SystemColumnDeleteCommand::getDeletedColumn()
{
    foreach (QSharedPointer<ColumnDesc> column, containingDesign_->getColumns())
    {
        if (column->name() == graphicalColumn_->name())
        {
            return column;
        }
    }

    return QSharedPointer<ColumnDesc>();
}
