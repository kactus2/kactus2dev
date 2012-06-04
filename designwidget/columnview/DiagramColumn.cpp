//-----------------------------------------------------------------------------
// File: DiagramColumn.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.4.2011
//
// Description:
// Diagram column class.
//-----------------------------------------------------------------------------

#include "DiagramColumn.h"

#include "../DiagramMoveCommands.h"
#include "../diagraminterface.h"
#include "../diagramcomponent.h"
#include "../diagraminterconnection.h"
#include "../DiagramAdHocInterface.h"
#include "../diagramport.h"

#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/GenericEditProvider.h>
#include <common/layouts/VStackedLayout.h>
#include <common/layouts/VCollisionLayout.h>

#include <common/DesignDiagram.h>
#include <common/diagramgrid.h>
#include <common/DiagramUtil.h>
#include <models/component.h>

//-----------------------------------------------------------------------------
// Function: DiagramColumn()
//-----------------------------------------------------------------------------
DiagramColumn::DiagramColumn(ColumnDesc const& desc, GraphicsColumnLayout* layout, QGraphicsScene* scene)
    : GraphicsColumn(desc, layout, scene),
      conns_()
{
}

//-----------------------------------------------------------------------------
// Function: ~DiagramColumn()
//-----------------------------------------------------------------------------
DiagramColumn::~DiagramColumn()
{
}

//-----------------------------------------------------------------------------
// Function: DiagramColumn::isItemAllowed()
//-----------------------------------------------------------------------------
bool DiagramColumn::isItemAllowed(QGraphicsItem* item, unsigned int allowedItems) const
{
    switch (item->type())
    {
    case DiagramInterface::Type:
    case DiagramAdHocInterface::Type:
        {
            return (allowedItems & CIT_INTERFACE);
        }

    case DiagramComponent::Type:
        {
            QSharedPointer<Component> comp = qgraphicsitem_cast<DiagramComponent*>(item)->componentModel();

            // Check if this is a packaged component (and has a strict type).
            if (comp->getVlnv()->isValid())
            {
                return ((comp->isBridge() && (allowedItems & CIT_BRIDGE)) ||
                    (comp->isChannel() && (allowedItems & CIT_CHANNEL)) ||
                    (!comp->isBus() && (allowedItems & CIT_COMPONENT)));
            }
            else
            {
                // Otherwise this is an unpacked component and can be of any type.
                return (allowedItems & (CIT_BRIDGE | CIT_CHANNEL | CIT_COMPONENT));
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: DiagramColumn::prepareColumnMove()
//-----------------------------------------------------------------------------
void DiagramColumn::prepareColumnMove()
{
    // Begin position update for the interconnections.
    foreach (QGraphicsItem* item, getItems())
    {
        if (item->type() == DiagramComponent::Type)
        {
            DiagramComponent* comp = static_cast<DiagramComponent*>(item);

            foreach (QGraphicsItem* childItem, comp->childItems())
            {
                if (childItem->type() == DiagramPort::Type)
                {
                    beginUpdateConnPositions(static_cast<DiagramConnectionEndpoint*>(childItem));
                }
            }
        }
        else if (item->type() == DiagramInterface::Type)
        {
            beginUpdateConnPositions(static_cast<DiagramConnectionEndpoint*>(item));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DiagramColumn::createMoveUndoCommand()
//-----------------------------------------------------------------------------
QSharedPointer<QUndoCommand> DiagramColumn::createMoveUndoCommand()
{
    QSharedPointer<QUndoCommand> cmd = GraphicsColumn::createMoveUndoCommand();

    // End position update for the interconnections.
    foreach (DiagramInterconnection* conn, conns_)
    {
        conn->endUpdatePosition(cmd.data());
    }

    conns_.clear();

    return cmd;
}

//-----------------------------------------------------------------------------
// Function: beginUpdateConnPositions()
//-----------------------------------------------------------------------------
void DiagramColumn::beginUpdateConnPositions(DiagramConnectionEndpoint* endPoint)
{
    foreach (DiagramInterconnection* conn, endPoint->getInterconnections())
    {
        if (!conns_.contains(conn))
        {
            conn->beginUpdatePosition();
            conns_.insert(conn);
        }
    }
}