////-----------------------------------------------------------------------------
// File: BusDefinitionUtils.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.03.2019
//
// Description:
// Utilities for Bus Definitions and abstraction definitions.
//-----------------------------------------------------------------------------

#include "BusDefinitionUtils.h"

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <library/LibraryInterface.h>

//-----------------------------------------------------------------------------
// Function: BusDefinitionUtils::getSystemGroups()
//-----------------------------------------------------------------------------
IPXACTMODELS_EXPORT QStringList BusDefinitionUtils::getSystemGroups(QSharedPointer<const BusDefinition> bus,
    LibraryInterface* libraryAccess)
{
    QStringList systemGroups = bus->getSystemGroupNames();

    VLNV& extendVLNV = bus->getExtends();
    if (extendVLNV.isValid() && extendVLNV.getType() == VLNV::BUSDEFINITION)
    {
        QSharedPointer<const Document> extendDocument = libraryAccess->getModelReadOnly(extendVLNV);
        if (extendDocument)
        {
            QSharedPointer<const BusDefinition> extendBus = extendDocument.dynamicCast<const BusDefinition>();
            if (extendBus)
            {
                systemGroups.append(getSystemGroups(extendBus, libraryAccess));
            }
        }
    }

    return systemGroups;
}
