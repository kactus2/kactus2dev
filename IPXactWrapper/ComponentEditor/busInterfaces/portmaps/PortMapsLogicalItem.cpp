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

#include <models/abstractiondefinition.h>
#include <models/businterface.h>
#include <models/component.h>


//-----------------------------------------------------------------------------
// Function: PortMapsLogicalItem::PortMapsLogicalItem()
//-----------------------------------------------------------------------------
PortMapsLogicalItem::PortMapsLogicalItem(PortMapsTreeItem* parent, 
    QString const& logicalName, 
    QSharedPointer<Component> component, 
    BusInterface* busif,
    QSharedPointer<AbstractionDefinition> absDef,
    General::InterfaceMode interfaceMode)
    : PortMapsTreeItem(parent, component, logicalName, PortMapsTreeItem::ITEM_LOGICAL_PORT),
    absDef_(absDef),
    interfaceMode_(interfaceMode),
    busif_(busif),
    portMaps_(busif->getPortMaps()), 
    right_(), 
    left_(),
    beginMapping_(true)
{
    refresh();
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
    int width = absDef_->getPortSize(getName(), interfaceMode_);        

    // If logical width is not defined in the abs def, find the highest bound in port maps for width.
    if (width == -1)
    {
        foreach (QSharedPointer<General::PortMap> portMap, portMaps_)
        {   
            if (portMap->logicalPort_ == getName())
            {
                int higherBound = qMax(portMap->logicalVector_->getLeft(), portMap->logicalVector_->getRight());
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
    foreach (QSharedPointer<General::PortMap> portMap, portMaps_)
    {
        if (portMap->logicalPort_ == getName())
        {

            int lowerPhysical = portMap->physicalVector_->getRight();
            int higherPhysical = portMap->physicalVector_->getLeft();

            int lowerLogical = portMap->logicalVector_->getRight();            
            int higherLogical = portMap->logicalVector_->getLeft();

            // If right bound > left bound, reverse left and right for logical port.
            if (lowerPhysical > portMap->physicalVector_->getLeft())
            {
                lowerPhysical = portMap->physicalVector_->getLeft();
                higherPhysical = portMap->physicalVector_->getRight();

                lowerLogical = portMap->logicalVector_->getLeft();
                higherLogical = portMap->logicalVector_->getRight();
                if (beginMapping_)
                {
                    left_ =  portMap->logicalVector_->getLeft();                    
                }
                else
                {
                    left_ = qMin(left_, portMap->logicalVector_->getLeft());
                }
                right_ = qMax(right_, portMap->logicalVector_->getRight());
            }
            else
            {
                left_ = qMax(left_, portMap->logicalVector_->getLeft());
                if (beginMapping_)
                {
                    right_ = portMap->logicalVector_->getRight();
                }
                else
                {
                    right_ = qMin(right_, portMap->logicalVector_->getRight());
                }
            }

            beginMapping_ = false;
            int physicalIndex = lowerPhysical;
            for (int logicalIndex = lowerLogical; logicalIndex <= higherLogical; logicalIndex++, physicalIndex++)
            {
                if (lowerPhysical <= physicalIndex && physicalIndex <= higherPhysical)
                {
                    PortMapsBitMapItem* child = dynamic_cast<PortMapsBitMapItem*>(getChild(logicalIndex));
                    Q_ASSERT(child);                    
                    child->addMapping(portMap->physicalPort_, physicalIndex);
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
    if (section < PortMapsTreeModel::COLUMN_COUNT)
    {
        switch (section)
        {
        case PortMapsTreeModel::COLUMN_TREE :
            {
                if (left_ == 0 && right_ == 0)
                {
                    return getName();
                }
                return getName() + " (" + QString::number(left_) + ":" + QString::number(right_) + ")";
            }
        case PortMapsTreeModel::COLUMN_PHYSICAL :
            {
                return getPhysPorts();
            }
        case PortMapsTreeModel::COLUMN_WIDTH_LOGICAL :
            {
                return QString::number(getWidth());
            }
        case PortMapsTreeModel::COLUMN_WIDTH_PHYSICAL :
            {
                return QString::number(getConnectionCount());
            }
       /* case PortMapsTreeModel::COLUMN_LEFT_LOGICAL:
            {
                return QString::number(left_);
            }
        case PortMapsTreeModel::COLUMN_RIGHT_LOGICAL:
            {
                return QString::number(right_);
            }
        case PortMapsTreeModel::COLUMN_LEFT_PHYSICAL:
            {
                return QString::number(getWidth());
            }
        case PortMapsTreeModel::COLUMN_RIGHT_PHYSICAL:
            {
                return QString::number(getWidth());
            }*/
        }
    }
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortMapsLogicalItem::getWidth()
//-----------------------------------------------------------------------------
int PortMapsLogicalItem::getWidth() const
{
    int width = absDef_->getPortSize(getName(), interfaceMode_);    

    // If abstraction definition does not define width, use the amount of physical connections.
    if (width == -1)
    {
        return getChildCount();
    }

    return width;
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
    foreach (QSharedPointer<General::PortMap> portMap, portMaps_)
    {
        if (portMap->logicalPort_ == getName())
        {                        
            if (physPort == portMap->physicalPort_)
            {
                continue;
            }

            if (physPort.isEmpty())
            {
                physPort = portMap->physicalPort_;
            } 
            else
            {
                return tr("[multiple]");
            }
        }
    }

    if (physPort.isEmpty())
    {
        return tr("unconnected");
    } 

    int left = 0;
    int right = 0;
    foreach (QSharedPointer<General::PortMap> portMap, portMaps_)
    {
        if (portMap->logicalPort_ == getName() && portMap->physicalPort_ == physPort)
        {
            left = portMap->physicalVector_->getLeft();
            right = portMap->physicalVector_->getRight();
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
            PortMapsBitMapItem* child = new PortMapsBitMapItem(this, component_);
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




