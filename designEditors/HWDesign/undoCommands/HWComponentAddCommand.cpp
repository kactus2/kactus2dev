//-----------------------------------------------------------------------------
// File: HWComponentAddCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 18.04.2016
//
// Description:
// Undo add command for component items in HW design.
//-----------------------------------------------------------------------------

#include "HWComponentAddCommand.h"

#include <common/graphicsItems/IGraphicsItemStack.h>

#include <designEditors/HWDesign/HWComponentItem.h>

#include <IPXACTmodels/Design/ComponentInstance.h>
#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: HWComponentAddCommand::HWComponentAddCommand()
//-----------------------------------------------------------------------------
HWComponentAddCommand::HWComponentAddCommand(QSharedPointer<Design> design, IGraphicsItemStack* stack,
                                             ComponentItem* item, QUndoCommand* parent /* = 0 */):
QUndoCommand(parent),
containingDesign_(design),
item_(item),
stack_(stack),
del_(false)
{

}

//-----------------------------------------------------------------------------
// Function: HWComponentAddCommand::~HWComponentAddCommand()
//-----------------------------------------------------------------------------
HWComponentAddCommand::~HWComponentAddCommand()
{
    if (del_)
    {
        delete item_;
    }
}

//-----------------------------------------------------------------------------
// Function: HWComponentAddCommand::undo()
//-----------------------------------------------------------------------------
void HWComponentAddCommand::undo()
{
    stack_->removeItem(item_);
    item_->scene()->removeItem(item_);
    del_ = true;

    QSharedPointer<ComponentInstance> itemInstance = item_->getComponentInstance();
    containingDesign_->getComponentInstances()->removeAll(itemInstance);

    emit componentInstanceRemoved(item_);

    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: HWComponentAddCommand::redo()
//-----------------------------------------------------------------------------
void HWComponentAddCommand::redo()
{
    stack_->addItem(item_);
    del_ = false;

    QSharedPointer<ComponentInstance> itemInstance = item_->getComponentInstance();
    containingDesign_->getComponentInstances()->append(itemInstance);

    emit componentInstantiated(item_);
}
