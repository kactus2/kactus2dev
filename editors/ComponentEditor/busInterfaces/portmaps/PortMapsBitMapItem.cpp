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
#include "PortMapsLogicalItem.h"

//-----------------------------------------------------------------------------
// Function: PortMapsPhysicalItem::PortMapsPhysicalItem()
//-----------------------------------------------------------------------------
PortMapsBitMapItem::PortMapsBitMapItem(PortMapsTreeItem* parent, QSharedPointer<Component> component,
    BusInterface* busIf, QString const& physicalName /*= QString()*/)
    : PortMapsTreeItem(parent, component, physicalName, PortMapsTreeItem::ITEM_BIT_MAP),
    busIf_(busIf),
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
// Function: PortMapsBitMapItem::isValid()
//-----------------------------------------------------------------------------
bool PortMapsBitMapItem::isValid() const
{    
    // if abstraction def is not set, bit map items are invalid.
    if (!busIf_->getAbstractionType().isValid())
    {
        return false;
    }

    if (isConnected())
    {
        PortMapsLogicalItem* parent = dynamic_cast<PortMapsLogicalItem*>(getParent());
        if (parent)
        {
            // if port directions for any of the mapped physical ports do not match, the item is invalid.
            General::Direction logDir = parent->getDirection();
            foreach (BitMapping bitMap, mappings_)
            {
                General::Direction physDir =  component_->getPortDirection(bitMap.physName);
                if (logDir != General::INOUT &&
                    physDir != General::INOUT &&
                    physDir != logDir) 
                {
                    return false;
                }         
            }
        }
    }
    
    // Bit map items have no children. No need to check them.
    return true;
}

//-----------------------------------------------------------------------------
// Function: PortMapsBitMapItem::isValid()
//-----------------------------------------------------------------------------
bool PortMapsBitMapItem::isValid(QStringList& errorList) const
{
    bool valid = true;

    // if abstraction def is not set, bit map items are invalid.
    if (!busIf_->getAbstractionType().isValid())
    {
        // Missing abs def is reported on logical signal level.
        valid = false;
    }

    if (isConnected())
    {
        PortMapsLogicalItem* parent = dynamic_cast<PortMapsLogicalItem*>(getParent());
        if (parent)
        {
            // if port directions for any of the mapped physical ports do not match, the item is invalid.
            General::Direction logDir = parent->getDirection();
            foreach (BitMapping bitMap, mappings_)
            {
                General::Direction physDir =  component_->getPortDirection(bitMap.physName);
                if (logDir != General::INOUT &&
                    physDir != General::INOUT &&
                    physDir != logDir) 
                {
                    QString error = tr("Direction between logical port %1 and physical port %2 did not match.");
                    errorList.append(error.arg(parent->getName(), bitMap.physName));
                    valid = false;
                }
            }
        }
    }

    return valid;
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
