//-----------------------------------------------------------------------------
// File: AutoConnectorItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 11.02.2019
//
// Description:
// Item used by component item auto connector.
//-----------------------------------------------------------------------------

#include "AutoConnectorItem.h"

//-----------------------------------------------------------------------------
// Function: AutoConnectorItem::AutoConnectorItem()
//-----------------------------------------------------------------------------
AutoConnectorItem::AutoConnectorItem(QString const& itemName, QString const& componentItemName,
    AutoConnectorItem::ItemType itemType, AutoConnectorItem::ContainerType containerType):
itemName_(itemName),
containingItemName_(componentItemName),
itemType_(itemType),
containterType_(containerType)
{

}

//-----------------------------------------------------------------------------
// Function: AutoConnectorItem::AutoConnectorItem()
//-----------------------------------------------------------------------------
AutoConnectorItem::AutoConnectorItem(const AutoConnectorItem &other):
itemName_(other.itemName_),
containingItemName_(other.containingItemName_),
itemType_(other.itemType_)
{

}

//-----------------------------------------------------------------------------
// Function: AutoConnectorItem::operator=()
//-----------------------------------------------------------------------------
AutoConnectorItem & AutoConnectorItem::operator=(const AutoConnectorItem &other)
{
    if (this != &other)
    {
        itemName_ = other.itemName_;
        containingItemName_ = other.containingItemName_;
        itemType_ = other.itemType_;
    }
    return *this;
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorItem::~AutoConnectorItem()
//-----------------------------------------------------------------------------
AutoConnectorItem::~AutoConnectorItem()
{

}

//-----------------------------------------------------------------------------
// Function: AutoConnectorItem::getName()
//-----------------------------------------------------------------------------
QString AutoConnectorItem::getName() const
{
    return itemName_;
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorItem::setName()
//-----------------------------------------------------------------------------
void AutoConnectorItem::setName(QString const& newName)
{
    itemName_ = newName;
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorItem::getContainingItem()
//-----------------------------------------------------------------------------
QString AutoConnectorItem::getContainingItem() const
{
    return containingItemName_;
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorItem::setContainingItem()
//-----------------------------------------------------------------------------
void AutoConnectorItem::setContainingItem(QString const& newComponentItem)
{
    containingItemName_ = newComponentItem;
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorItem::getItemType()
//-----------------------------------------------------------------------------
AutoConnectorItem::ItemType AutoConnectorItem::getItemType() const
{
    return itemType_;
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorItem::setNewItemType()
//-----------------------------------------------------------------------------
void AutoConnectorItem::setNewItemType(AutoConnectorItem::ItemType newItemType)
{
    itemType_ = newItemType;
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorItem::getContainterType()
//-----------------------------------------------------------------------------
AutoConnectorItem::ContainerType AutoConnectorItem::getContainterType() const
{
    return containterType_;
}

//-----------------------------------------------------------------------------
// Function: AutoConnectorItem::setNewContainerType()
//-----------------------------------------------------------------------------
void AutoConnectorItem::setNewContainerType(AutoConnectorItem::ContainerType newContainerType)
{
    containterType_ = newContainerType;
}
