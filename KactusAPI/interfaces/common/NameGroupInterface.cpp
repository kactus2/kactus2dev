//-----------------------------------------------------------------------------
// File: NameGroupInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.04.2020
//
// Description:
// Interface for name groups.
//-----------------------------------------------------------------------------

#include "NameGroupInterface.h"

//-----------------------------------------------------------------------------
// Function: NameGroupInterface::getDisplayName()
//-----------------------------------------------------------------------------
std::string NameGroupInterface::getDisplayName(std::string const& itemName) const
{
    if (QSharedPointer<NameGroup> item = getItem(itemName); item != nullptr)
    {
        return item->displayName().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: NameGroupInterface::setDisplayName()
//-----------------------------------------------------------------------------
bool NameGroupInterface::setDisplayName(std::string const& itemName, std::string const& newDisplayName) const
{
    QSharedPointer<NameGroup> item = getItem(itemName);
    if (item)
    {
        return false;
    }

    item->setDisplayName(QString::fromStdString(newDisplayName));
    return true;
}

//-----------------------------------------------------------------------------
// Function: NameGroupInterface::getDescription()
//-----------------------------------------------------------------------------
std::string NameGroupInterface::getDescription(std::string const& itemName) const
{
    if (QSharedPointer<NameGroup> item = getItem(itemName); item != nullptr)
    {
        return item->description().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: NameGroupInterface::setDescription()
//-----------------------------------------------------------------------------
bool NameGroupInterface::setDescription(std::string const& itemName, std::string const& newDescription)
{
    QSharedPointer<NameGroup> item = getItem(itemName);
    if (!item)
    {
        return false;
    }

    item->setDescription(QString::fromStdString(newDescription));
    return true;
}

//-----------------------------------------------------------------------------
// Function: NameGroupInterface::getShortDescription()
//-----------------------------------------------------------------------------
std::string NameGroupInterface::getShortDescription(std::string const& itemName) const
{
    if (QSharedPointer<NameGroup> item = getItem(itemName); item != nullptr)
    {
        return item->shortDescription().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: NameGroupInterface::setShortDescription()
//-----------------------------------------------------------------------------
bool NameGroupInterface::setShortDescription(std::string const& itemName, std::string const& newShortDescription) const
{
    if (QSharedPointer<NameGroup> item = getItem(itemName); item != nullptr)
    {
        item->shortDescription() = QString::fromStdString(newShortDescription);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: NameGroupInterface::NameGroupInterface()
//-----------------------------------------------------------------------------
NameGroupInterface::NameGroupInterface():
CommonInterface()
{

}
