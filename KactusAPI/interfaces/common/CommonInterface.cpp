//-----------------------------------------------------------------------------
// File: CommonInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.04.2020
//
// Description:
// Common functions for interfaces.
//-----------------------------------------------------------------------------

#include "CommonInterface.h"

bool CommonInterface::isNameUnique(std::string name, std::string* previousName) const
{
    auto items = getItemNames();
    if (previousName) 
    {
        auto it = std::find(items.begin(), items.end(), *previousName);
        if (it != items.end())
        {
            items.erase(it);
        }
    }
    return isNameUniqueInCollection(name.c_str(), items);
}

//-----------------------------------------------------------------------------
// Function: CommonInterface::getUniqueName()
//-----------------------------------------------------------------------------
QString CommonInterface::getUniqueName(std::string const& newName, std::string const& itemTypeName) const
{
    std::string referenceName = newName;
    if (referenceName.empty())
    {
        referenceName = itemTypeName;
    }

    std::string name = referenceName;
    int runningNumber = 0;
    auto const items = getItemNames();
    while (!isNameUniqueInCollection(name, items))
    {
        name = referenceName + "_" + std::to_string(runningNumber);
        runningNumber++;
    }

    return QString::fromStdString(name);
}

//-----------------------------------------------------------------------------
// Function: CommonInterface::nameHasChanged()
//-----------------------------------------------------------------------------
bool CommonInterface::nameHasChanged(std::string const& newName, std::string const& oldName) const
{
    return newName != oldName;
}

//-----------------------------------------------------------------------------
// Function: CommonInterface::isNameUniqueInCollection()
//-----------------------------------------------------------------------------
bool CommonInterface::isNameUniqueInCollection(std::string_view name, std::vector<std::string> const& reservedNames) const
{
    return std::find(reservedNames.cbegin(), reservedNames.cend(), name) == reservedNames.cend();
}
