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
    while (!nameIsUnique(name, items))
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
// Function: CommonInterface::nameIsUnique()
//-----------------------------------------------------------------------------
bool CommonInterface::nameIsUnique(std::string_view name, std::vector<std::string> const& reservedNamed) const
{
    return std::find(reservedNamed.cbegin(), reservedNamed.cend(), name) == reservedNamed.cend();
}
