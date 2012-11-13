//-----------------------------------------------------------------------------
// File: AddressBlockItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 02.11.2012
//
// Description:
// Address block graphics item.
//-----------------------------------------------------------------------------

#include "AddressBlockItem.h"

#include <models/component.h>
#include <models/addressspace.h>

//-----------------------------------------------------------------------------
// Function: AddressBlockItem::AddressBlockItem()
//-----------------------------------------------------------------------------
AddressBlockItem::AddressBlockItem(QSharedPointer<Component> component,
                                   QSharedPointer<MemoryMap> memoryMap,
                                   QString const& name, quint64 startAddress,
                                   quint64 range, QGraphicsItem* parent)
    : AddressSectionItem(name, startAddress, range, parent),
      component_(component),
      memoryMap_(memoryMap)
{
}

//-----------------------------------------------------------------------------
// Function: AddressBlockItem::~AddressBlockItem()
//-----------------------------------------------------------------------------
AddressBlockItem::~AddressBlockItem()
{
}
