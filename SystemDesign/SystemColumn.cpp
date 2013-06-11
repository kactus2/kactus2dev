//-----------------------------------------------------------------------------
// File: SystemColumn.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 31.5.2011
//
// Description:
// System column class for managing SW mapping components in a system design.
//-----------------------------------------------------------------------------

#include "SystemColumn.h"

#include "SWComponentItem.h"
#include "HWMappingItem.h"
#include "SWInterfaceItem.h"

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>

#include <models/component.h>

//-----------------------------------------------------------------------------
// Function: SystemColumn()
//-----------------------------------------------------------------------------
SystemColumn::SystemColumn(ColumnDesc const& desc, GraphicsColumnLayout* layout)
    : GraphicsColumn(desc, layout),
      conns_()
{
}

//-----------------------------------------------------------------------------
// Function: ~SystemColumn()
//-----------------------------------------------------------------------------
SystemColumn::~SystemColumn()
{
}

//-----------------------------------------------------------------------------
// Function: SystemColumn::isItemAllowed()
//-----------------------------------------------------------------------------
bool SystemColumn::isItemAllowed(QGraphicsItem* item, unsigned int allowedItems) const
{
    switch (item->type())
    {
    case HWMappingItem::Type:
    case SWComponentItem::Type:
        {
            return (allowedItems & CIT_COMPONENT);
        }

    case SWInterfaceItem::Type:
        {
            return (allowedItems & CIT_INTERFACE);
        }

    default:
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: SystemColumn::prepareColumnMove()
//-----------------------------------------------------------------------------
void SystemColumn::prepareColumnMove()
{
    // Begin position update for the connections.
    foreach (QGraphicsItem* item, getItems())
    {
        foreach (QGraphicsItem* childItem, item->childItems())
        {
            if (childItem->type() == SWComponentItem::Type)
            {
                foreach (QGraphicsItem* childItem2, childItem->childItems())
                {
                    if (childItem2->type() == SWPortItem::Type)
                    {
                        SWConnectionEndpoint* endpoint = static_cast<SWConnectionEndpoint*>(childItem2);

                        foreach (GraphicsConnection* conn, endpoint->getConnections())
                        {
                            if (!conns_.contains(conn))
                            {
                                conn->beginUpdatePosition();
                                conns_.insert(conn);
                            }
                        }
                    }
                }
            }
            else if (dynamic_cast<SWConnectionEndpoint*>(childItem))
            {
                SWConnectionEndpoint* endpoint = static_cast<SWConnectionEndpoint*>(childItem);

                foreach (GraphicsConnection* conn, endpoint->getConnections())
                {
                    if (!conns_.contains(conn))
                    {
                        conn->beginUpdatePosition();
                        conns_.insert(conn);
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemColumn::createMoveUndoCommand()
//-----------------------------------------------------------------------------
QSharedPointer<QUndoCommand> SystemColumn::createMoveUndoCommand()
{
    QSharedPointer<QUndoCommand> cmd = GraphicsColumn::createMoveUndoCommand();

    // End position update for the connections.
    foreach (GraphicsConnection* conn, conns_)
    {
        conn->endUpdatePosition(cmd.data());
    }

    conns_.clear();

    return cmd;
}
