//-----------------------------------------------------------------------------
// File: ComponentItemMoveCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 17.09.2015
//
// Description:
// Undo command for moving component instances in a design.
//-----------------------------------------------------------------------------

#include "ComponentItemMoveCommand.h"

#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/IGraphicsItemStack.h>

#include <designEditors/common/DesignDiagram.h>

#include <IPXACTmodels/Design/ComponentInstance.h>

//-----------------------------------------------------------------------------
// Function: ComponentItemMoveCommand()
//-----------------------------------------------------------------------------
ComponentItemMoveCommand::ComponentItemMoveCommand(ComponentItem* item, QPointF const& oldPos,
    IGraphicsItemStack* oldStack, DesignDiagram* diagram, QUndoCommand* parent):
QUndoCommand(parent),
item_(item),
oldPos_(oldPos),
oldStack_(oldStack),
newPos_(item->scenePos()),
newStack_(dynamic_cast<IGraphicsItemStack*>(item->parentItem())),
diagram_(diagram)
{

}

//-----------------------------------------------------------------------------
// Function: ~ComponentItemMoveCommand()
//-----------------------------------------------------------------------------
ComponentItemMoveCommand::~ComponentItemMoveCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ComponentItemMoveCommand::undo()
{
    newStack_->removeItem(item_);

    item_->setPos(oldPos_);
    oldStack_->addItem(item_);

    item_->getComponentInstance()->setPosition(oldPos_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ComponentItemMoveCommand::redo()
{
    oldStack_->removeItem(item_);

    item_->setPos(newPos_);
    newStack_->addItem(item_);

     item_->getComponentInstance()->setPosition(newPos_);
    // Execute child commands.
    QUndoCommand::redo();

    resetSceneRectangle();
}

//-----------------------------------------------------------------------------
// Function: ComponentItemMoveCommand::resetSceneRectangle()
//-----------------------------------------------------------------------------
void ComponentItemMoveCommand::resetSceneRectangle()
{
    if (oldStack_ != newStack_)
    {
        diagram_->resetSceneRectangleForItems();
    }
}
