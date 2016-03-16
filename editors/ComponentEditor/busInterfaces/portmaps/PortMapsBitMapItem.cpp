//-----------------------------------------------------------------------------
// File: PortMapsBitMapItem.cpp
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

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Port.h>

#include <IPXACTmodels/common/DirectionTypes.h>

#include "PortMapsLogicalItem.h"

//-----------------------------------------------------------------------------
// Function: PortMapsBitMapItem::PortMapsBitMapItem()
//-----------------------------------------------------------------------------
PortMapsBitMapItem::PortMapsBitMapItem(PortMapsTreeItem* parent, QSharedPointer<Component> component,
    QSharedPointer<BusInterface> busIf, QSharedPointer<ExpressionParser> expressionParser,
    QString const& physicalName):
PortMapsTreeItem(parent, component, physicalName, PortMapsTreeItem::ITEM_BIT_MAP),
    busIf_(busIf),
    mappings_(),
    expressionParser_(expressionParser)
{    
}

//-----------------------------------------------------------------------------
// Function: PortMapsBitMapItem::~PortMapsBitMapItem()
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
        if (section == PortMapsTreeModel::COLUMN_TREE)
        {
            return getIndex();
        }
        else if (section == PortMapsTreeModel::COLUMN_PHYSICAL)
        {
            return getPortConnections();
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
    if (!busIf_->getAbstractionTypes() || busIf_->getAbstractionTypes()->isEmpty())
    {
        return false;
    }

    if (isConnected())
    {
        PortMapsLogicalItem* parent = dynamic_cast<PortMapsLogicalItem*>(getParent());
        if (parent)
        {
            // if port directions for any of the mapped physical ports do not match, the item is invalid.
            DirectionTypes::Direction logDir = parent->getDirection();
            foreach (BitMapping bitMap, mappings_)
            {
                QSharedPointer<Port> physicalPort = component_->getPort(bitMap.physName);
                if (!physicalPort)
                {
                    return false;
                }

                DirectionTypes::Direction physDir =  physicalPort->getDirection();
                if (logDir != DirectionTypes::INOUT && physDir != DirectionTypes::INOUT && physDir != logDir) 
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
// Function: PortMapsPhysicalItem::addChild()
//-----------------------------------------------------------------------------
void PortMapsBitMapItem::addChild(PortMapsBitMapItem* /*item*/)
{
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
    return !mappings_.isEmpty();
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
            if (getPortWidth(bitMap.physName) == 1)
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

//-----------------------------------------------------------------------------
// Function: PortMapsBitMapItem::getPortWidth()
//-----------------------------------------------------------------------------
int PortMapsBitMapItem::getPortWidth(QString const& portName) const
{
    QSharedPointer<Port> targetPort = component_->getPort(portName);
    if (!targetPort)
    {
        return 0;
    }

    int portLeft = expressionParser_->parseExpression(targetPort->getLeftBound()).toInt();
    int portRight = expressionParser_->parseExpression(targetPort->getRightBound()).toInt();

    return abs(portLeft - portRight) + 1;
}
