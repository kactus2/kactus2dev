//-----------------------------------------------------------------------------
// File: SystemComponentAddCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 22.02.2016
//
// Description:
// Undo add command for component items in system designs.
//-----------------------------------------------------------------------------

#include "SystemComponentAddCommand.h"

#include <common/graphicsItems/ComponentItem.h>

#include <designEditors/SystemDesign/HWMappingItem.h>
#include <designEditors/SystemDesign/SWComponentItem.h>
#include <designEditors/SystemDesign/SystemComponentItem.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/ComponentInstance.h>

#include <IPXACTmodels/kactusExtensions/SWInstance.h>

//-----------------------------------------------------------------------------
// Function: SystemComponentAddCommand::SystemComponentAddCommand()
//-----------------------------------------------------------------------------
SystemComponentAddCommand::SystemComponentAddCommand(IGraphicsItemStack* stack, QGraphicsItem* item,
                                                     QSharedPointer<Design> containingDesign,
                                                     QUndoCommand* parent):
QUndoCommand(parent),
item_(item),
stack_(stack),
del_(false),
newHwComponentId_(""),
containingDesign_(containingDesign)
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
        QSharedPointer<SWInstance> swInstance = swItem->getComponentInstance().dynamicCast<SWInstance>();
        if (swInstance)
        {
            QList<QSharedPointer<SWInstance> > containedSWInstances = containingDesign_->getSWInstances();
            containedSWInstances.removeAll(swInstance);
            containingDesign_->setSWInstances(containedSWInstances);

            if (!newHwComponentId_.isEmpty())
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
        QSharedPointer<SWInstance> swInstance = swItem->getComponentInstance().dynamicCast<SWInstance>();
        if (swInstance)
        {
            QList<QSharedPointer<SWInstance> > containedSWInstances = containingDesign_->getSWInstances();
            containedSWInstances.append(swInstance);
            containingDesign_->setSWInstances(containedSWInstances);

            if (!newHwComponentId_.isEmpty())
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
}
