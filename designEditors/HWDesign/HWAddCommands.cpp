//-----------------------------------------------------------------------------
// File: HWAddCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
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

#include <designEditors/common/DesignDiagram.h>
#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>

//-----------------------------------------------------------------------------
// Function: PortAddCommand()
//-----------------------------------------------------------------------------
PortAddCommand::PortAddCommand(HWComponentItem* component, QPointF const& pos,
                               QUndoCommand* parent) : QUndoCommand(parent),
                                                       component_(component), pos_(pos),
                                                       port_(0), scene_(component->scene()),
                                                       del_(false)
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
}

//-----------------------------------------------------------------------------
// Function: ConnectionAddCommand::ConnectionAddCommand()
//-----------------------------------------------------------------------------
ConnectionAddCommand::ConnectionAddCommand(QGraphicsScene* scene, HWConnection* conn,
                                           QSharedPointer<Design> design, QUndoCommand* parent) :
QUndoCommand(parent),
connection_(conn),
design_(design),
mode1_(General::MASTER),
mode2_(General::MASTER),
portMaps_(),
scene_(scene),
del_(false),
portsCopied_(false)

{
    QSharedPointer<GenericEditProvider> editProvider = 
        static_cast<DesignDiagram*>(scene)->getEditProvider().dynamicCast<GenericEditProvider>();
    portsCopied_ = editProvider->getState("portsCopied").toBool();

    QSharedPointer<BusInterface> busIf1 = connection_->endpoint1()->getBusInterface();
    QSharedPointer<BusInterface> busIf2 = connection_->endpoint2()->getBusInterface();

    QSharedPointer<Component> srcComponent;

    if (busIf1 != 0 && busIf1->getBusType().isValid() && !connection_->endpoint1()->isTypeLocked())
    {
        mode1_ = busIf1->getInterfaceMode();
        portMaps_ = *busIf1->getPortMaps();
        srcComponent = connection_->endpoint1()->getOwnerComponent();
    }

    if (busIf2 != 0 && busIf2->getBusType().isValid() && !connection_->endpoint2()->isTypeLocked())
    {
        mode2_ = busIf2->getInterfaceMode();
        portMaps_ = *busIf2->getPortMaps();
        srcComponent = connection_->endpoint2()->getOwnerComponent();
    }

    if (portsCopied_ && srcComponent != 0)
    {
        foreach (QSharedPointer<PortMap> portMap, portMaps_)
        {
            QSharedPointer<Port> port = srcComponent->getPort(portMap->getPhysicalPort()->name_);
            if (!port.isNull())
            {
                new AddPhysicalPortCommand(srcComponent, port, this);
            }
        }
    }
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
    QSharedPointer<GenericEditProvider> editProvider =
        static_cast<DesignDiagram*>(scene_)->getEditProvider().dynamicCast<GenericEditProvider>();
    editProvider->setState("portsCopied", portsCopied_);

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
    QSharedPointer<GenericEditProvider> editProvider = 
        static_cast<DesignDiagram*>(scene_)->getEditProvider().dynamicCast<GenericEditProvider>();
    editProvider->setState("portsCopied", portsCopied_);

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
        changeHierarchicalInterfaceBusReference();

//         QSharedPointer<BusInterface> busIf1 = connection_->endpoint1()->getBusInterface();
//         QSharedPointer<BusInterface> busIf2 = connection_->endpoint2()->getBusInterface();

        design_->getInterconnections()->append(connection_->getInterconnection());
        design_->addRoute(connection_->getRouteExtension());
    }

    del_ = false;
}

//-----------------------------------------------------------------------------
// Function: HWAddCommands::changeHierarchicalInterfaceBusReference()
//-----------------------------------------------------------------------------
void ConnectionAddCommand::changeHierarchicalInterfaceBusReference()
{
    QSharedPointer<Interconnection> interconnection = connection_->getInterconnection();
    if (interconnection)
    {
        if (!interconnection->getHierInterfaces()->isEmpty())
        {
            QSharedPointer<HierInterface> topInterface = interconnection->getHierInterfaces()->first();
            QSharedPointer<ActiveInterface> startInterface = interconnection->getStartInterface();

            ConnectionEndpoint* firstPoint = connection_->endpoint1();
            ConnectionEndpoint* secondPoint = connection_->endpoint2();
            ComponentItem* firstComponent = firstPoint->encompassingComp();

            if (firstComponent != 0 && firstComponent->name() == startInterface->getComponentReference() &&
                firstPoint->getBusInterface()->name() == startInterface->getBusReference())
            {
                topInterface->setBusReference(secondPoint->getBusInterface()->name());
            }
            else
            {
                topInterface->setBusReference(firstPoint->getBusInterface()->name());
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfacePasteCommand()
//-----------------------------------------------------------------------------
BusInterfacePasteCommand::BusInterfacePasteCommand(QSharedPointer<Component> srcComponent, 
    QSharedPointer<Component> destComponent, BusInterfaceItem* interfaceItem, GraphicsColumn* column,
    DesignDiagram* diagram, QUndoCommand* parent) :
QUndoCommand(parent),
    srcComponent_(srcComponent),
    destComponent_(destComponent),
    busInterface_(interfaceItem->getBusInterface()), 
    interfaceItem_(interfaceItem),
    column_(column),
    diagram_(diagram),
    del_(false)
{
    // Create child commands for adding physical ports to target component. 
    // Physical ports must have a unique name within the component.
    foreach (QString const& portName, interfaceItem->getBusInterface()->getPhysicalPortNames())
    {	
        QString uniquePortName = portName;
        unsigned int count = 0;
        while (destComponent->getPort(uniquePortName) != 0)
        {
            count++;
            uniquePortName = portName + "_" + QString::number(count);
        }

        if (srcComponent->hasPort(portName))
        {
            // Create copies of the physical ports in the source component and rename them.
            QSharedPointer<Port> physPortCopy = QSharedPointer<Port>(new Port(*srcComponent->getPort(portName)));
            physPortCopy->setName(uniquePortName);	

            // If port name changed, it is also changed in bus interface.
            if (uniquePortName != portName)
            {
                foreach (QSharedPointer<PortMap> portMap, *interfaceItem->getBusInterface()->getPortMaps())
                {
                    if (portMap->getPhysicalPort()->name_ == portName)
                    {
                        portMap->getPhysicalPort()->name_ = uniquePortName;
                    }
                }
            }

            new AddPhysicalPortCommand(destComponent_, physPortCopy, this);
        }       
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfacePasteCommand::BusInterfacePasteCommand()
//-----------------------------------------------------------------------------
BusInterfacePasteCommand::BusInterfacePasteCommand(QSharedPointer<Component> srcComponent, 
    QSharedPointer<Component> destComponent, BusInterfaceItem* interfaceItem, 
    GraphicsColumn* column, DesignDiagram* diagram, QList<QSharedPointer<Port> > ports, QUndoCommand* parent):
QUndoCommand(parent),
    srcComponent_(srcComponent),
    destComponent_(destComponent),
    busInterface_(interfaceItem->getBusInterface()), 
    interfaceItem_(interfaceItem),
    column_(column),
    diagram_(diagram),
    del_(false)
{
    // Create child commands for adding pre-defined ports.
    foreach (QSharedPointer<Port> port, ports)
    {
        new AddPhysicalPortCommand(destComponent_, port, this);
    }
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
    component_->getPorts()->append(port_);

	// Execute child commands.
    QUndoCommand::redo();
}
