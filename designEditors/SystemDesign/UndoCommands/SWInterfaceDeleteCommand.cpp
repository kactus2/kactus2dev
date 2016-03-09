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

#include <common/graphicsItems/GraphicsColumnLayout.h>

#include <designEditors/SystemDesign/UndoCommands/ApiConnectionDeleteCommand.h>
#include <designEditors/SystemDesign/UndoCommands/ComConnectionDeleteCommand.h>

#include <designEditors/SystemDesign/ApiGraphicsConnection.h>
#include <designEditors/SystemDesign/ComGraphicsConnection.h>
#include <designEditors/SystemDesign/SWInterfaceItem.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/kactusExtensions/InterfaceGraphicsData.h>

//-----------------------------------------------------------------------------
// Function: SWInterfaceDeleteCommand::SWInterfaceDeleteCommand()
//-----------------------------------------------------------------------------
SWInterfaceDeleteCommand::SWInterfaceDeleteCommand(SWInterfaceItem* interface,
                                                   QSharedPointer<Design> containingDesign,
                                                   QSharedPointer<Component> component, QUndoCommand* parent):
QUndoCommand(parent),
interfaceItem_(interface),
apiInterface_(interface->getApiInterface()),
comInterface_(interface->getComInterface()),
parent_(dynamic_cast<IGraphicsItemStack*>(interface->parentItem())),
scene_(interface->scene()),
del_(true),
containingDesign_(containingDesign),
containingComponent_(component)
{
    if (apiInterface_)
    {
        foreach (GraphicsConnection* connection, interfaceItem_->getConnections())
        {
            ApiGraphicsConnection* apiConnection = dynamic_cast<ApiGraphicsConnection*>(connection);

            new ApiConnectionDeleteCommand(apiConnection, containingDesign, this);
        }
    }
    else if (comInterface_)
    {
        foreach (GraphicsConnection* connection, interfaceItem_->getConnections())
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
        delete interfaceItem_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SWInterfaceDeleteCommand::undo()
{
    // Add the interface back to the scene.
    parent_->addItem(interfaceItem_);
    del_ = false;

    QSharedPointer<InterfaceGraphicsData> graphicsData = interfaceItem_->getInterfaceGraphicsData();
    containingDesign_->getVendorExtensions()->append(graphicsData);

    // Define the interface.
    if (apiInterface_ != 0)
    {
        interfaceItem_->define(apiInterface_);

        QList<QSharedPointer<ApiInterface> > interfaces = containingComponent_->getApiInterfaces();
        interfaces.append(apiInterface_);
        containingComponent_->setApiInterfaces(interfaces);
    }
    else if (comInterface_ != 0)
    {
        interfaceItem_->define(comInterface_);

        QList<QSharedPointer<ComInterface> > interfaces = containingComponent_->getComInterfaces();
        interfaces.append(comInterface_);
        containingComponent_->setComInterfaces(interfaces);
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
        interfaceItem_->undefine();
    }

    if (apiInterface_)
    {
        QList<QSharedPointer<ApiInterface> > interfaces = containingComponent_->getApiInterfaces();
        interfaces.removeAll(apiInterface_);
        containingComponent_->setApiInterfaces(interfaces);
    }
    else if (comInterface_)
    {
        QList<QSharedPointer<ComInterface> > interfaces = containingComponent_->getComInterfaces();
        interfaces.removeAll(comInterface_);
        containingComponent_->setComInterfaces(interfaces);
    }

    QSharedPointer<InterfaceGraphicsData> graphicsData = interfaceItem_->getInterfaceGraphicsData();
    containingDesign_->getVendorExtensions()->removeAll(graphicsData);

    // Remove the interface from the scene.
    parent_->removeItem(interfaceItem_);
    scene_->removeItem(interfaceItem_);
    del_ = true;
}
