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
    QString referencePortName(QString::fromStdString(newName));
    if (referencePortName.isEmpty())
    {
        referencePortName = QString::fromStdString(itemTypeName);
    }

    QString newPortName(referencePortName);

    QString format(QLatin1String("$itemName$_$itemNumber$"));
    int runningNumber = 0;
    while (!nameIsUnique(newPortName))
    {
        newPortName = format;
        newPortName.replace("$itemName$", referencePortName);
        newPortName.replace("$itemNumber$", QString::number(runningNumber));

        runningNumber++;
    }

    return newPortName;
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
bool CommonInterface::nameIsUnique(QString const& portName) const
{
    for (auto containedName : getItemNames())
    {
        QString convertedName(QString::fromStdString(containedName));
        if (convertedName == portName)
        {
            return false;
        }
    }

    return true;
}
