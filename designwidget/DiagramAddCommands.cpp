//-----------------------------------------------------------------------------
// File: DiagramAddCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.8.2011
//
// Description:
// Undo add commands for the design diagram.
//-----------------------------------------------------------------------------

#include "DiagramAddCommands.h"

#include "diagraminterconnection.h"
#include "diagramport.h"
#include "diagramcomponent.h"
#include "diagraminterface.h"

#include "columnview/DiagramColumn.h"
#include "columnview/DiagramColumnLayout.h"

#include <models/businterface.h>

//-----------------------------------------------------------------------------
// Function: ItemAddCommand()
//-----------------------------------------------------------------------------
ItemAddCommand::ItemAddCommand(DiagramColumn* column, QGraphicsItem* item,
                               QUndoCommand* parent) : QUndoCommand(parent), item_(item),
                                                       column_(column), del_(false)
{
}

//-----------------------------------------------------------------------------
// Function: ~ItemAddCommand()
//-----------------------------------------------------------------------------
ItemAddCommand::~ItemAddCommand()
{
    if (del_)
    {
        delete item_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ItemAddCommand::undo()
{
    // Remove the item from the column and the scene.
    column_->removeItem(item_);
    column_->scene()->removeItem(item_);
    del_ = true;

	if (item_->type() == DiagramComponent::Type)
		emit componentInstanceRemoved(static_cast<DiagramComponent*>(item_));

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ItemAddCommand::redo()
{
    // Add the item to the column.
    column_->addItem(item_);
    del_ = false;

	if (item_->type() == DiagramComponent::Type)
		emit componentInstantiated(static_cast<DiagramComponent*>(item_));

    // Child commands need not be executed because the other items change their position
    // in a deterministic way.
    //QUndoCommand::redo();
}

//-----------------------------------------------------------------------------
// Function: PortAddCommand()
//-----------------------------------------------------------------------------
PortAddCommand::PortAddCommand(DiagramComponent* component, QPointF const& pos,
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
// Function: ColumnAddCommand()
//-----------------------------------------------------------------------------
ColumnAddCommand::ColumnAddCommand(DiagramColumnLayout* layout, QString const& name,
                                   ColumnContentType contentType, unsigned int allowedItems,
                                   QUndoCommand* parent) : QUndoCommand(parent), layout_(layout),
                                                           name_(name), contentType_(contentType),
                                                           allowedItems_(allowedItems),
                                                           column_(0), del_(false)
{
}

//-----------------------------------------------------------------------------
// Function: ~ColumnAddCommand()
//-----------------------------------------------------------------------------
ColumnAddCommand::~ColumnAddCommand()
{
    if (del_)
    {
        delete column_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ColumnAddCommand::undo()
{
    Q_ASSERT(column_ != 0);

    // Remove the column from the layout.
    layout_->removeColumn(column_);
    del_ = true;
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ColumnAddCommand::redo()
{
    // If this is the first time, create the column.
    if (column_ == 0)
    {
        layout_->addColumn(name_, contentType_, allowedItems_);
        column_ = layout_->getColumns().back();
    }
    else
    {
        // Otherwise add the existing column to the layout.
        layout_->addColumn(column_);
    }

    del_ = false;
}

//-----------------------------------------------------------------------------
// Function: ConnectionAddCommand()
//-----------------------------------------------------------------------------
ConnectionAddCommand::ConnectionAddCommand(QGraphicsScene* scene, DiagramInterconnection* conn,
                                           QUndoCommand* parent) : QUndoCommand(parent),
                                                                   conn_(conn), mode1_(General::MASTER),
                                                                   mode2_(General::MASTER),
                                                                   portMaps_(), scene_(scene),
                                                                   del_(false)
                                                                   
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
// Function: ~ConnectionAddCommand()
//-----------------------------------------------------------------------------
ConnectionAddCommand::~ConnectionAddCommand()
{
    if (del_)
    {
        delete conn_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ConnectionAddCommand::undo()
{
    // Remove the interconnection from the scene.
    scene_->removeItem(conn_);

    // Disconnect the ends.
    conn_->disconnectEnds();
    del_ = true;
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ConnectionAddCommand::redo()
{
    // Add the back to the scene.
    if (!scene_->items().contains(conn_))
    {
        scene_->addItem(conn_);
    }

    // Connect the ends and set the interface modes and port map for the hierarchical end point.
    if (conn_->connectEnds())
    {
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
    }

    del_ = false;
}
