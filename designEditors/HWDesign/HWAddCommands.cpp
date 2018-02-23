//-----------------------------------------------------------------------------
// File: HWAddCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.8.2011
//
// Description:
// Undo add commands for the design diagram.
//-----------------------------------------------------------------------------

#include "HWAddCommands.h"

#include "HWConnection.h"
#include "BusPortItem.h"
#include "HWComponentItem.h"
#include "BusInterfaceItem.h"

#include <common/GenericEditProvider.h>
#include <common/graphicsItems/GraphicsColumn.h>
#include <common/graphicsItems/GraphicsColumnConstants.h>

#include <designEditors/common/DesignDiagram.h>
#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>

//-----------------------------------------------------------------------------
// Function: PortAddCommand()
//-----------------------------------------------------------------------------
PortAddCommand::PortAddCommand(HWComponentItem* component, QPointF const& pos, DesignDiagram* diagram,
    QUndoCommand* parent):
QUndoCommand(parent),
component_(component),
pos_(pos),
port_(0), 
scene_(component->scene()),
del_(false),
diagram_(diagram)
{

}

//-----------------------------------------------------------------------------
// Function: ~PortAddCommand()
//-----------------------------------------------------------------------------
PortAddCommand::~PortAddCommand()
{
    if (del_)
    {
        delete port_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void PortAddCommand::undo()
{
    Q_ASSERT(port_ != 0);

    // Remove the port from the component and from the scene
    component_->removePort(port_);
    scene_->removeItem(port_);
    del_ = true;

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void PortAddCommand::redo()
{
    // Add a port to the component.
    if (port_ == 0)
    {
        port_ = component_->addPort(pos_);
    }
    else
    {
        component_->addPort(port_);
    }

    del_ = false;

    // Child commands need not be executed because the other ports change their position
    // in a deterministic way.
    //QUndoCommand::redo();

    if (port_->scenePos().y() + GraphicsColumnConstants::MIN_Y_PLACEMENT > diagram_->sceneRect().height())
    {
        diagram_->resetSceneRectangleForItems();
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectionAddCommand::ConnectionAddCommand()
//-----------------------------------------------------------------------------
ConnectionAddCommand::ConnectionAddCommand(QGraphicsScene* scene, HWConnection* connectionItem,
    QSharedPointer<Design> design, QUndoCommand* parent) :
QUndoCommand(parent),
    connection_(connectionItem),
    design_(design),
    scene_(scene),
    del_(false)
{
  
}

//-----------------------------------------------------------------------------
// Function: ConnectionAddCommand::~ConnectionAddCommand()
//-----------------------------------------------------------------------------
ConnectionAddCommand::~ConnectionAddCommand()
{
    if (del_)
    {
        delete connection_;
    }
}

//-----------------------------------------------------------------------------
// Function: ConnectionAddCommand::undo()
//-----------------------------------------------------------------------------
void ConnectionAddCommand::undo()
{
    // Remove the interconnection from the scene.
    if (connection_->scene() != 0)
    {
        scene_->removeItem(connection_);
    }

    // Disconnect the ends.
    connection_->setSelected(false);
    connection_->disconnectEnds();

    del_ = true;

    design_->getInterconnections()->removeAll(connection_->getInterconnection());
    design_->removeRoute(connection_->getRouteExtension());
    
    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: ConnectionAddCommand::redo()
//-----------------------------------------------------------------------------
void ConnectionAddCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    // Add the back to the scene.
    if (connection_->scene() != scene_)
    {
        scene_->addItem(connection_);
    }

    // Connect the ends and set the interface modes and port map for the hierarchical end point.
    if (connection_->connectEnds())
    {
        design_->getInterconnections()->append(connection_->getInterconnection());
        design_->addRoute(connection_->getRouteExtension());
    }

    del_ = false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfacePasteCommand()
//-----------------------------------------------------------------------------
BusInterfacePasteCommand::BusInterfacePasteCommand(
    QSharedPointer<Component> destComponent, BusInterfaceItem* interfaceItem, GraphicsColumn* column,
    DesignDiagram* diagram, QUndoCommand* parent) :
QUndoCommand(parent),
    destComponent_(destComponent),
    busInterface_(interfaceItem->getBusInterface()), 
    interfaceItem_(interfaceItem),
    column_(column),
    diagram_(diagram),
    del_(false)
{
  
}

//-----------------------------------------------------------------------------
// Function: BusInterfacePasteCommand::~BusInterfacePasteCommand()
//-----------------------------------------------------------------------------
BusInterfacePasteCommand::~BusInterfacePasteCommand()
{
    if (del_)
    {
        delete interfaceItem_;
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfacePasteCommand::undo()
//-----------------------------------------------------------------------------
void BusInterfacePasteCommand::undo()
{
    // Execute child commands.
    QUndoCommand::undo();

    Q_ASSERT(interfaceItem_ != 0);

    // Remove the port from the component and from the scene
    destComponent_->getBusInterfaces()->removeOne(busInterface_);

    column_->removeItem(interfaceItem_);
    diagram_->removeItem(interfaceItem_);       
    diagram_->getDesign()->getVendorExtensions()->removeOne(interfaceItem_->getDataExtension());

    del_ = true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfacePasteCommand::redo()
//-----------------------------------------------------------------------------
void BusInterfacePasteCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();    

    Q_ASSERT(interfaceItem_ != 0);

    // Copy a port to the component.
    destComponent_->getBusInterfaces()->append(busInterface_);
    column_->addItem(interfaceItem_);
    diagram_->getDesign()->getVendorExtensions()->append(interfaceItem_->getDataExtension());

    del_ = false;

    interfaceItem_->updateInterface();

    if (interfaceItem_->scenePos().y() + GraphicsColumnConstants::MIN_Y_PLACEMENT > diagram_->sceneRect().height())
    {
        diagram_->resetSceneRectangleForItems();
    }
}

//-----------------------------------------------------------------------------
// Function: AddPhysicalPortCommand()
//-----------------------------------------------------------------------------
AddPhysicalPortCommand::AddPhysicalPortCommand(QSharedPointer<Component> component,
                                               QSharedPointer<Port> port,
                                               QUndoCommand* parent)
    : QUndoCommand(parent),
      component_(component),
      port_(port)                                                           
{
}

//-----------------------------------------------------------------------------
// Function: ~AddPhysicalPortCommand()
//-----------------------------------------------------------------------------
AddPhysicalPortCommand::~AddPhysicalPortCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void AddPhysicalPortCommand::undo()
{
    Q_ASSERT(component_ != 0 && port_ != 0);
    component_->getPorts()->removeOne(port_);
	
    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void AddPhysicalPortCommand::redo()
{
	Q_ASSERT(component_ != 0 && port_ != 0);
    
    if (!component_->getPorts()->contains(port_))
    {
        component_->getPorts()->append(port_);
    }
  
	// Execute child commands.
    QUndoCommand::redo();
}
