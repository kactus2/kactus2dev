//-----------------------------------------------------------------------------
// File: AddressEntry.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 01.09.2012
//
// Description:
// Address entry class.
//-----------------------------------------------------------------------------

#include "AddressEntry.h"

#include <common/DesignDiagram.h>
#include <common/GenericEditProvider.h>
#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsConnection.h>

#include <designwidget/BusPortItem.h>
#include <designwidget/HWChangeCommands.h>

#include <models/businterface.h>
#include <models/slaveinterface.h>
#include <models/memorymap.h>

//-----------------------------------------------------------------------------
// Function: AddressEntry::AddressEntry()
//-----------------------------------------------------------------------------
AddressEntry::AddressEntry(ComponentItem* component, BusPortItem* port)
    : component_(component),
      port_(port),
      connectedPort_(0),
      range_(0),
      baseEndAddress_(0),
      aub_(0),
      editProvider_(static_cast<DesignDiagram*>(component->scene())->getEditProvider())
{
    if (port_->isConnected())
    {
        // Determine the connected port.
        GraphicsConnection const* conn = port_->getConnections().first();

        if (conn->endpoint1() != port_)
        {
            connectedPort_ = conn->endpoint1();
        }
        else
        {
            connectedPort_ = conn->endpoint2();
        }

        // Retrieve the size of the memory map.
        if (hasValidConnection())
        {
            ComponentItem* connectedComp = connectedPort_->encompassingComp();
            QString mapName = connectedPort_->getBusInterface()->getSlave()->getMemoryMapRef();
            MemoryMap const* map = connectedComp->componentModel()->getMemoryMap(mapName);

            if (map != 0)
            {
                range_ = map->getLastAddress() + 1;
                aub_ = map->getAddressUnitBits();
                baseEndAddress_ = range_ * aub_ / 8 - 1;
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressEntry::setStartAddress()
//-----------------------------------------------------------------------------
void AddressEntry::setStartAddress(unsigned int startAddress)
{
    QMap<QString, QString> elements = component_->getConfigurableElements();
    elements.insert(port_->name() + "_addr_start", QString::number(startAddress));
    elements.insert(port_->name() + "_addr_end", QString::number(startAddress + baseEndAddress_));

    QSharedPointer<QUndoCommand> cmd(new ComponentConfElementChangeCommand(component_, elements));
    editProvider_.addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: AddressEntry::setLocked()
//-----------------------------------------------------------------------------
void AddressEntry::setLocked(bool locked)
{
    QMap<QString, QString> elements = component_->getConfigurableElements();

    if (locked)
    {
        elements.insert(port_->name() + "_addr_locked", "true");
    }
    else
    {
        elements.insert(port_->name() + "_addr_locked", "false");
    }

    QSharedPointer<QUndoCommand> cmd(new ComponentConfElementChangeCommand(component_, elements));
    editProvider_.addCommand(cmd);
}

//-----------------------------------------------------------------------------
// Function: AddressEntry::getStartAddress()
//-----------------------------------------------------------------------------
unsigned int AddressEntry::getStartAddress() const
{
    QMap<QString, QString> const& elements = component_->getConfigurableElements();
    return elements.value(port_->name() + "_addr_start", "0").toUInt();
}

//-----------------------------------------------------------------------------
// Function: AddressEntry::getEndAddress()
//-----------------------------------------------------------------------------
unsigned int AddressEntry::getEndAddress() const
{
    return getStartAddress() + baseEndAddress_;
}

//-----------------------------------------------------------------------------
// Function: AddressEntry::isLocked()
//-----------------------------------------------------------------------------
bool AddressEntry::isLocked() const
{
    QMap<QString, QString> const& elements = component_->getConfigurableElements();
    return elements.value(port_->name() + "_addr_locked", "false") == "true";
}

//-----------------------------------------------------------------------------
// Function: AddressEntry::getRange()
//-----------------------------------------------------------------------------
unsigned int AddressEntry::getRange() const
{
    return range_;
}

//-----------------------------------------------------------------------------
// Function: AddressEntry::getInterfaceName()
//-----------------------------------------------------------------------------
QString AddressEntry::getInterfaceName() const
{
    return component_->name() + "." + port_->name();
}

//-----------------------------------------------------------------------------
// Function: AddressEntry::getMemoryMapName()
//-----------------------------------------------------------------------------
QString AddressEntry::getMemoryMapName() const
{
    if (connectedPort_ == 0)
    {
        return QObject::tr("unspecified");
    }

    return connectedPort_->encompassingComp()->name() + "." +
           connectedPort_->getBusInterface()->getSlave()->getMemoryMapRef();
}

//-----------------------------------------------------------------------------
// Function: AddressEntry::isConnected()
//-----------------------------------------------------------------------------
bool AddressEntry::hasValidConnection() const
{
    return (connectedPort_ != 0 && connectedPort_->encompassingComp() != 0 &&
            connectedPort_->getBusInterface() != 0 &&
            connectedPort_->getBusInterface()->getInterfaceMode() == General::SLAVE);
}

//-----------------------------------------------------------------------------
// Function: AddressEntry::overlaps()
//-----------------------------------------------------------------------------
bool AddressEntry::overlaps(AddressEntry const& other) const
{
    return !(getEndAddress() < other.getStartAddress() || getStartAddress() > other.getEndAddress());
}

//-----------------------------------------------------------------------------
// Function: AddressEntry::getRangeInBytes()
//-----------------------------------------------------------------------------
unsigned int AddressEntry::getRangeInBytes()
{
    return range_ * aub_ / 8;
}

//-----------------------------------------------------------------------------
// Function: AddressEntry::getMemoryMapAUB()
//-----------------------------------------------------------------------------
unsigned int AddressEntry::getMemoryMapAUB()
{
    return aub_;
}
