//-----------------------------------------------------------------------------
// File: SWInterfaceDeleteCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.02.2016
//
// Description:
// Undo delete command for SW interface in system design.
//-----------------------------------------------------------------------------

#include "SWInterfaceDeleteCommand.h"

#include "SWInterfaceItem.h"

#include <common/graphicsItems/GraphicsColumnLayout.h>

#include <designEditors/SystemDesign/ComGraphicsConnection.h>
#include <designEditors/SystemDesign/ApiGraphicsConnection.h>
#include <designEditors/SystemDesign/ComConnectionDeleteCommand.h>
#include <designEditors/SystemDesign/ApiConnectionDeleteCommand.h>

//-----------------------------------------------------------------------------
// Function: SWInterfaceDeleteCommand::SWInterfaceDeleteCommand()
//-----------------------------------------------------------------------------
SWInterfaceDeleteCommand::SWInterfaceDeleteCommand(SWInterfaceItem* interface,
                                                   QSharedPointer<Design> containingDesign, QUndoCommand* parent):
QUndoCommand(parent),
interface_(interface),
apiInterface_(interface->getApiInterface()),
comInterface_(interface->getComInterface()),
parent_(dynamic_cast<IGraphicsItemStack*>(interface->parentItem())),
scene_(interface->scene()),
del_(true)
{
    if (apiInterface_)
    {
        foreach (GraphicsConnection* connection, interface_->getConnections())
        {
            ApiGraphicsConnection* apiConnection = dynamic_cast<ApiGraphicsConnection*>(connection);

            new ApiConnectionDeleteCommand(apiConnection, containingDesign, this);
        }
    }
    else if (comInterface_)
    {
        foreach (GraphicsConnection* connection, interface_->getConnections())
        {
            ComGraphicsConnection* comConnection = dynamic_cast<ComGraphicsConnection*>(connection);
            new ComConnectionDeleteCommand(comConnection, containingDesign, this);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ~SWInterfaceDeleteCommand()
//-----------------------------------------------------------------------------
SWInterfaceDeleteCommand::~SWInterfaceDeleteCommand()
{
    if (del_)
    {
        delete interface_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SWInterfaceDeleteCommand::undo()
{
    // Add the interface back to the scene.
    parent_->addItem(interface_);
    del_ = false;

    // Define the interface.
    if (apiInterface_ != 0)
    {
        interface_->define(apiInterface_);
    }
    else if (comInterface_ != 0)
    {
        interface_->define(comInterface_);
    }

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void SWInterfaceDeleteCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    // Undefine the interface.
    if (apiInterface_ != 0 || comInterface_ != 0)
    {
        interface_->undefine();
    }

    // Remove the interface from the scene.
    parent_->removeItem(interface_);
    scene_->removeItem(interface_);
    del_ = true;
}