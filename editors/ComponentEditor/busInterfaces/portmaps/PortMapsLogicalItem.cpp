//-----------------------------------------------------------------------------
// File: PortMapsLogicalItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 7.10.2013
//
// Description:
// Port maps tree item representing a logical port.
//-----------------------------------------------------------------------------

#include "PortMapsLogicalItem.h"
#include "PortMapsBitMapItem.h"
#include "PortMapsTreeModel.h"

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/PortMap.h>

//-----------------------------------------------------------------------------
// Function: PortMapsLogicalItem::PortMapsLogicalItem()
//-----------------------------------------------------------------------------
PortMapsLogicalItem::PortMapsLogicalItem(PortMapsTreeItem* parent, QString const& logicalName,
    QSharedPointer<Component> component, QSharedPointer<BusInterface> busif,
    QSharedPointer<AbstractionDefinition> absDef, QSharedPointer<ExpressionParser> expressionParser ):
PortMapsTreeItem(parent, component, logicalName, PortMapsTreeItem::ITEM_LOGICAL_PORT),
busIf_(busif),
absDef_(absDef),
portMaps_(busif->getPortMaps()),
right_(0),
left_(0),
beginMapping_(true),
expressionParser_(expressionParser)
{

}

//-----------------------------------------------------------------------------
// Function: PortMapsLogicalItem::~PortMapsLogicalItem()
//-----------------------------------------------------------------------------
PortMapsLogicalItem::~PortMapsLogicalItem()
{
}


