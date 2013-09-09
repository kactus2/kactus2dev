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

#include <common/DesignDiagram.h>
#include <common/GenericEditProvider.h>
#include <common/graphicsItems/GraphicsColumn.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>

#include <models/component.h>
#include <models/businterface.h>
#include <models/port.h>

//-----------------------------------------------------------------------------
// Function: ColumnDeleteCommand()
//-----------------------------------------------------------------------------
ColumnDeleteCommand::ColumnDeleteCommand(GraphicsColumnLayout* layout, GraphicsColumn* column,
                                         QUndoCommand* parent) : QUndoCommand(parent),
                                                                 layout_(layout), column_(column), del_(true)
{
    // Create child commands for removing interconnections.
    foreach (QGraphicsItem* item, column->childItems())
    {
        if (item->type() == HWComponentItem::Type)
        {
            HWComponentItem* comp = static_cast<HWComponentItem*>(item);

            foreach (QGraphicsItem* childItem, comp->childItems())
            {
                if (childItem->type() != BusPortItem::Type)
                {
                    continue;
                }

                BusPortItem *diagramPort = qgraphicsitem_cast<BusPortItem *>(childItem);

                foreach (GraphicsConnection* conn, diagramPort->getConnections())
                {
                    new ConnectionDeleteCommand(static_cast<HWConnection*>(conn), this);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ~ColumnDeleteCommand()
//-----------------------------------------------------------------------------
ColumnDeleteCommand::~ColumnDeleteCommand()
{
    if (del_)
    {
        delete column_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ColumnDeleteCommand::undo()
{
    // Add the item back to the layout.
    layout_->addColumn(column_);
    del_ = false;

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ColumnDeleteCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    // Remove the item from the layout.
    layout_->removeColumn(column_);
    del_ = true;
}

//-----------------------------------------------------------------------------
// Function: ComponentDeleteCommand()
//-----------------------------------------------------------------------------
ComponentDeleteCommand::ComponentDeleteCommand(HWComponentItem* component, QUndoCommand* parent) :
    QUndoCommand(parent), component_(component), parent_(static_cast<GraphicsColumn*>(component->parentItem())),
    scene_(component->scene()),
    del_(true),
    firstRun_(true)
{
}

//-----------------------------------------------------------------------------
// Function: ~ComponentDeleteCommand()
//-----------------------------------------------------------------------------
ComponentDeleteCommand::~ComponentDeleteCommand()
{
    if (del_)
    {
        delete component_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ComponentDeleteCommand::undo()
{
    // Add the item back to the scene.
    parent_->addItem(component_);
    del_ = false;

	emit componentInstantiated(component_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ComponentDeleteCommand::redo()
{
    if (firstRun_)
    {
        // Create child commands for removing interconnections.
        foreach (QGraphicsItem *item, component_->childItems())
        {
            HWConnectionEndpoint* endpoint = dynamic_cast<HWConnectionEndpoint*>(item);

            if (endpoint == 0)
            {
                continue;
            }

            foreach (GraphicsConnection* conn, endpoint->getConnections())
            {
                new ConnectionDeleteCommand(static_cast<HWConnection*>(conn), this);
            }

            if (endpoint->getOffPageConnector() != 0)
            {
                foreach (GraphicsConnection* conn, endpoint->getOffPageConnector()->getConnections())
                {
                    new ConnectionDeleteCommand(static_cast<HWConnection*>(conn), this);
                }
            }
        }

        firstRun_ = false;
    }

    // Execute child commands.
    QUndoCommand::redo();

    // Remove the item from the scene.
    parent_->removeItem(component_);
    scene_->removeItem(component_);
    del_ = true;

	emit componentInstanceRemoved(component_);
}

//-----------------------------------------------------------------------------
// Function: ConnectionDeleteCommand()
//-----------------------------------------------------------------------------
ConnectionDeleteCommand::ConnectionDeleteCommand(HWConnection* conn, QUndoCommand* parent)
    : QUndoCommand(parent),
      conn_(conn),
      mode1_(General::MASTER),
      mode2_(General::MASTER),
      portMaps_(),
      portsCopied_(false),
      scene_(conn->scene()),
      del_(true)
{
    QSharedPointer<BusInterface> busIf1 = conn_->endpoint1()->getBusInterface();
    QSharedPointer<BusInterface> busIf2 = conn_->endpoint2()->getBusInterface();

    QSharedPointer<Component> srcComponent;

    bool lastConnection = false;

    if (busIf1 != 0 && busIf1->getBusType().isValid() && !conn_->endpoint1()->isTypeLocked())
    {
        mode1_ = busIf1->getInterfaceMode();
        portMaps_ = busIf1->getPortMaps();

        BusInterfaceItem* busIfItem = dynamic_cast<BusInterfaceItem*>(conn_->endpoint1());
        
        if (busIfItem != 0)
        {
            portsCopied_ = busIfItem->hasPortsCopied();
            lastConnection = busIfItem->getConnections().size() == 1;
            srcComponent = conn_->endpoint1()->getOwnerComponent();
        }
    }

    if (busIf2 != 0 && busIf2->getBusType().isValid() && !conn_->endpoint2()->isTypeLocked())
    {
        mode2_ = busIf2->getInterfaceMode();
        portMaps_ = busIf2->getPortMaps();

        BusInterfaceItem* busIfItem = dynamic_cast<BusInterfaceItem*>(conn_->endpoint2());

        if (busIfItem != 0)
        {
            portsCopied_ = busIfItem->hasPortsCopied();
            lastConnection = busIfItem->getConnections().size() == 1;
            srcComponent = conn_->endpoint2()->getOwnerComponent();
        }
    }

    if (portsCopied_ && lastConnection)
    {
        foreach (QSharedPointer<General::PortMap> portMap, portMaps_)
        {
            QSharedPointer<Port> port = srcComponent->getPort(portMap->physicalPort_);
            DeletePhysicalPortCommand* delCmd = new DeletePhysicalPortCommand(srcComponent, port, this);

            // If the port is visible as ad-hoc in the current design, it must be hidden.
            DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(scene_);
            Q_ASSERT(diagram);
            if (diagram->getDiagramAdHocPort(port->getName()) != 0)
            {                               
                new AdHocVisibilityChangeCommand(diagram, port->getName(), false, delCmd);
            }        
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ~ConnectionDeleteCommand()
//-----------------------------------------------------------------------------
ConnectionDeleteCommand::~ConnectionDeleteCommand()
{
    if (del_)
    {
        delete conn_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ConnectionDeleteCommand::undo()
{
    // Execute child commands.
    QUndoCommand::undo();

    GenericEditProvider& editProvider = static_cast<DesignDiagram*>(scene_)->getEditProvider();
    editProvider.setState("portsCopied", portsCopied_);

    // Add the item back to the scene.
    scene_->addItem(conn_);
    
    // Connect the ends and set the interface modes.
    conn_->connectEnds();
    
    QSharedPointer<BusInterface> busIf1 = conn_->endpoint1()->getBusInterface();
    QSharedPointer<BusInterface> busIf2 = conn_->endpoint2()->getBusInterface();

    if (busIf1 != 0 && busIf1->getBusType().isValid() && !conn_->endpoint1()->isTypeLocked())
    {
        busIf1->setInterfaceMode(mode1_);
        busIf1->setPortMaps(portMaps_);
        conn_->endpoint1()->updateInterface();
    }

    if (busIf2 != 0 && busIf2->getBusType().isValid() && !conn_->endpoint2()->isTypeLocked())
    {
        busIf2->setInterfaceMode(mode2_);
        busIf2->setPortMaps(portMaps_);
        conn_->endpoint2()->updateInterface();
    }

    scene_->clearSelection();
    conn_->setVisible(true);
    conn_->setSelected(true);
    del_ = false;
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ConnectionDeleteCommand::redo()
{
    GenericEditProvider& editProvider = static_cast<DesignDiagram*>(scene_)->getEditProvider();
    editProvider.setState("portsCopied", portsCopied_);

    // Disconnect the ends.
    conn_->setSelected(false);
    conn_->disconnectEnds();

    // Remove the item from the scene.
    scene_->removeItem(conn_);
    del_ = true;

    // Execute child commands.
    QUndoCommand::redo();
}

//-----------------------------------------------------------------------------
// Function: PortDeleteCommand()
//-----------------------------------------------------------------------------
PortDeleteCommand::PortDeleteCommand(HWConnectionEndpoint* port, QUndoCommand* parent) :
    QUndoCommand(parent), port_(port), parent_(static_cast<HWComponentItem*>(port->parentItem())),
    scene_(port->scene()), del_(true)
{
    // Create child commands for removing interconnections.
    foreach (GraphicsConnection* conn, port_->getConnections())
    {
        new ConnectionDeleteCommand(static_cast<HWConnection*>(conn), this);
    }

    foreach (GraphicsConnection* conn, port_->getOffPageConnector()->getConnections())
    {
        new ConnectionDeleteCommand(static_cast<HWConnection*>(conn), this);
    }

    foreach (QSharedPointer<General::PortMap> portMap, port_->getBusInterface()->getPortMaps() )
    {
        new DeletePhysicalPortCommand(parent_->componentModel(),
                                      parent_->componentModel()->getPort(portMap->physicalPort_),
                                      this);
    }

}

//-----------------------------------------------------------------------------
// Function: ~PortDeleteCommand()
//-----------------------------------------------------------------------------
PortDeleteCommand::~PortDeleteCommand()
{
    if (del_)
    {
        delete port_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void PortDeleteCommand::undo()
{
    // Add the port back to the scene.
    //scene_->addItem(port_);

    // Add the port to the parent component.
    parent_->addPort(port_);

    del_ = false;

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void PortDeleteCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    // Remove the port from the parent component.
    parent_->removePort(port_);

    // Remove the port from the scene.
    scene_->removeItem(port_);
    del_ = true;
}

//-----------------------------------------------------------------------------
// Function: InterfaceDeleteCommand()
//-----------------------------------------------------------------------------
InterfaceDeleteCommand::InterfaceDeleteCommand(BusInterfaceItem* interface,
                                               bool removePorts, QUndoCommand* parent)
    : QUndoCommand(parent),
      interface_(interface),
      busIf_(interface_->getBusInterface()),
      ports_(),
      mode_(interface_->getBusInterface()->getInterfaceMode()),
      portMaps_(interface_->getBusInterface()->getPortMaps()),
      parent_(static_cast<GraphicsColumn*>(interface->parentItem())),
      scene_(interface->scene()),
      del_(true),
      removePorts_(removePorts)
{
    if (removePorts_)
    {
        // Create copies of the related ports and remove commands for them.
        QList<QSharedPointer<Port> > ports = interface_->getPorts();
        foreach (QSharedPointer<Port> port, ports)
        {
            ports_.append(QSharedPointer<Port>(new Port(*port.data())));
            DeletePhysicalPortCommand* delCmd = new DeletePhysicalPortCommand(interface_->getOwnerComponent(), port, this);

            // If the port is visible as ad-hoc in the current design, it must be hidden.
            DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(interface->scene());
            Q_ASSERT(diagram);
            if (diagram->getDiagramAdHocPort(port->getName()) != 0)
            {                               
                new AdHocVisibilityChangeCommand(diagram, port->getName(), false, delCmd);
            }
        }
    }

    // Create child commands for removing interconnections.
    foreach (GraphicsConnection* conn, interface_->getConnections())
    {
        new ConnectionDeleteCommand(static_cast<HWConnection*>(conn), this);
    }

    foreach (GraphicsConnection* conn, interface_->getOffPageConnector()->getConnections())
    {
        new ConnectionDeleteCommand(static_cast<HWConnection*>(conn), this);
    }
}

//-----------------------------------------------------------------------------
// Function: ~InterfaceDeleteCommand()
//-----------------------------------------------------------------------------
InterfaceDeleteCommand::~InterfaceDeleteCommand()
{
    if (del_)
    {
        delete interface_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void InterfaceDeleteCommand::undo()
{
    // Add the interface back to the scene.
    parent_->addItem(interface_);
    del_ = false;

    // Redefine the interface.
    if (busIf_ != 0)
    {
        busIf_->setInterfaceMode(mode_);
        busIf_->setPortMaps(portMaps_);
        interface_->define(busIf_, false, ports_);
    }

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void InterfaceDeleteCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    // Undefine the interface.
    if (busIf_ != 0)
    {
        interface_->undefine(removePorts_);
    }

    // Remove the interface from the scene.
    parent_->removeItem(interface_);
    scene_->removeItem(interface_);
    emit interfaceDeleted();
    del_ = true;
}

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
    component_->addPort(port_);

    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void DeletePhysicalPortCommand::redo()
{
    QUndoCommand::redo();

    Q_ASSERT(component_ != 0 && !port_.isNull());
    component_->removePort(port_->getName());
}