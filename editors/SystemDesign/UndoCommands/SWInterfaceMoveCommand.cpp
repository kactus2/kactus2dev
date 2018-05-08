//-----------------------------------------------------------------------------
// File: SWInterfaceMoveCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 26.02.2016
//
// Description:
// Undo move command for SW interface.
//-----------------------------------------------------------------------------

#include "SWInterfaceMoveCommand.h"

#include <editors/SystemDesign/SWInterfaceItem.h>

#include <IPXACTmodels/kactusExtensions/InterfaceGraphicsData.h>

//-----------------------------------------------------------------------------
// Function: SWInterfaceMoveCommand::SWInterfaceMoveCommand()
//-----------------------------------------------------------------------------
SWInterfaceMoveCommand::SWInterfaceMoveCommand(SWInterfaceItem* item, QPointF const& oldPos,
    IGraphicsItemStack* oldStack, DesignDiagram* diagram, QUndoCommand* parent):
ItemMoveCommand(item, oldPos, oldStack, diagram, parent),
interfaceItem_(item),
oldPosition_(oldPos),
newPosition_(item->scenePos())
{

}

//-----------------------------------------------------------------------------
// Function: SWInterfaceMoveCommand::SWInterfaceMoveCommand()
//-----------------------------------------------------------------------------
SWInterfaceMoveCommand::SWInterfaceMoveCommand(SWInterfaceItem* item, QPointF const& oldPos,
    IGraphicsItemStack* oldStack, QPointF const& newPos, IGraphicsItemStack* newStack, DesignDiagram* diagram,
    QUndoCommand* parent):
ItemMoveCommand(item, oldPos, oldStack, newPos, newStack, diagram, parent),
interfaceItem_(item),
oldPosition_(oldPos),
newPosition_(newPos)
{

}

//-----------------------------------------------------------------------------
// Function: SWInterfaceMoveCommand::~SWInterfaceMoveCommand()
//-----------------------------------------------------------------------------
SWInterfaceMoveCommand::~SWInterfaceMoveCommand()
{

}

//-----------------------------------------------------------------------------
// Function: SWInterfaceMoveCommand::undo()
//-----------------------------------------------------------------------------
void SWInterfaceMoveCommand::undo()
{
    ItemMoveCommand::undo();

    QSharedPointer<InterfaceGraphicsData> graphicsData = interfaceItem_->getInterfaceGraphicsData();
    graphicsData->setPosition(oldPosition_);
}

//-----------------------------------------------------------------------------
// Function: SWInterfaceMoveCommand::redo()
//-----------------------------------------------------------------------------
void SWInterfaceMoveCommand::redo()
{
    ItemMoveCommand::redo();

    QSharedPointer<InterfaceGraphicsData> graphicsData = interfaceItem_->getInterfaceGraphicsData();
    graphicsData->setPosition(newPosition_);
}
