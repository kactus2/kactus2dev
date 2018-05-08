//-----------------------------------------------------------------------------
// File: PhysicalPortDeleteCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.09.2015
//
// Description:
// Undo command for removing a physical port in component.
//-----------------------------------------------------------------------------

#include "PhysicalPortDeleteCommand.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>

//-----------------------------------------------------------------------------
// Function: DeletePhysicalPortCommand::DeletePhysicalPortCommand()
//-----------------------------------------------------------------------------
DeletePhysicalPortCommand::DeletePhysicalPortCommand(QSharedPointer<Component> component,
    QSharedPointer<Port> port, QUndoCommand* parent):
QUndoCommand(parent),
component_(component),
port_(port)                                                           
{

}

//-----------------------------------------------------------------------------
// Function: DeletePhysicalPortCommand::~DeletePhysicalPortCommand()
//-----------------------------------------------------------------------------
DeletePhysicalPortCommand::~DeletePhysicalPortCommand()
{
}

//-----------------------------------------------------------------------------
// Function: DeletePhysicalPortCommand::undo()
//-----------------------------------------------------------------------------
void DeletePhysicalPortCommand::undo()
{
    Q_ASSERT(component_ != 0 && !port_.isNull());
    component_->getPorts()->append(port_);

    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: DeletePhysicalPortCommand::redo()
//-----------------------------------------------------------------------------
void DeletePhysicalPortCommand::redo()
{
    QUndoCommand::redo();

    Q_ASSERT(component_ != 0 && !port_.isNull());
    component_->getPorts()->removeAll(port_);
}
