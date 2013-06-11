//-----------------------------------------------------------------------------
// File: HWColumn.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.4.2011
//
// Description:
// Diagram column class.
//-----------------------------------------------------------------------------

#include "HWColumn.h"

#include "../HWMoveCommands.h"
#include "../BusInterfaceItem.h"
#include "../HWComponentItem.h"
#include "../HWConnection.h"
#include "../AdHocInterfaceItem.h"
#include "../BusPortItem.h"

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/GenericEditProvider.h>
#include <common/layouts/VStackedLayout.h>
#include <common/layouts/VCollisionLayout.h>

#include <common/DesignDiagram.h>
#include <common/diagramgrid.h>
#include <common/DiagramUtil.h>
#include <models/component.h>

//-----------------------------------------------------------------------------
// Function: HWColumn()
//-----------------------------------------------------------------------------
HWColumn::HWColumn(ColumnDesc const& desc, GraphicsColumnLayout* layout)
    : GraphicsColumn(desc, layout),
      conns_()
{
}

//-----------------------------------------------------------------------------
// Function: ~HWColumn()
//-----------------------------------------------------------------------------
HWColumn::~HWColumn()
{
}

//-----------------------------------------------------------------------------
// Function: HWColumn::isItemAllowed()
//-----------------------------------------------------------------------------
bool HWColumn::isItemAllowed(QGraphicsItem* item, unsigned int allowedItems) const
{
    switch (item->type())
    {
    case BusInterfaceItem::Type:
    case AdHocInterfaceItem::Type:
        {
            return (allowedItems & CIT_INTERFACE);
        }

    case HWComponentItem::Type:
        {
            QSharedPointer<Component> comp = qgraphicsitem_cast<HWComponentItem*>(item)->componentModel();

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
// Function: HWColumn::prepareColumnMove()
//-----------------------------------------------------------------------------
void HWColumn::prepareColumnMove()
{
    // Begin position update for the interconnections.
    foreach (QGraphicsItem* item, getItems())
    {
        if (item->type() == HWComponentItem::Type)
        {
            HWComponentItem* comp = static_cast<HWComponentItem*>(item);

            foreach (QGraphicsItem* childItem, comp->childItems())
            {
                if (childItem->type() == BusPortItem::Type)
                {
                    beginUpdateConnPositions(static_cast<HWConnectionEndpoint*>(childItem));
                }
            }
        }
        else if (item->type() == BusInterfaceItem::Type)
        {
            beginUpdateConnPositions(static_cast<HWConnectionEndpoint*>(item));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWColumn::createMoveUndoCommand()
//-----------------------------------------------------------------------------
QSharedPointer<QUndoCommand> HWColumn::createMoveUndoCommand()
{
    QSharedPointer<QUndoCommand> cmd = GraphicsColumn::createMoveUndoCommand();

    // End position update for the interconnections.
    foreach (GraphicsConnection* conn, conns_)
    {
        conn->endUpdatePosition(cmd.data());
    }

    conns_.clear();

    return cmd;
}

//-----------------------------------------------------------------------------
// Function: beginUpdateConnPositions()
//-----------------------------------------------------------------------------
void HWColumn::beginUpdateConnPositions(HWConnectionEndpoint* endpoint)
{
    foreach (GraphicsConnection* conn, endpoint->getConnections())
    {
        if (!conns_.contains(conn))
        {
            conn->beginUpdatePosition();
            conns_.insert(conn);
        }
    }
}