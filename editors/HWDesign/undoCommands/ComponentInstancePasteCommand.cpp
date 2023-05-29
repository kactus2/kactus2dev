//-----------------------------------------------------------------------------
// File: ComponentInstancePasteCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 15.02.2016
//
// Description:
// Undo command for pasting a component instance in a design.
//-----------------------------------------------------------------------------

#include "ComponentInstancePasteCommand.h"

#include <common/graphicsItems/CommonGraphicsUndoCommands.h>
#include <common/graphicsItems/GraphicsColumn.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>

#include <editors/common/DesignDiagram.h>

#include <editors/HWDesign/HWComponentItem.h>
#include <editors/HWDesign/undoCommands/HWComponentAddCommand.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/ComponentInstance.h>
#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: ComponentInstancePasteCommand::ComponentInstancePasteCommand()
//-----------------------------------------------------------------------------
ComponentInstancePasteCommand::ComponentInstancePasteCommand(QSharedPointer<Component> component,
    QSharedPointer<ComponentInstance> componentInstance, QPointF const& position, GraphicsColumn* column,
    DesignDiagram* diagram, QUndoCommand* parent):
QUndoCommand(parent),
componentInstance_(new ComponentInstance(*componentInstance)),
design_(diagram->getDesign())
{
    // Take a copy of the component in case of a draft.
    QSharedPointer<Component> componentCopy = component;
    if (!componentCopy->getVlnv().isValid())
    {
        componentCopy = QSharedPointer<Component>(new Component(*component));
    }

    auto uniqueInstanceName = diagram->createInstanceName(componentInstance->getInstanceName());
    componentInstance_->setInstanceName(uniqueInstanceName);

    HWComponentItem* componentItem = new HWComponentItem(diagram->getLibraryInterface(), componentInstance_, 
        componentCopy, column);
    componentItem->setPos(position);

    GraphicsColumn* targetColumn = column;

    // Check if the column does not accept the given component.
    if (!targetColumn->isItemAllowed(componentItem))
    {
        targetColumn = 0;

        // Find the first column that accepts it.
        foreach (GraphicsColumn* otherColumn, diagram->getLayout()->getColumns())
        {
            if (otherColumn->isItemAllowed(componentItem))
            {
                targetColumn = otherColumn;
                break;
            }
        }
    }

    if (targetColumn != 0)
    {
        HWComponentAddCommand* graphicsAddCommand =
            new HWComponentAddCommand(diagram, targetColumn, componentItem, this);

        QObject::connect(graphicsAddCommand, SIGNAL(componentInstantiated(ComponentItem*)),
            diagram, SIGNAL(componentInstantiated(ComponentItem*)), Qt::UniqueConnection);
        QObject::connect(graphicsAddCommand, SIGNAL(componentInstanceRemoved(ComponentItem*)),
            diagram, SIGNAL(componentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstancePasteCommand::~ComponentInstancePasteCommand()
//-----------------------------------------------------------------------------
ComponentInstancePasteCommand::~ComponentInstancePasteCommand()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstancePasteCommand::undo()
//-----------------------------------------------------------------------------
void ComponentInstancePasteCommand::undo()
{
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstancePasteCommand::redo()
//-----------------------------------------------------------------------------
void ComponentInstancePasteCommand::redo()
{
    QUndoCommand::redo();
}
