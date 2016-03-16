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
#include <IPXACTmodels/common/PresenceTypes.h>

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
    clearMappings();
    updateWidth();       

    // Create new mappings for children.
    foreach (QSharedPointer<PortMap> portMap, *portMaps_)
    {
        if (portMap->getLogicalPort()->name_ == name())
        {
            QPair<int, int> physicalBounds = findPhysicalBounds(portMap);
            int lowerPhysical = qMin(physicalBounds.first, physicalBounds.second);
            int higherPhysical = qMax(physicalBounds.first, physicalBounds.second);

            QPair<int, int> logicalBounds = findLogicalBounds(portMap);
            int logicalRangeLeft = logicalBounds.first;
            int logicalRangeRight = logicalBounds.second;
            int lowerLogical = qMin(logicalRangeLeft, logicalRangeRight);
            int higherLogical = qMax(logicalRangeLeft, logicalRangeRight);

            // Update mappings to children.
            QString physicalPort = portMap->getPhysicalPort()->name_;
            int physicalIndex = lowerPhysical;
            for (int logicalIndex = lowerLogical; logicalIndex <= higherLogical; logicalIndex++, physicalIndex++)
            {
                if (lowerPhysical <= physicalIndex && physicalIndex <= higherPhysical)
                {
                    PortMapsBitMapItem* child = dynamic_cast<PortMapsBitMapItem*>(getChild(logicalIndex));
                    if (child)
                    {
                        child->addMapping(physicalPort, physicalIndex);
                    }
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

                PresenceTypes::Presence portPresence =
                    absDef_->getPort(portMap->getLogicalPort()->name_)->getPresence(busIf_->getInterfaceMode());

                // if abstraction def is set and logical port is illegal 
                if (portPresence == PresenceTypes::ILLEGAL)
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
    return abs(left_ - right_) + 1;
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

    left_ = 0;
    right_ = 0;
}

//-----------------------------------------------------------------------------
// Function: PortMapsLogicalItem::updateWidthTo()
//-----------------------------------------------------------------------------
void PortMapsLogicalItem::updateWidth()
{
    updateLogicalBoundaries();

    int width = getWidth();

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
// Function: PortMapsLogicalItem::updateLogicalBoundaries()
//-----------------------------------------------------------------------------
void PortMapsLogicalItem::updateLogicalBoundaries()
{
    if (absDef_)
    {
        QSharedPointer<PortAbstraction> port = absDef_->getPort(name());
        if (port && port->getWire())
        {
            QString widthExpression = port->getWire()->getWidth(busIf_->getInterfaceMode());
            if (!widthExpression.isEmpty())
            {
                int width = expressionParser_->parseExpression(widthExpression).toInt();
                left_ = qMax(0, width - 1);
                right_ = 0;
                return;
            }
        }
    }

    foreach (QSharedPointer<PortMap> portMap, *portMaps_)
    {
        if (portMap->getLogicalPort() && portMap->getLogicalPort()->name_ == name())
        {
            QPair<int, int> logicalBounds = findLogicalBounds(portMap);

            if (logicalBounds.first >= logicalBounds.second)
            {
                left_ = qMax(left_, logicalBounds.first);
                right_ = qMin(right_, logicalBounds.second);
            }
            else
            {
                left_ = qMin(left_, logicalBounds.first);
                right_ = qMax(right_, logicalBounds.second);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PortMapsLogicalItem::findLogicalBounds()
//-----------------------------------------------------------------------------
QPair<int, int> PortMapsLogicalItem::findLogicalBounds(QSharedPointer<PortMap> portMap) const
{
    if (portMap->getLogicalPort()->range_)
    {
        int leftBound = expressionParser_->parseExpression(portMap->getLogicalPort()->range_->getLeft()).toInt();
        int rightBound = expressionParser_->parseExpression(portMap->getLogicalPort()->range_->getRight()).toInt();
        return qMakePair(leftBound, rightBound);
    }
    else if (component_->hasPort(portMap->getPhysicalPort()->name_))
    {
        int leftBound = expressionParser_->parseExpression(
            component_->getPort(portMap->getPhysicalPort()->name_)->getLeftBound()).toInt();
        int rightBound = expressionParser_->parseExpression( 
            component_->getPort(portMap->getPhysicalPort()->name_)->getRightBound()).toInt();
        return qMakePair(abs(leftBound - rightBound), 0);
    }

    return qMakePair(0, 0);
}

//-----------------------------------------------------------------------------
// Function: PortMapsLogicalItem::findPhysicalBounds()
//-----------------------------------------------------------------------------
QPair<int, int> PortMapsLogicalItem::findPhysicalBounds(QSharedPointer<PortMap> portMap) const
{
    int leftBound = 0;
    int rightBound = 0;

    if (portMap->getPhysicalPort() && portMap->getPhysicalPort()->partSelect_)
    {
        leftBound = expressionParser_->parseExpression(
            portMap->getPhysicalPort()->partSelect_->getLeftRange()).toInt();
        rightBound = expressionParser_->parseExpression(
            portMap->getPhysicalPort()->partSelect_->getRightRange()).toInt();
    }
    else if (component_->hasPort(portMap->getPhysicalPort()->name_))
    {
        leftBound = expressionParser_->parseExpression( 
            component_->getPort(portMap->getPhysicalPort()->name_)->getLeftBound()).toInt();
        rightBound = expressionParser_->parseExpression( 
            component_->getPort(portMap->getPhysicalPort()->name_)->getRightBound()).toInt();
    }

    return qMakePair(leftBound, rightBound);
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
    int physicalLeft = 0;
    int physicalRight = 0;
    foreach (QSharedPointer<PortMap> portMap, *portMaps_)
    {
        if (portMap->getLogicalPort()->name_ == name())
        {                        
            if (physPort.isEmpty())
            {
                physPort = portMap->getPhysicalPort()->name_;

                QPair<int, int> physicalBounds = findPhysicalBounds(portMap);
                physicalLeft = physicalBounds.first;
                physicalRight = physicalBounds.second;
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

    if (physicalLeft == 0 && physicalRight == 0)
    {
        return physPort;
    }
    return physPort + " (" + QString::number(physicalLeft) + ":" + QString::number(physicalRight) + ")";
}

