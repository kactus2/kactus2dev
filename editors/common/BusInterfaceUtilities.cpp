//-----------------------------------------------------------------------------
// File: BusInterfaceUtilities.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.02.2019
//
// Description:
// Utilities for bus interface mode compatibilities.
//-----------------------------------------------------------------------------

#include "BusInterfaceUtilities.h"

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <library/LibraryInterface.h>

//-----------------------------------------------------------------------------
// Function: BusInterfaceUtilities::hasMatchingBusDefinitions()
//-----------------------------------------------------------------------------
bool BusInterfaceUtilities::hasMatchingBusDefinitions(QSharedPointer<const BusDefinition> firstDefinition,
    VLNV const& secondDefinitionVLNV, LibraryInterface* library)
{
    QSharedPointer<const BusDefinition> comparisonDefinition =
        library->getModelReadOnly(secondDefinitionVLNV).dynamicCast<const BusDefinition>();
    if (comparisonDefinition)
    {
        if (firstDefinition == comparisonDefinition)
        {
            return true;
        }
        else
        {
            VLNV comparisonExtendVLNV = comparisonDefinition->getExtends();
            VLNV firstExtendVLNV = firstDefinition->getExtends();

            if ((comparisonExtendVLNV.isValid() &&
                hasMatchingBusDefinitions(firstDefinition, comparisonExtendVLNV, library)) ||
                (firstExtendVLNV.isValid() &&
                    hasMatchingBusDefinitions(comparisonDefinition, firstExtendVLNV, library)))
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceUtilities::getIconPath()
//-----------------------------------------------------------------------------
QString BusInterfaceUtilities::getIconPath(General::InterfaceMode busMode)
{
    QString iconPath("");
    if (busMode == General::MASTER)
    {
        iconPath = ":icons/common/graphics/busInterfaceMaster.png";
    }
    else if (busMode == General::MIRROREDMASTER)
    {
        iconPath = ":icons/common/graphics/busInterfaceMirroredMaster.png";
    }
    else if (busMode == General::SLAVE)
    {
        iconPath = ":icons/common/graphics/busInterfaceSlave.png";
    }
    else if (busMode == General::MIRROREDSLAVE)
    {
        iconPath = ":icons/common/graphics/busInterfaceMirroredSlave.png";
    }
    else if (busMode == General::SYSTEM)
    {
        iconPath = ":icons/common/graphics/busInterfaceSystem.png";
    }
    else if (busMode == General::MIRROREDSYSTEM)
    {
        iconPath = ":icons/common/graphics/busInterfaceMirroredSystem.png";
    }
    else if (busMode == General::MONITOR)
    {
        iconPath = ":icons/common/graphics/busInterfaceMonitor.png";
    }

    return iconPath;
}
