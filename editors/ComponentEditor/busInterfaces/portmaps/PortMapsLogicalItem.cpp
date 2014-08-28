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

#include <IPXACTmodels/abstractiondefinition.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/PortMap.h>

//-----------------------------------------------------------------------------
// Function: PortMapsLogicalItem::PortMapsLogicalItem()
//-----------------------------------------------------------------------------
PortMapsLogicalItem::PortMapsLogicalItem(PortMapsTreeItem* parent, 
    QString const& logicalName, 
    QSharedPointer<Component> component, 
    BusInterface* busif,
    QSharedPointer<AbstractionDefinition> absDef)
    : PortMapsTreeItem(parent, component, logicalName, PortMapsTreeItem::ITEM_LOGICAL_PORT),    
    busIf_(busif),
    absDef_(absDef),
    portMaps_(busif->getPortMaps()), 
    right_(0), 
    left_(0),
    beginMapping_(true)
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
        width = absDef_->getPortSize(getName(), busIf_->getInterfaceMode());
    }
    

    // If logical width is not defined in the abs def, find the highest bound in port maps for width.
    if (width == -1)
    {
        foreach (QSharedPointer<PortMap> portMap, portMaps_)
        {   
            if (portMap->logicalPort() == getName())
            {
                General::PortBounds logicalRange = 
                    portMap->getLogicalRange(component_->getPort(portMap->physicalPort()));
                int higherBound = qMax(logicalRange.left_, logicalRange.right_);
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
    foreach (QSharedPointer<PortMap> portMap, portMaps_)
    {
        if (portMap->logicalPort() == getName())
        {
            // Get the port bounds.
            General::PortBounds physicalRange =
                portMap->getPhysicalRange(component_->getPort(portMap->physicalPort()));
            int lowerPhysical = qMin(physicalRange.left_, physicalRange.right_);
            int higherPhysical = qMax(physicalRange.left_, physicalRange.right_);
            General::PortBounds logicalRange = 
                portMap->getLogicalRange(component_->getPort(portMap->physicalPort()));
            int lowerLogical = qMin(logicalRange.left_,logicalRange.right_);
            int higherLogical = qMax(logicalRange.left_,logicalRange.right_);

            // The first mapping defines the logical bounds.
            if (beginMapping_)
            {
                left_ =  logicalRange.left_;
                right_ = logicalRange.right_;   
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
                    child->addMapping(portMap->physicalPort(), physicalIndex);
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
        }
    }
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortMapsLogicalItem::isValid()
//-----------------------------------------------------------------------------
bool PortMapsLogicalItem::isValid() const
{    
    if(!busIf_->getAbstractionType().isValid())
    {
        return false;
    }
            
    QList<General::PortBounds> bounds = component_->getPortBounds();
    foreach (QSharedPointer<PortMap> portMap, portMaps_)
    {
        if (QString::compare(portMap->logicalPort(), getName()) == 0)
        {
            // if the physical port's bounds don't match the actual port size
            if (!portMap->isValid(bounds))
            {
                return false;
            }

            // if abstraction def is not set, logical items are invalid.
            if (absDef_)
            {
                // if port is not defined in abs def.
                if (!absDef_->hasPort(portMap->logicalPort(), busIf_->getInterfaceMode()))
                {
                    return false;
                } 
                // if abstraction def is set but port is not defined as optional or
                // required
                if (!absDef_->isRequired(portMap->logicalPort(), busIf_->getInterfaceMode()) &&
                    !absDef_->isOptional(portMap->logicalPort(), busIf_->getInterfaceMode())) 
                {
                    return false;
                } 
                // if abstraction def is set and logical port is illegal 
                if (absDef_->isIllegal(portMap->logicalPort(), busIf_->getInterfaceMode()))
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
// Function: PortMapsLogicalItem::isValid()
//-----------------------------------------------------------------------------
bool PortMapsLogicalItem::isValid(QStringList& errorList) const
{
    bool valid = true;
    if(!busIf_->getAbstractionType().isValid())
    {
        errorList.append(tr("No abstraction definition set for interface %1.").arg(busIf_->getName()));
        valid = false;
    }

    QList<General::PortBounds> bounds = component_->getPortBounds();
    foreach (QSharedPointer<PortMap> portMap, portMaps_)
    {
        if (QString::compare(portMap->logicalPort(), getName()) == 0)
        {
            // if the physical port's bounds don't match the actual port size
            if (!portMap->isValid(bounds, errorList, "boo"))
            {                
                valid = false;
            }

            // if abstraction def is not set, logical items are invalid.
            if (absDef_)
            {
                // if port is not defined in abs def.
                if (!absDef_->hasPort(portMap->logicalPort(), busIf_->getInterfaceMode()))
                {
                    errorList.append(tr("Port %1 is not defined in abstraction definition.").arg(portMap->logicalPort()));
                    valid = false;
                }    
                // if port is not defined as optional or required
                else if (!absDef_->isRequired(portMap->logicalPort(), busIf_->getInterfaceMode()) &&
                    !absDef_->isOptional(portMap->logicalPort(), busIf_->getInterfaceMode())) 
                {
                    errorList.append(tr("Port %1 is not required or options in abstraction definition.").arg(portMap->logicalPort()));
                    valid = false;
                }
                // if abstraction def is set and logical port is illegal 
                if (absDef_->isIllegal(portMap->logicalPort(), busIf_->getInterfaceMode()))
                {
                    errorList.append(tr("Port %1 is illegal in abstraction definition.").arg(portMap->logicalPort()));
                    valid = false;
                }
            }
        }
    }

    // Finally, check children.
    return PortMapsTreeItem::isValid(errorList) && valid;
}

//-----------------------------------------------------------------------------
// Function: PortMapsLogicalItem::getWidth()
//-----------------------------------------------------------------------------
int PortMapsLogicalItem::getWidth() const
{
    int width = -1;
    if (absDef_)
    {
        width = absDef_->getPortSize(getName(), busIf_->getInterfaceMode());    
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
General::Direction PortMapsLogicalItem::getDirection() const
{
    if (absDef_)
    {
        return absDef_->getPortDirection(getName(), busIf_->getInterfaceMode());
    }
    
    return General::DIRECTION_INVALID;
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
    foreach (QSharedPointer<PortMap> portMap, portMaps_)
    {
        if (portMap->logicalPort() == getName())
        {                        
            if (physPort == portMap->physicalPort())
            {
                continue;
            }

            if (physPort.isEmpty())
            {
                physPort = portMap->physicalPort();
            } 
            else
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
    foreach (QSharedPointer<PortMap> portMap, portMaps_)
    {
        if (portMap->logicalPort() == getName() && portMap->physicalPort() == physPort)
        {
            General::PortBounds logicalRange = 
                portMap->getPhysicalRange(component_->getPort(portMap->physicalPort()));
            left = logicalRange.left_;
            right = logicalRange.right_;
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
            PortMapsBitMapItem* child = new PortMapsBitMapItem(this, component_, busIf_);
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
