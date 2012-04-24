//-----------------------------------------------------------------------------
// File: SystemDeleteCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Määttä
// Date: 12.10.2011
//
// Description:
// Undo delete commands for system designs.
//-----------------------------------------------------------------------------

#include "SystemDeleteCommands.h"

#include "../SystemDesign/SystemColumnLayout.h"
#include "../SystemDesign/SystemColumn.h"
#include "MappingComponentItem.h"
#include "ProgramEntityItem.h"
#include "PlatformComponentItem.h"
#include "EndpointConnection.h"
#include "EndpointItem.h"
#include "EndpointStack.h"
#include "ApplicationItem.h"

//-----------------------------------------------------------------------------
// Function: SystemColumnDeleteCommand()
//-----------------------------------------------------------------------------
SystemColumnDeleteCommand::SystemColumnDeleteCommand(SystemColumnLayout* layout, SystemColumn* column,
                                                     QUndoCommand* parent) : QUndoCommand(parent),
                                                                             layout_(layout),
                                                                             column_(column), del_(true)
{
    // Create child commands for removing connections.
    foreach (QGraphicsItem* item, column->childItems())
    {
        if (item->type() == MappingComponentItem::Type)
        {
            MappingComponentItem* comp = static_cast<MappingComponentItem*>(item);

            foreach (QGraphicsItem* childItem, comp->childItems())
            {
                if (childItem->type() != ProgramEntityItem::Type)
                {
                    continue;
                }

                ProgramEntityItem* progEntity = static_cast<ProgramEntityItem*>(childItem);

                foreach (EndpointItem* endpoint, progEntity->getEndpoints())
                {
                    foreach (EndpointConnection* conn, endpoint->getConnections())
                    {
                        QUndoCommand* cmd = new EndpointConnectionDeleteCommand(conn, this);
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
    if (del_)
    {
        delete column_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SystemColumnDeleteCommand::undo()
{
    // Add the item back to the layout.
    layout_->addColumn(column_);
    del_ = false;

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
    layout_->removeColumn(column_);
    del_ = true;
}

//-----------------------------------------------------------------------------
// Function: EndpointConnectionDeleteCommand()
//-----------------------------------------------------------------------------
EndpointConnectionDeleteCommand::EndpointConnectionDeleteCommand(EndpointConnection* conn,
                                                                 QUndoCommand* parent) :
    QUndoCommand(parent), conn_(conn), scene_(conn->scene()), del_(true)
{
}

//-----------------------------------------------------------------------------
// Function: ~EndpointConnectionDeleteCommand()
//-----------------------------------------------------------------------------
EndpointConnectionDeleteCommand::~EndpointConnectionDeleteCommand()
{
    if (del_)
    {
        delete conn_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndpointConnectionDeleteCommand::undo()
{
    // Add the item back to the scene.
    scene_->addItem(conn_);

    // Connect the ends.
    conn_->connectEnds();
    del_ = false;
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndpointConnectionDeleteCommand::redo()
{
    // Remove the item from the scene.
    scene_->removeItem(conn_);

    // Disconnect the ends.
    conn_->disconnectEnds();
    del_ = true;
}

//-----------------------------------------------------------------------------
// Function: EndpointDeleteCommand()
//-----------------------------------------------------------------------------
EndpointDeleteCommand::EndpointDeleteCommand(EndpointItem* endpoint, QUndoCommand* parent) :
    QUndoCommand(parent), endpoint_(endpoint), parent_(static_cast<EndpointStack*>(endpoint->parentItem())),
    scene_(endpoint->scene()), del_(true)
{
    // Create child commands for removing connections.
    foreach (EndpointConnection* conn, endpoint->getConnections())
    {
        QUndoCommand* cmd = new EndpointConnectionDeleteCommand(conn, this);
    }
}

//-----------------------------------------------------------------------------
// Function: ~EndpointDeleteCommand()
//-----------------------------------------------------------------------------
EndpointDeleteCommand::~EndpointDeleteCommand()
{
    if (del_)
    {
        delete endpoint_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndpointDeleteCommand::undo()
{
    // Add the item back to the scene.
    parent_->addEndpoint(endpoint_);
    del_ = false;

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndpointDeleteCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    // Remove the item from the scene.
    parent_->removeEndpoint(endpoint_);
    scene_->removeItem(endpoint_);
    del_ = true;
}

//-----------------------------------------------------------------------------
// Function: PlatformCompDeleteCommand()
//-----------------------------------------------------------------------------
PlatformCompDeleteCommand::PlatformCompDeleteCommand(PlatformComponentItem* item, QUndoCommand* parent) :
    QUndoCommand(parent), item_(item), parent_(static_cast<MappingComponentItem*>(item->parentItem())),
    scene_(item->scene()), del_(true)
{
    Q_ASSERT(parent_ != 0);
}

//-----------------------------------------------------------------------------
// Function: ~PlatformCompDeleteCommand()
//-----------------------------------------------------------------------------
PlatformCompDeleteCommand::~PlatformCompDeleteCommand()
{
    if (del_)
    {
        delete item_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void PlatformCompDeleteCommand::undo()
{
    // Add the item back to the scene.
    parent_->setPlatformComponent(item_);
    del_ = false;

    emit componentInstantiated(item_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void PlatformCompDeleteCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    // Remove the item from the scene.
    parent_->setPlatformComponent(0);
    scene_->removeItem(item_);
    del_ = true;

    emit componentInstanceRemoved(item_);
}

//-----------------------------------------------------------------------------
// Function: ApplicationDeleteCommand()
//-----------------------------------------------------------------------------
ApplicationDeleteCommand::ApplicationDeleteCommand(ApplicationItem* item, QUndoCommand* parent) :
    QUndoCommand(parent), item_(item), parent_(static_cast<ProgramEntityItem*>(item->parentItem())),
    scene_(item->scene()), del_(true)
{
    Q_ASSERT(parent_ != 0);
}

//-----------------------------------------------------------------------------
// Function: ~ApplicationDeleteCommand()
//-----------------------------------------------------------------------------
ApplicationDeleteCommand::~ApplicationDeleteCommand()
{
    if (del_)
    {
        delete item_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ApplicationDeleteCommand::undo()
{
    // Add the item back to the scene.
    parent_->setApplication(item_);
    del_ = false;

    emit componentInstantiated(item_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ApplicationDeleteCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    // Remove the item from the scene.
    parent_->setApplication(0);
    scene_->removeItem(item_);
    del_ = true;

    emit componentInstanceRemoved(item_);
}

//-----------------------------------------------------------------------------
// Function: ProgramEntityDeleteCommand()
//-----------------------------------------------------------------------------
ProgramEntityDeleteCommand::ProgramEntityDeleteCommand(ProgramEntityItem* item, QUndoCommand* parent) :
    QUndoCommand(parent), item_(item), parent_(static_cast<MappingComponentItem*>(item->parentItem())),
    scene_(item->scene()), del_(true)
{
    Q_ASSERT(parent_ != 0);

    // Create child commands for removing the endpoint connections.
    foreach (EndpointItem* endpoint, item->getEndpoints())
    {
        foreach (EndpointConnection* conn, endpoint->getConnections())
        {
            QUndoCommand* cmd = new EndpointConnectionDeleteCommand(conn, this);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ~ProgramEntityDeleteCommand()
//-----------------------------------------------------------------------------
ProgramEntityDeleteCommand::~ProgramEntityDeleteCommand()
{
    if (del_)
    {
        delete item_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ProgramEntityDeleteCommand::undo()
{
    // Add the item back to the scene.
    parent_->addProgramEntity(item_);
    del_ = false;

    emit componentInstantiated(item_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ProgramEntityDeleteCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    // Remove the item from the scene.
    parent_->removeProgramEntity(item_);
    scene_->removeItem(item_);
    del_ = true;

    emit componentInstanceRemoved(item_);
}

//-----------------------------------------------------------------------------
// Function: MappingCompDeleteCommand()
//-----------------------------------------------------------------------------
MappingCompDeleteCommand::MappingCompDeleteCommand(MappingComponentItem* item, QUndoCommand* parent) :
    QUndoCommand(parent), item_(item), column_(static_cast<SystemColumn*>(item->parentItem())),
    scene_(item->scene()), del_(true)
{
    // Create child commands for removing connections.
    foreach (QGraphicsItem* childItem, item->childItems())
    {
        if (childItem->type() != ProgramEntityItem::Type)
        {
            continue;
        }

        ProgramEntityItem* progEntity = static_cast<ProgramEntityItem*>(childItem);

        foreach (EndpointItem* endpoint, progEntity->getEndpoints())
        {
            foreach (EndpointConnection* conn, endpoint->getConnections())
            {
                QUndoCommand* cmd = new EndpointConnectionDeleteCommand(conn, this);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ~MappingCompDeleteCommand()
//-----------------------------------------------------------------------------
MappingCompDeleteCommand::~MappingCompDeleteCommand()
{
    if (del_)
    {
        delete item_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void MappingCompDeleteCommand::undo()
{
    // Add the item back to the scene.
    column_->addItem(item_);
    del_ = false;

    emit componentInstantiated(item_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void MappingCompDeleteCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    // Remove the item from the scene.
    column_->removeItem(item_);
    scene_->removeItem(item_);
    del_ = true;

    emit componentInstanceRemoved(item_);
}