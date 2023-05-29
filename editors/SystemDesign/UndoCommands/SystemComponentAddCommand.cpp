//-----------------------------------------------------------------------------
// File: SystemComponentAddCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 22.02.2016
//
// Description:
// Undo add command for component items in system designs.
//-----------------------------------------------------------------------------

#include "SystemComponentAddCommand.h"

#include <common/graphicsItems/ComponentItem.h>

#include <editors/common/DesignDiagram.h>
#include <editors/SystemDesign/HWMappingItem.h>
#include <editors/SystemDesign/SWComponentItem.h>
#include <editors/SystemDesign/SystemComponentItem.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/ComponentInstance.h>

//-----------------------------------------------------------------------------
// Function: SystemComponentAddCommand::SystemComponentAddCommand()
//-----------------------------------------------------------------------------
SystemComponentAddCommand::SystemComponentAddCommand(IGraphicsItemStack* stack, QGraphicsItem* item,
    DesignDiagram* diagram, QUndoCommand* parent):
QUndoCommand(parent),
item_(item),
stack_(stack),
del_(false),
newHwComponentId_(""),
containingDesign_(diagram->getDesign()),
diagram_(diagram)
{
    if (stack_)
    {
        HWMappingItem* hwItem = dynamic_cast<HWMappingItem*>(stack_);
        if (hwItem && hwItem->getComponentInstance())
        {
            newHwComponentId_ = hwItem->getComponentInstance()->getUuid();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemComponentAddCommand::~SystemComponentAddCommand()
//-----------------------------------------------------------------------------
SystemComponentAddCommand::~SystemComponentAddCommand()
{
    if (del_)
    {
        delete item_;
    }
}

//-----------------------------------------------------------------------------
// Function: SystemComponentAddCommand::undo()
//-----------------------------------------------------------------------------
void SystemComponentAddCommand::undo()
{
    stack_->removeItem(item_);
    item_->scene()->removeItem(item_);
    del_ = true;

    if (dynamic_cast<ComponentItem*>(item_) != 0)
    {
        emit componentInstanceRemoved(static_cast<ComponentItem*>(item_));
    }

    SWComponentItem* swItem = dynamic_cast<SWComponentItem*>(item_);
    if (swItem)
    {
        QSharedPointer<ComponentInstance> swInstance = swItem->getComponentInstance();
        if (swInstance)
        {
            containingDesign_->getComponentInstances()->removeAll(swInstance);

            if (!newHwComponentId_.empty())
            {
                swInstance->setMapping("");
            }
        }
    }

    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: SystemComponentAddCommand::redo()
//-----------------------------------------------------------------------------
void SystemComponentAddCommand::redo()
{
    // Add the item to the column.
    stack_->addItem(item_);
    del_ = false;

    SWComponentItem* swItem = dynamic_cast<SWComponentItem*>(item_);
    if (swItem)
    {
        QSharedPointer<ComponentInstance> swInstance = swItem->getComponentInstance();
        if (swInstance)
        {
            containingDesign_->getComponentInstances()->append(swInstance);

            if (!newHwComponentId_.empty())
            {
                swInstance->setMapping(newHwComponentId_);
            }
        }
    }
    else
    {
        HWMappingItem* mappingItem = dynamic_cast<HWMappingItem*>(item_);
        if (mappingItem && mappingItem->getComponentInstance())
        {
            containingDesign_->getComponentInstances()->append(mappingItem->getComponentInstance());
        }
    }

    if (dynamic_cast<ComponentItem*>(item_) != 0)
    {
        emit componentInstantiated(static_cast<ComponentItem*>(item_));
    }

    // Child commands need not be executed because the other items change their position
    // in a deterministic way.
    //QUndoCommand::redo();

    diagram_->resetSceneRectangleForItems();
}
