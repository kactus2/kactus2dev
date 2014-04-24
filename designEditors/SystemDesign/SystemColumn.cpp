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

#include <IPXACTmodels/component.h>

//-----------------------------------------------------------------------------
// Function: SystemColumn()
//-----------------------------------------------------------------------------
SystemColumn::SystemColumn(ColumnDesc const& desc, GraphicsColumnLayout* layout)
    : GraphicsColumn(desc, layout)
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
    // Begin the position update for all connections.
    foreach (QGraphicsItem *item, scene()->items())
    {
        GraphicsConnection* conn = dynamic_cast<GraphicsConnection*>(item);

        if (conn != 0)
        {
            conn->beginUpdatePosition();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemColumn::createMoveUndoCommand()
//-----------------------------------------------------------------------------
QSharedPointer<QUndoCommand> SystemColumn::createMoveUndoCommand()
{
    QSharedPointer<QUndoCommand> cmd = GraphicsColumn::createMoveUndoCommand();

    // End the position update for all connections.
    foreach (QGraphicsItem *item, scene()->items())
    {
        GraphicsConnection* conn = dynamic_cast<GraphicsConnection*>(item);

        if (conn != 0)
        {
            conn->endUpdatePosition(cmd.data());
        }
    }

    return cmd;
}
