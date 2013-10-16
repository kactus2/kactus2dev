//-----------------------------------------------------------------------------
// File: PortMapsPhysicalItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 7.10.2013
//
// Description:
// Port maps tree item representing a mapping for one bit in logical port.
//-----------------------------------------------------------------------------

#include "PortMapsBitMapItem.h"

#include "PortMapsTreeModel.h"
#include <models/abstractiondefinition.h>
#include <models/businterface.h>
#include <models/component.h>

//-----------------------------------------------------------------------------
// Function: PortMapsPhysicalItem::PortMapsPhysicalItem()
//-----------------------------------------------------------------------------
PortMapsBitMapItem::PortMapsBitMapItem(PortMapsTreeItem* parent, QSharedPointer<Component> component,
    QString const& physicalName /*= QString()*/)
: PortMapsTreeItem(parent, component, physicalName, PortMapsTreeItem::ITEM_BIT_MAP),
mappings_()
{    
}

//-----------------------------------------------------------------------------
// Function: PortMapsPhysicalItem::~PortMapsPhysicalItem()
//-----------------------------------------------------------------------------
PortMapsBitMapItem::~PortMapsBitMapItem()
{

}

//-----------------------------------------------------------------------------
// Function: PortMapsBitMapItem::refresh()
//-----------------------------------------------------------------------------
void PortMapsBitMapItem::refresh()
{
}

//-----------------------------------------------------------------------------
// Function: PortMapsLogicalItem::data()
//-----------------------------------------------------------------------------
QVariant PortMapsBitMapItem::data(int section) const
{
    if (section < PortMapsTreeModel::COLUMN_COUNT)
    {
        switch (section)
        {
        case PortMapsTreeModel::COLUMN_TREE :
            {
                return getIndex();
            }
        case PortMapsTreeModel::COLUMN_PHYSICAL :
            {
               return getPortConnections();
            }
        }
    }
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortMapsPhysicalItem::addChild()
//-----------------------------------------------------------------------------
void PortMapsBitMapItem::addChild(PortMapsBitMapItem* item)
{
    if (!item)
    {
        return;
    }

    // Physical ports should not have children.
    Q_ASSERT(false);
}

//-----------------------------------------------------------------------------
// Function: PortMapsBitMapItem::addMapping()
//-----------------------------------------------------------------------------
void PortMapsBitMapItem::addMapping(QString const& portName, int index /*= 0*/)
{
    BitMapping toAdd(index, portName);
    if (!mappings_.contains(toAdd))
    {
        mappings_.append(toAdd);
    }
}


//-----------------------------------------------------------------------------
// Function: PortMapsBitMapItem::clearMappings()
//-----------------------------------------------------------------------------
void PortMapsBitMapItem::clearMappings()
{
    mappings_.clear();
}


//-----------------------------------------------------------------------------
// Function: PortMapsBitMapItem::isConnected()
//-----------------------------------------------------------------------------
bool PortMapsBitMapItem::isConnected() const
{
    return mappings_.size() != 0;
}

//-----------------------------------------------------------------------------
// Function: PortMapsPhysicalItem::getPortConnections()
//-----------------------------------------------------------------------------
QString PortMapsBitMapItem::getPortConnections() const
{
    QStringList portList;

    if (mappings_.isEmpty())
    {
        return "unconnected";
    }
    else
    {
        foreach (BitMapping bitMap, mappings_)
        {
            if (component_->getPortWidth(bitMap.physName) == 1)
            {
                portList.append(bitMap.physName);
            }
            else
            {
                portList.append(bitMap.physName + "(" + QString::number(bitMap.physIndex) + ")");
            }
        }
    }

    return portList.join(", ");
}