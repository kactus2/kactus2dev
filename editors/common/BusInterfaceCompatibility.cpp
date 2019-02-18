//-----------------------------------------------------------------------------
// File: BusInterfaceCompatibility.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.02.2019
//
// Description:
// Utilities for bus interface mode compatibilities.
//-----------------------------------------------------------------------------

#include "BusInterfaceCompatibility.h"

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <library/LibraryInterface.h>

//-----------------------------------------------------------------------------
// Function: BusInterfaceCompatibility::hasMatchingBusDefinitions()
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
