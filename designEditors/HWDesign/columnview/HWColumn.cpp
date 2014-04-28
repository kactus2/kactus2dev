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

#include <designEditors/common/DesignDiagram.h>
#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/DiagramUtil.h>
#include <IPXACTmodels/component.h>

//-----------------------------------------------------------------------------
// Function: HWColumn()
//-----------------------------------------------------------------------------
HWColumn::HWColumn(ColumnDesc const& desc, GraphicsColumnLayout* layout)
    : GraphicsColumn(desc, layout)
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
    // Begin position update for all connections.
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
// Function: HWColumn::createMoveUndoCommand()
//-----------------------------------------------------------------------------
QSharedPointer<QUndoCommand> HWColumn::createMoveUndoCommand()
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
