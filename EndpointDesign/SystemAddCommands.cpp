//-----------------------------------------------------------------------------
// File: SystemAddCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Määttä
// Date: 13.10.2011
//
// Description:
// Undo add commands for system designs.
//-----------------------------------------------------------------------------

#include "SystemAddCommands.h"

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
// Function: SystemColumnAddCommand()
//-----------------------------------------------------------------------------
SystemColumnAddCommand::SystemColumnAddCommand(SystemColumnLayout* layout, QString const& name,
                                               QUndoCommand* parent) : QUndoCommand(parent), layout_(layout),
                                                                       name_(name), column_(0), del_(false)
{
}

//-----------------------------------------------------------------------------
// Function: ~SystemColumnAddCommand()
//-----------------------------------------------------------------------------
SystemColumnAddCommand::~SystemColumnAddCommand()
{
    if (del_)
    {
        delete column_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SystemColumnAddCommand::undo()
{
    Q_ASSERT(column_ != 0);

    // Remove the column from the layout.
    layout_->removeColumn(column_);
    del_ = true;
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void SystemColumnAddCommand::redo()
{
    // If this is the first time, create the column.
    if (column_ == 0)
    {
        layout_->addColumn(name_);
        column_ = layout_->getColumns().back();
    }
    else
    {
        // Otherwise add the existing column to the layout.
        layout_->addColumn(column_);
    }

    del_ = false;
}

//-----------------------------------------------------------------------------
// Function: EndpointConnectionAddCommand()
//-----------------------------------------------------------------------------
EndpointConnectionAddCommand::EndpointConnectionAddCommand(QGraphicsScene* scene, EndpointConnection* conn,
                                                           QUndoCommand* parent) : QUndoCommand(parent),
                                                                                   conn_(conn), scene_(scene),
                                                                                   del_(false)

{
}

//-----------------------------------------------------------------------------
// Function: ~EndpointConnectionAddCommand()
//-----------------------------------------------------------------------------
EndpointConnectionAddCommand::~EndpointConnectionAddCommand()
{
    if (del_)
    {
        delete conn_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndpointConnectionAddCommand::undo()
{
    // Remove the interconnection from the scene.
    scene_->removeItem(conn_);

    // Disconnect the ends.
    conn_->disconnectEnds();
    del_ = true;
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndpointConnectionAddCommand::redo()
{
    // Add the back to the scene.
    if (!scene_->items().contains(conn_))
    {
        scene_->addItem(conn_);
    }

    // Connect the ends and set the interface modes and port map for the hierarchical end point.
    conn_->connectEnds();
    del_ = false;
}

//-----------------------------------------------------------------------------
// Function: PlatformCompAddCommand()
//-----------------------------------------------------------------------------
PlatformCompAddCommand::PlatformCompAddCommand(MappingComponentItem* mappingComp, PlatformComponentItem* item,
                                               QUndoCommand* parent) : QUndoCommand(parent), item_(item),
                                                                       mappingComp_(mappingComp), del_(false)
{
}

//-----------------------------------------------------------------------------
// Function: ~PlatformCompAddCommand()
//-----------------------------------------------------------------------------
PlatformCompAddCommand::~PlatformCompAddCommand()
{
    if (del_)
    {
        delete item_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void PlatformCompAddCommand::undo()
{
    Q_ASSERT(mappingComp_ != 0);

    // Remove the platform item from the mapping component and scene.
    mappingComp_->setPlatformComponent(0);
    mappingComp_->scene()->removeItem(item_);
    del_ = true;

    emit componentInstanceRemoved(item_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void PlatformCompAddCommand::redo()
{
    Q_ASSERT(mappingComp_ != 0);

    mappingComp_->setPlatformComponent(item_);
    del_ = false;

    emit componentInstantiated(item_);
}

//-----------------------------------------------------------------------------
// Function: ProgramEntityAddCommand()
//-----------------------------------------------------------------------------
ProgramEntityAddCommand::ProgramEntityAddCommand(MappingComponentItem* mappingComp, ProgramEntityItem* item,
                                                 QUndoCommand* parent) : QUndoCommand(parent), item_(item),
                                                 mappingComp_(mappingComp), del_(false)
{
}

//-----------------------------------------------------------------------------
// Function: ~ProgramEntityAddCommand()
//-----------------------------------------------------------------------------
ProgramEntityAddCommand::~ProgramEntityAddCommand()
{
    if (del_)
    {
        delete item_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ProgramEntityAddCommand::undo()
{
    Q_ASSERT(mappingComp_ != 0);

    // Remove the platform item from the mapping component and scene.
    mappingComp_->removeProgramEntity(item_);
    mappingComp_->scene()->removeItem(item_);
    del_ = true;

    emit componentInstanceRemoved(item_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ProgramEntityAddCommand::redo()
{
    Q_ASSERT(mappingComp_ != 0);

    mappingComp_->addProgramEntity(item_);
    del_ = false;

    emit componentInstantiated(item_);
}

//-----------------------------------------------------------------------------
// Function: ApplicationAddCommand()
//-----------------------------------------------------------------------------
ApplicationAddCommand::ApplicationAddCommand(ProgramEntityItem* progEntity, ApplicationItem* item,
                                               QUndoCommand* parent) : QUndoCommand(parent), item_(item),
                                               progEntity_(progEntity), del_(false)
{
}

//-----------------------------------------------------------------------------
// Function: ~ApplicationAddCommand()
//-----------------------------------------------------------------------------
ApplicationAddCommand::~ApplicationAddCommand()
{
    if (del_)
    {
        delete item_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ApplicationAddCommand::undo()
{
    Q_ASSERT(progEntity_ != 0);

    // Remove the application item from the program entity and scene.
    progEntity_->setApplication(0);
    progEntity_->scene()->removeItem(item_);
    del_ = true;

    emit componentInstanceRemoved(item_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ApplicationAddCommand::redo()
{
    Q_ASSERT(progEntity_ != 0);

    progEntity_->setApplication(item_);
    del_ = false;

    emit componentInstantiated(item_);
}

//-----------------------------------------------------------------------------
// Function: MappingCompAddCommand()
//-----------------------------------------------------------------------------
MappingCompAddCommand::MappingCompAddCommand(SystemColumn* column, MappingComponentItem* item,
                                             QUndoCommand* parent) : QUndoCommand(parent), item_(item),
                                             column_(column), del_(false)
{
}

//-----------------------------------------------------------------------------
// Function: ~MappingCompAddCommand()
//-----------------------------------------------------------------------------
MappingCompAddCommand::~MappingCompAddCommand()
{
    if (del_)
    {
        delete item_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void MappingCompAddCommand::undo()
{
    // Remove the item from the column and the scene.
    column_->removeItem(item_);
    column_->scene()->removeItem(item_);
    del_ = true;

    emit componentInstanceRemoved(item_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void MappingCompAddCommand::redo()
{
    // Add the item to the column.
    column_->addItem(item_);
    del_ = false;

    emit componentInstantiated(item_);
}

//-----------------------------------------------------------------------------
// Function: EndpointAddCommand()
//-----------------------------------------------------------------------------
EndpointAddCommand::EndpointAddCommand(EndpointStack* stack, EndpointItem* item,
                                       QUndoCommand* parent) : QUndoCommand(parent), item_(item),
                                                               stack_(stack), del_(false)
{
}

//-----------------------------------------------------------------------------
// Function: ~EndpointAddCommand()
//-----------------------------------------------------------------------------
EndpointAddCommand::~EndpointAddCommand()
{
    if (del_)
    {
        delete item_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndpointAddCommand::undo()
{
    Q_ASSERT(stack_ != 0);

    // Remove the endpoint from the stack and the scene.
    stack_->removeEndpoint(item_);
    stack_->scene()->removeItem(item_);
    del_ = true;
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndpointAddCommand::redo()
{
    Q_ASSERT(stack_ != 0);

    stack_->addEndpoint(item_);
    del_ = false;
}