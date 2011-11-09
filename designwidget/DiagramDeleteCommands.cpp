//-----------------------------------------------------------------------------
// File: DiagramDeleteCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 1.8.2011
//
// Description:
// Undo commands for the design widget.
//-----------------------------------------------------------------------------

#include "DiagramDeleteCommands.h"

#include "diagraminterconnection.h"
#include "diagramport.h"
#include "diagramcomponent.h"
#include "diagraminterface.h"

#include "columnview/DiagramColumn.h"
#include "columnview/DiagramColumnLayout.h"

#include <models/component.h>
#include <models/businterface.h>
#include <models/port.h>

//-----------------------------------------------------------------------------
// Function: ColumnDeleteCommand()
//-----------------------------------------------------------------------------
ColumnDeleteCommand::ColumnDeleteCommand(DiagramColumnLayout* layout, DiagramColumn* column,
                                         QUndoCommand* parent) : QUndoCommand(parent),
                                                                 layout_(layout), column_(column), del_(true)
{
    // Create child commands for removing interconnections.
    foreach (QGraphicsItem* item, column->childItems())
    {
        if (item->type() == DiagramComponent::Type)
        {
            DiagramComponent* comp = static_cast<DiagramComponent*>(item);

            foreach (QGraphicsItem* childItem, comp->childItems())
            {
                if (childItem->type() != DiagramPort::Type)
                {
                    continue;
                }

                DiagramPort *diagramPort = qgraphicsitem_cast<DiagramPort *>(childItem);

                foreach (DiagramInterconnection* conn, diagramPort->getInterconnections())
                {
                    QUndoCommand* cmd = new ConnectionDeleteCommand(conn, this);
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
ComponentDeleteCommand::ComponentDeleteCommand(DiagramComponent* component, QUndoCommand* parent) :
    QUndoCommand(parent), component_(component), parent_(static_cast<DiagramColumn*>(component->parentItem())),
    scene_(component->scene()), del_(true)
{
    // Create child commands for removing interconnections.
    foreach (QGraphicsItem *item, component_->childItems())
    {
        if (item->type() != DiagramPort::Type)
        {
            continue;
        }

        DiagramPort *diagramPort = qgraphicsitem_cast<DiagramPort *>(item);

        foreach (DiagramInterconnection* conn, diagramPort->getInterconnections())
        {
            QUndoCommand* cmd = new ConnectionDeleteCommand(conn, this);
        }
    }
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
ConnectionDeleteCommand::ConnectionDeleteCommand(DiagramInterconnection* conn,
                                                 QUndoCommand* parent) : QUndoCommand(parent), conn_(conn),
                                                 mode1_(General::MASTER), mode2_(General::MASTER),
                                                 portMaps_(), scene_(conn->scene()), del_(true)
{
    QSharedPointer<BusInterface> busIf1 = conn_->endPoint1()->getBusInterface();
    QSharedPointer<BusInterface> busIf2 = conn_->endPoint2()->getBusInterface();

    if (busIf1 != 0 && busIf1->getBusType().isValid())
    {
        mode1_ = busIf1->getInterfaceMode();

        if (conn_->endPoint1()->isHierarchical())
        {
            portMaps_ = busIf1->getPortMaps();
        }
    }

    if (busIf2 != 0 && busIf2->getBusType().isValid())
    {
        mode2_ = busIf2->getInterfaceMode();

        if (conn_->endPoint2()->isHierarchical())
        {
            portMaps_ = busIf2->getPortMaps();
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
    // Add the item back to the scene.
    scene_->addItem(conn_);
    
    // Connect the ends and set the interface modes.
    conn_->connectEnds();

    QSharedPointer<BusInterface> busIf1 = conn_->endPoint1()->getBusInterface();
    QSharedPointer<BusInterface> busIf2 = conn_->endPoint2()->getBusInterface();

    if (busIf1 != 0 && busIf1->getBusType().isValid())
    {
        busIf1->setInterfaceMode(mode1_);

        if (conn_->endPoint1()->isHierarchical())
        {
            busIf1->setPortMaps(portMaps_);
        }

        conn_->endPoint1()->updateInterface();
    }

    if (busIf2 != 0 && busIf2->getBusType().isValid())
    {
        busIf2->setInterfaceMode(mode2_);

        if (conn_->endPoint2()->isHierarchical())
        {
            busIf2->setPortMaps(portMaps_);
        }

        conn_->endPoint2()->updateInterface();
    }

    del_ = false;
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ConnectionDeleteCommand::redo()
{
    // Disconnect the ends.
    conn_->disconnectEnds();

    // Remove the item from the scene.
    scene_->removeItem(conn_);

    del_ = true;
}

//-----------------------------------------------------------------------------
// Function: PortDeleteCommand()
//-----------------------------------------------------------------------------
PortDeleteCommand::PortDeleteCommand(DiagramPort* port, QUndoCommand* parent) :
    QUndoCommand(parent), port_(port), parent_(static_cast<DiagramComponent*>(port->parentItem())),
    scene_(port->scene()), del_(true)
{
    // Create child commands for removing interconnections.
    foreach (DiagramInterconnection* conn, port_->getInterconnections())
    {
        QUndoCommand* cmd = new ConnectionDeleteCommand(conn, this);
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
    scene_->addItem(port_);

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
InterfaceDeleteCommand::InterfaceDeleteCommand(DiagramInterface* interface, QUndoCommand* parent) :
    QUndoCommand(parent), interface_(interface), busIf_(interface_->getBusInterface()),
    parent_(static_cast<DiagramColumn*>(interface->parentItem())),
    scene_(interface->scene()), del_(true)
{
    // Create copies of the related ports.
    QList<Port*> ports = interface_->getPorts();

    foreach (Port* port, ports)
    {
        ports_.append(QSharedPointer<Port>(new Port(*port)));
    }

    // Create child commands for removing interconnections.
    foreach (DiagramInterconnection* conn, interface_->getInterconnections())
    {
        QUndoCommand* cmd = new ConnectionDeleteCommand(conn, this);
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
        interface_->define(busIf_, ports_);
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
        interface_->undefine();
    }

    // Remove the interface from the scene.
    parent_->removeItem(interface_);
    scene_->removeItem(interface_);
    del_ = true;
}
