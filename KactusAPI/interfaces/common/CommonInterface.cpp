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

//-----------------------------------------------------------------------------
// Function: CommonInterface::getUniqueName()
//-----------------------------------------------------------------------------
std::string CommonInterface::getUniqueName(std::string_view newName, std::string_view itemTypeName) const
{
    std::string referenceName(newName);
    if (referenceName.empty())
    {
        referenceName = itemTypeName;
    }

    std::string name(referenceName);
    int runningNumber = 0;
    while (!nameIsUnique(name))
    {
        name = referenceName + "_" + std::to_string(runningNumber);
        ++runningNumber;
    }

    return name;
}

//-----------------------------------------------------------------------------
// Function: CommonInterface::nameHasChanged()
//-----------------------------------------------------------------------------
bool CommonInterface::nameHasChanged(std::string_view newName, std::string_view oldName) const
{
    return newName != oldName;
}

//-----------------------------------------------------------------------------
// Function: CommonInterface::nameIsUnique()
//-----------------------------------------------------------------------------
bool CommonInterface::nameIsUnique(std::string_view name) const
{
    auto itemNames = getItemNames();
    return std::none_of(itemNames.cbegin(), itemNames.cend(),
        [&name](auto const& item) { return item == name; });
}