//-----------------------------------------------------------------------------
// Function: PortMapsLogicalItem::refresh()
//-----------------------------------------------------------------------------
void PortMapsLogicalItem::refresh()
{        
    int width = -1;
    if (absDef_)
    {
        width = getPortSize(name(), busIf_->getInterfaceMode());
    }

    // If logical width is not defined in the abs def, find the highest bound in port maps for width.
    if (width == -1)
    {
        foreach (QSharedPointer<PortMap> portMap, *portMaps_)
        {
            if (portMap->getLogicalPort()->name_ == name())
            {
                int rangeLeft =
                    expressionParser_->parseExpression(portMap->getLogicalPort()->range_->getLeft()).toInt();
                int rangeRight =
                    expressionParser_->parseExpression(portMap->getLogicalPort()->range_->getRight()).toInt();
                int higherBound = qMax(rangeLeft, rangeRight);
                width = qMax(width, higherBound);
            }
        }

        width++;
    }

    // Adjust width if needed.
    if (width == 0)
    {
        return;
    }

    clearMappings();
    updateWidthTo(width);       

    // Create new mappings for childs.
    foreach (QSharedPointer<PortMap> portMap, *portMaps_)
    {
        if (portMap->getLogicalPort()->name_ == name())
        {
            // Get the port bounds.
            int physicalRangeLeft = expressionParser_->parseExpression(
                portMap->getPhysicalPort()->partSelect_->getLeftRange()).toInt();
            int physicalRangeRight = expressionParser_->parseExpression(
                portMap->getPhysicalPort()->partSelect_->getRightRange()).toInt();
            int lowerPhysical = qMin(physicalRangeLeft, physicalRangeRight);
            int higherPhysical = qMax(physicalRangeLeft, physicalRangeRight);

            int logicalRangeLeft =
                expressionParser_->parseExpression(portMap->getLogicalPort()->range_->getLeft()).toInt();
            int logicalRangeRight =
                expressionParser_->parseExpression(portMap->getLogicalPort()->range_->getRight()).toInt();
            int lowerLogical = qMin(logicalRangeLeft, logicalRangeRight);
            int higherLogical = qMax(logicalRangeLeft, logicalRangeRight);

            // The first mapping defines the logical bounds.
            if (beginMapping_)
            {
                left_ = logicalRangeLeft;
                right_ = logicalRangeRight;
                beginMapping_ = false;                 
            }
            else if ( left_ <= right_)
            {
                left_ = qMin(left_, lowerLogical);
                right_ = qMax(right_, higherLogical);                    
            }
            else
            {
                left_ = qMax(left_, higherLogical);
                right_ = qMin(right_, lowerLogical);                
            }

            // Update mappings to children.
            int physicalIndex = lowerPhysical;
            for (int logicalIndex = lowerLogical; logicalIndex <= higherLogical; logicalIndex++, physicalIndex++)
            {
                if (lowerPhysical <= physicalIndex && physicalIndex <= higherPhysical)
                {
                    PortMapsBitMapItem* child = dynamic_cast<PortMapsBitMapItem*>(getChild(logicalIndex));
                    Q_ASSERT(child);                    
                    child->addMapping(portMap->getPhysicalPort()->name_, physicalIndex);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapsLogicalItem::data()
//-----------------------------------------------------------------------------
QVariant PortMapsLogicalItem::data(int section) const
{

    if (section == PortMapsTreeModel::COLUMN_TREE)
    {
        if (left_ == 0 && right_ == 0)
        {
            return name();
        }

        return name() + " (" + QString::number(left_) + ":" + QString::number(right_) + ")";
    }
    else if (section == PortMapsTreeModel::COLUMN_PHYSICAL)
    {
        return getPhysPorts();
    }
    else if (section == PortMapsTreeModel::COLUMN_WIDTH_LOGICAL)
    {
        return QString::number(getWidth());
    }
    else if (section == PortMapsTreeModel::COLUMN_WIDTH_PHYSICAL)
    {
        return QString::number(getConnectionCount());
    }    

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortMapsLogicalItem::isValid()
//-----------------------------------------------------------------------------
bool PortMapsLogicalItem::isValid() const
{                
    foreach (QSharedPointer<PortMap> portMap, *portMaps_)
    {
        if (QString::compare(portMap->getLogicalPort()->name_, name()) == 0)
        {
            // if abstraction def is not set, logical items are invalid.
            if (absDef_)
            {
                // if port is not defined in abs def.
                if (!absDef_->hasPort(portMap->getLogicalPort()->name_, busIf_->getInterfaceMode()))
                {
                    return false;
                } 

                General::Presence portPresence = absDef_->getPort(portMap->getLogicalPort()->name_)->getPresence(busIf_->getInterfaceMode());
                // if abstraction def is set but port is not defined as optional or required
                if (!portPresence != General::REQUIRED && portPresence != General::OPTIONAL)
                {
                    return false;
                }
                // if abstraction def is set and logical port is illegal 
                if (portPresence == General::ILLEGAL)
                {
                    return false;
                }
            }
        }
    }

    // Finally, check children.
    return PortMapsTreeItem::isValid();
}

//-----------------------------------------------------------------------------
// Function: PortMapsLogicalItem::getWidth()
//-----------------------------------------------------------------------------
int PortMapsLogicalItem::getWidth() const
{
    int width = -1;
    if (absDef_)
    {
        width = getPortSize(name(), busIf_->getInterfaceMode());
    }
  

    // If abstraction definition does not define width, use the amount of physical connections.
    if (width == -1)
    {
        return getChildCount();
    }

    return width;
}

//-----------------------------------------------------------------------------
// Function: PortMapsLogicalItem::getDirection()
//-----------------------------------------------------------------------------
DirectionTypes::Direction PortMapsLogicalItem::getDirection() const
{
    if (absDef_)
    {
        return absDef_->getPortDirection(name(), busIf_->getInterfaceMode());
    }
    
    return DirectionTypes::DIRECTION_INVALID;
}

//-----------------------------------------------------------------------------
// Function: PortMapsLogicalItem::getConnectionCount()
//-----------------------------------------------------------------------------
int PortMapsLogicalItem::getConnectionCount() const
{
    int connected = 0;
    for (int i = 0; i < getChildCount(); i++)
    {
        PortMapsBitMapItem const* child = dynamic_cast<PortMapsBitMapItem const*>(getChild(i));
        if (child->isConnected())
        {
            connected++;
        }
    }

    return connected;
}

//-----------------------------------------------------------------------------
// Function: PortMapsLogicalItem::getPhysPorts()
//-----------------------------------------------------------------------------
QString PortMapsLogicalItem::getPhysPorts() const
{
    QString physPort = "";
    foreach (QSharedPointer<PortMap> portMap, *portMaps_)
    {
        if (portMap->getLogicalPort()->name_ == name())
        {                        
            if (physPort.isEmpty())
            {
                physPort = portMap->getPhysicalPort()->name_;
            } 
            else if (physPort != portMap->getPhysicalPort()->name_)
            {
                return tr("[multiple]");
            }
        }
    }

    if (physPort.isEmpty())
    {
        Q_ASSERT(false);
        return tr("unconnected");
    } 

    int left = 0;
    int right = 0;
    foreach (QSharedPointer<PortMap> portMap, *portMaps_)
    {
        if (portMap->getLogicalPort()->name_ == name() && portMap->getPhysicalPort()->name_ == physPort)
        {
            QSharedPointer<PartSelect> logicalRange = portMap->getPhysicalPort()->partSelect_;
            left = expressionParser_->parseExpression(logicalRange->getLeftRange()).toInt();
            right = expressionParser_->parseExpression(logicalRange->getRightRange()).toInt();
            break;
        }
    }

    if (left == 0 && right == 0)
    {
        return physPort;
    }
    return physPort + " (" + QString::number(left) + ":" + QString::number(right) + ")";
}

//-----------------------------------------------------------------------------
// Function: PortMapsLogicalItem::updateWidthTo()
//-----------------------------------------------------------------------------
void PortMapsLogicalItem::updateWidthTo(int width)
{
    if (getChildCount() < width)
    {
        while(getChildCount() < width)
        {
            PortMapsBitMapItem* child = new PortMapsBitMapItem(this, component_, busIf_, expressionParser_);
            addChild(child);
        }
    }
    else if (getChildCount() > width)
    {
        while(getChildCount() > width)
        {
            removeChild(getChild(getChildCount() - 1));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapsLogicalItem::clearMappings()
//-----------------------------------------------------------------------------
void PortMapsLogicalItem::clearMappings()
{
    for (int i = 0; i < getChildCount(); i++)
    {
        PortMapsBitMapItem* child = dynamic_cast<PortMapsBitMapItem*>(getChild(i));
        Q_ASSERT(child);
        child->clearMappings();
    }

    beginMapping_ = true;
    left_ = 0;
    right_ = 0;
}

//-----------------------------------------------------------------------------
// Function: PortMapsLogicalItem::getPortSize()
//-----------------------------------------------------------------------------
int PortMapsLogicalItem::getPortSize(QString const& portName, General::InterfaceMode interfaceMode) const
{
    QSharedPointer<PortAbstraction> port = absDef_->getPort(portName);
    if (port->getWire())
    {
        return expressionParser_->parseExpression(port->getWire()->getWidth(interfaceMode)).toInt();
    }

    return -1;
}