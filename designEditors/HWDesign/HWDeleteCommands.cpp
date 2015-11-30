//-----------------------------------------------------------------------------
// File: HWDeleteCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 1.8.2011
//
// Description:
// Undo commands for the design widget.
//-----------------------------------------------------------------------------

#include "HWDeleteCommands.h"

#include "HWChangeCommands.h"
#include "HWConnection.h"
#include "BusPortItem.h"
#include "HWComponentItem.h"
#include "BusInterfaceItem.h"

#include <designEditors/HWDesign/undoCommands/AdHocVisibilityChangeCommand.h>

#include <common/GenericEditProvider.h>
#include <common/graphicsItems/GraphicsColumn.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>

#include <designEditors/common/Association/Association.h>
#include <designEditors/common/DesignDiagram.h>
#include <designEditors/common/Association/AssociationRemoveCommand.h>

#include <designEditors/HWDesign/undoCommands/ComponentDeleteCommand.h>
#include <designEditors/HWDesign/undoCommands/ConnectionDeleteCommand.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/PortMap.h>

//-----------------------------------------------------------------------------
// Function: DeletePhysicalPortCommand()
//-----------------------------------------------------------------------------
DeletePhysicalPortCommand::DeletePhysicalPortCommand(QSharedPointer<Component> component,
                                                   QSharedPointer<Port> port,
                                                   QUndoCommand* parent)
    : QUndoCommand(parent),
      component_(component),
      port_(port)                                                           
{
}

//-----------------------------------------------------------------------------
// Function: ~DeletePhysicalPortCommand()
//-----------------------------------------------------------------------------
DeletePhysicalPortCommand::~DeletePhysicalPortCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void DeletePhysicalPortCommand::undo()
{
    Q_ASSERT(component_ != 0 && !port_.isNull());
    component_->getPorts()->append(port_);

    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void DeletePhysicalPortCommand::redo()
{
    QUndoCommand::redo();

    Q_ASSERT(component_ != 0 && !port_.isNull());
    component_->getPorts()->removeOne(port_);
}
