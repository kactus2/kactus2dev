//-----------------------------------------------------------------------------
// File: SWInterfaceAddCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 26.02.2016
//
// Description:
// Undo add command for SW interfaces.
//-----------------------------------------------------------------------------

#include "SWInterfaceAddCommand.h"

#include <designEditors/SystemDesign/SWInterfaceItem.h>

#include <common/graphicsItems/IGraphicsItemStack.h>

#include <IPXACTmodels/kactusExtensions/InterfaceGraphicsData.h>

#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: SWInterfaceAddCommand::SWInterfaceAddCommand()
//-----------------------------------------------------------------------------
SWInterfaceAddCommand::SWInterfaceAddCommand(IGraphicsItemStack* stack, SWInterfaceItem* item,
                                             QSharedPointer<Design> containingDesign, QUndoCommand* parent):
QUndoCommand(parent),
interfaceItem_(item),
apiInterface_(item->getApiInterface()),
comInterface_(item->getComInterface()),
stack_(stack),
del_(false),
containingDesign_(containingDesign)
{

}

//-----------------------------------------------------------------------------
// Function: SWInterfaceAddCommand::~SWInterfaceAddCommand()
//-----------------------------------------------------------------------------
SWInterfaceAddCommand::~SWInterfaceAddCommand()
{
    if (del_)
    {
        delete interfaceItem_;
    }
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceAddCommand::undo()
//-----------------------------------------------------------------------------
void SWInterfaceAddCommand::undo()
{
    QUndoCommand::undo();

    // Undefine the interface.
    if (apiInterface_ != 0 || comInterface_ != 0)
    {
        interfaceItem_->undefine();
    }

    // Remove the item from the column and the scene.
    stack_->removeItem(interfaceItem_);
    interfaceItem_->scene()->removeItem(interfaceItem_);
    del_ = true;

    QSharedPointer<InterfaceGraphicsData> graphicsData = interfaceItem_->getInterfaceGraphicsData();
    containingDesign_->getVendorExtensions()->removeAll(graphicsData);
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceAddCommand::redo()
//-----------------------------------------------------------------------------
void SWInterfaceAddCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    // Add the item to the column.
    stack_->addItem(interfaceItem_);
    del_ = false;

    QSharedPointer<InterfaceGraphicsData> graphicsData = interfaceItem_->getInterfaceGraphicsData();
    containingDesign_->getVendorExtensions()->append(graphicsData);

    // Define the interface.
    if (apiInterface_ != 0)
    {
        interfaceItem_->define(apiInterface_);
    }
    else if (comInterface_ != 0)
    {
        interfaceItem_->define(comInterface_);
    }
}
